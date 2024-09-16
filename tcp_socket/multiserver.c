#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>

#define MAX_CONN 3

// Global exit flag
int __exit = 1;

// Flag lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Arguments for send_msg
struct args {
	char **buff;
	pthread_mutex_t *lock;
};

// Fills send buffer
void *send_msg(void *arg) {

	// Dereference arguments
	struct args *arguments = (struct args *)arg;

	// Get buffer from args 
	char **buffer = arguments->buff;

	// Get lock from args
	pthread_mutex_t *lock = arguments->lock;

	// Create function buffer
	char *b = malloc(40);

	// Continously get input and copy to global buffer
	int n;
	while(1) {
		n=0;
		
		// Push inputted characters into buffer
		while((b[n++]=getchar())!='\n');
		b[n-1]='\0';

		// Lock buffer and copy buffer to global buffer space
		pthread_mutex_lock(lock);
		for(int i=0;i<MAX_CONN;i++) {
			strcpy(buffer[i],b);
		}
		
		// Unlock buffer
		pthread_mutex_unlock(lock);

		// If exit is typed then stop server
		if(strcmp(b,"exit")==0) {
			printf("Server ended\n");
			
			// Change exit state globally
			pthread_mutex_lock(lock);
			__exit = 0;
			pthread_mutex_unlock(lock);
			break;
		}
	}
	
	// Free memory
	free(b);
}

// Receive and print messages received
void *recv_msg(void *socket) {

	// Receive buffer
	char *buffer = malloc(40);

	// Typecast and dereference socket to int
	int sock = *(int *)socket;

	// Continously read received messages to buffer
	while(1) {
		pthread_mutex_lock(&lock);
		if (__exit == 0) {
			pthread_mutex_unlock(&lock);
			break;
		}
		pthread_mutex_unlock(&lock);
		read(sock, buffer, 40);

		// Print out message if buffer contains new message
		if(buffer[0]) {

			// Print out message
			printf("Client %d: %s\n",sock-3,buffer);

			// Reset buffer
			*buffer = '\0';
		}
		sleep(0.1);
	}

	// Free memory
	free(buffer);
}


int main() {

	// Size of sockaddr
	int addrlen = sizeof(struct sockaddr);

	// Create memory for sockets
	int *sockets = malloc(MAX_CONN*sizeof(int));
	memset(sockets,0,MAX_CONN*sizeof(int));

	// Initialise struct with relevant protocol, port and ip address
	struct sockaddr_in address;
	address.sin_family=AF_INET;
	address.sin_port=htons(1024);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

	// Global write buffer allocation
	char **write_buffer = malloc(sizeof(char **)*MAX_CONN);

	for(int i=0;i<MAX_CONN;i++){
		write_buffer[i] = malloc(40);
	}

	// Global read buffer allocation
	char *buffer = malloc(MAX_CONN * 40);

	// Create tcp socket for accepting connections
	int server_sock = socket(AF_INET, SOCK_STREAM,0);
	if(server_sock==-1){
		perror("Socket error");
		exit(1);
	}

	// Bind socket to address and port
	if(bind(server_sock, (struct sockaddr *)&address, sizeof(struct sockaddr))<0) {
		perror("Bind error");
		exit(1);
	}
	
	// Set socket to listen with a total backlog of 3 connections that can be queued
	if(listen(server_sock,MAX_CONN)<0){
		perror("Listen error");
		exit(1);
	}

	printf("Server started successfully\n");

	// Create arguments
	struct args arguments;
	arguments.buff = write_buffer;
	arguments.lock = &lock;

	// Create a send_msg thread and detach it(runs and terminates independently)
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, send_msg,(void *)&arguments);
	pthread_detach(thread_id);


	// Holds the number of file descriptors ready for I/O operations 
	int ready;

	// Holds value of the highest file descriptor to use for range of file descriptors to monitor
	int maxfd;

	// File descriptors for reading and writing
	fd_set readfds;
	fd_set writefds;

	// Timeout for select
    struct timeval timeout;
    timeout.tv_sec = 1;  // Set timeout for 1 second
    timeout.tv_usec = 0;

	// Continously check possible I/O operations on sockets
	while(1) {

		pthread_mutex_lock(&lock);
		if (__exit == 0) {
			pthread_mutex_unlock(&lock);
			break;
		}
		pthread_mutex_unlock(&lock);
		
		// Initalises read and write file descriptor sets
		FD_ZERO(&writefds);
		FD_ZERO(&readfds);

		// Adds socket to read set to monitor it for incoming messages
		FD_SET(server_sock, &readfds);

		// Set maxfd to socket number
		maxfd = server_sock;

		// Add all active socket connections to read and write sets
		for (int i=0;i<MAX_CONN;i++) {
			if(sockets[i]>0) {
				FD_SET(sockets[i], &readfds);
				FD_SET(sockets[i], &writefds);
				if(sockets[i]>maxfd){
					maxfd=sockets[i];
				}
			}
		}

		// Returns the number of connections that are active
		ready = select(maxfd+1, &readfds, &writefds, NULL, &timeout);
		if(ready<0){
			perror("Select error");
			exit(1);
		}

		// Checks if socket is still in read set
		if(FD_ISSET(server_sock, &readfds)) {

			// If clients are trying to connect, accept as a new socket connection
			int new_sock;
			if((new_sock=accept(server_sock,(struct sockaddr *)&address,&addrlen))<0){
				perror("Accept error");
				exit(1);
			}

			// Find next empty space in socket connections
			for(int i=0;i<MAX_CONN;i++){
				if(sockets[i]==0){
					sockets[i]=new_sock;
					break;
				}
			}
		}

		// Check every active connection for read/write operations
		for(int i=0;i<MAX_CONN;i++) {

			// Skip if no active connection
			if(sockets[i]==0)
				continue;

			// Check if socket is still valid
			if(FD_ISSET(sockets[i], &readfds)) {

				// Read data into buffer
				int bytes_read = read(sockets[i], buffer, 40);
				if (bytes_read < 0) {
					perror("Read error");

				// If bytes were read
				} else if (bytes_read>0){
					
					// End connection when exit is received
					if(strcmp(buffer, "exit")==0){
						printf("Sender %d ended connection\n", i+1);
						sockets[i]=0;
					} else {
						// Print out sender message and reset buffer
						printf("Sender %d: %s\n", i+1, buffer);
						buffer[bytes_read] = '\0';
					}
				} else {
					
					// Sleeps program for 1 second
					usleep(100000);
				}
			}

			// Prevent write_buffer data race conditions by locking	
			pthread_mutex_lock(&lock);

			// Check write buffer
			if(write_buffer[i][0]) {

				// Validate socket connection
				if(FD_ISSET(sockets[i], &writefds)) {

					// Write to connection and reset buffer
					write(sockets[i], write_buffer[i], 40);
					write_buffer[i][0]='\0';
				}
			}

			// Unlock
			pthread_mutex_unlock(&lock);
		}
	}

	// Free memory	
	for(int i=0;i<MAX_CONN;i++) {
		free(write_buffer[i]);
	}
	free(write_buffer);

	free(buffer);

	// Close all sockets
    for (int i = 0; i < MAX_CONN; i++) {
        if (sockets[i] > 0) {
            close(sockets[i]);
        }
    }

	// Close the server socket
    close(server_sock);

    // Free the socket array
    free(sockets);


	// Exit program
	exit(0);
}
