#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

// Send message
void *send_msg(void *socket) {

	// Allocate send buffer
	char *buffer = malloc(40*sizeof(char));

	// Typecast socket to integer
	int sock = *((int *)socket);

	// Get input and send to socket loop
	int n;
	while(1) {
		n=0;

		// Writes characters to buffer until new line
		while((buffer[n++]=getchar())!='\n');

		// Add string termination
		buffer[n-1]='\0';

		// Write the buffer to socket
		write(sock, buffer, sizeof(char)*40);

		// Stop sending when bye is typed
		if(strcmp(buffer,"bye")==0) {
			printf("Server send ended\n");
			break;
		}
	}

	// Free memory
	free(buffer);
}

// Receive message
void *recv_msg(void *socket) {

	// Allocate buffer
	char *buffer = malloc(40*sizeof(char));

	// Typecast socket to int
	int sock = *((int *)socket);

	// Continously read and print data from socket buffer
	while(1) {
		read(sock, buffer, sizeof(char)*40);
		if(buffer[0]){
			
			// Stop receiving when bye is received
			if(strcmp(buffer, "bye")==0) {
				printf("Server receive ended\n");
				break;
			}

			// Print out message
			printf("Client: %s\n", buffer);
			*buffer = '\0';
		}
		sleep(0.1);
	}

	// Free memory
	free(buffer);
}

int main() {

	// Create tcp socket
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Catch error on socket creation
	if (tcp_socket < 0) {
		perror("There was an error setting socket\n");
		return 1;
	}
	
	// Initialise struct with relevant protocol, port and ip address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(1024);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

	// Typecast to general struct
	struct sockaddr *addr = (struct sockaddr *)&address;

	// Bind socket to port
	if(bind(tcp_socket,addr, sizeof(struct sockaddr)) < 0) {
		perror("There was an error binding socket to address\n");
		return 1;
	}

	// Set socket to listen
	if(listen(tcp_socket, 1) < 0) {
		perror("Error on socket listen\n");
		return 1;
	}
	printf("Server listening on port 1024\n");
	
	// Accept connections on listening socket and create new socket for further communication
	socklen_t addrlen = sizeof(struct sockaddr);
	int new_sock = accept(tcp_socket, addr, &addrlen);
	if(new_sock < 0) {
	perror("Error on accepting socket\n");
		return 1;
	}

	printf("Connection with client established\n\n");

	// Create and start send/receive threads for communication through new socket
	pthread_t send_thread, receive_thread;
	pthread_create(&send_thread, NULL, send_msg, &new_sock);
	pthread_create(&receive_thread, NULL, recv_msg, &new_sock);
	pthread_join(send_thread, NULL);
	pthread_join(receive_thread, NULL);

	printf("Server ended\n");

	return 0;

}
