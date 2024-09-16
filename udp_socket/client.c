#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main() {
	// Create a udp socket with IPv4
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

	// Initialise struct with relevant protocol, port and ip address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(1024);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr.s_addr);
	
	// Cast to general sockaddr for use
	struct sockaddr *addr = (struct sockaddr *)&address;

	// Initialise send buffer
	char *buffer = malloc(40*sizeof(char));

	int n;
	// Continously read and send from terminal input
	while(1) {
		n=0;
		// Writes characters to buffer until new line
		while((buffer[n++]=getchar())!='\n');
		// Add string termination
		buffer[n-1]='\0';

		// Send the buffer through socket
		sendto(udp_socket, buffer, sizeof(char)*40, 0, addr, sizeof(struct sockaddr));

		// Exit program when exit is typed
		if(strcmp(buffer, "exit")==0) {
			break;
		}
	}
	printf("Client ended\n");

	// Free dynamically allocated memory
	free(buffer);
}
