#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// Send message
void *send_msg(void *socket) {

	// Allocate send buffer
	char *buffer = malloc(40*sizeof(char));

	// Get input and send to socket loop
	int n;
	while(1) {
		n=0;

		// Write characters to buffer until new line
		while((buffer[n++] = getchar())!='\n');

		// Add string termination
		buffer[n-1]='\0';

		// Write the buffer to socket
		write(*((int *)socket), buffer, 40*sizeof(char));

		// End communication when bye is typed
		if(strcmp(buffer, "bye")==0) {
			printf("Send ended\n");
			break;
		}
	}

	// Free memory
	free(buffer);	
}

// Receive message
void *recv_msg(void *socket) {
	
	// Typecast socket to integer
	int sock = *((int *)socket);
	
	// Allocate receive buffer
	char *buffer = malloc(40*sizeof(char));

	// Continously read receive buffer and print to output
	while(1) {
		read(sock, buffer, sizeof(char)*40);

		// If first character exists in buffer
		if(buffer[0]){

			// End communication when bye is received 
			if(strcmp(buffer, "bye")==0) {
				printf("Receive ended\n");
				break;
			}

			// Print out received message
			printf("Server: %s\n", buffer);

			// Reset buffer
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

	// Catch socket creation error
	if(tcp_socket < 0) {
		perror("Error: socket\n");
		return 1;
	}

	// Initialise struct with relevant protocol, port and ip address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(1024);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

	// Typecast to general struct required for function use
	struct sockaddr *addr = (struct sockaddr *)&address;

	// Connect to server at specified address/port 
	if(connect(tcp_socket, addr, sizeof(struct sockaddr)) < 0) {
		perror("Error: connect\n");
		return 1;
	}
	printf("Successfully connected\n\n");

	// Create and start send/receive threads for communication with server

	pthread_t send_thread, receive_thread;
	pthread_create(&send_thread, NULL, send_msg, &tcp_socket);
	pthread_create(&receive_thread, NULL, recv_msg, &tcp_socket);
	pthread_join(send_thread, NULL);
	pthread_join(receive_thread, NULL);

	printf("Client ended\n");

	return 0;

}
	
