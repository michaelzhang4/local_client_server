#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

// Function for receiving data from udp socket
void *recv_thread(void *socket) {
	// Socket number
	int sock = *(int *)socket;
	// Buffer to store data
	char *buffer = malloc(40);
	while(1) {
		// Read in data
		read(sock, buffer, sizeof(char)*40);
		
		// Stop sending when exit is typed
		if(buffer) {
			if(strcmp(buffer, "exit")==0) {
				printf("Exiting server receive\n");
				break;
			}
			printf("Sender: %s\n", buffer);
			buffer[0]='\0';
			sleep(0.1);
		}
	}
	
	// Free memory allocated
	free(buffer);
}


int main() {

	// Create udp socket
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

	// Initialise struct with protocol type, port, ip address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(1024);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr.s_addr);

	// Cast to general sockaddr struct for use
	struct sockaddr *addr = (struct sockaddr *)&address;

	// Bind socket
	bind(udp_socket, addr, sizeof(struct sockaddr));
	
	// Start a thread for receiving data
	pthread_t receive_thread;

	pthread_create(&receive_thread, NULL, recv_thread,&udp_socket);

	pthread_join(receive_thread, NULL);

	printf("Server ended\n");
}
