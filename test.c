
#include <stdio.h>
#include <cutils/sockets.h>
#include <sys/socket.h>
#include <unistd.h>

#include "net_wrapper.h"

#define PORT 12345



int commandHandler(int sock, int cmd, const void *data, int sz) {

	switch (cmd) {
		case 1:
			printf("%s\n", (char*)data);
			sendCommand(sock, 919, (const void *)data, sz);
			return 0;
		case 99:
			printf("%s\n", (char*)data);
			sendCommand(sock, 919, (const void *)data, sz);
			return 0;

		case 919:
			printf("%s\n", (char*)data);
			sendCommand(sock, 919, (const void *)data, sz);
			return 0;			
		default:
			return -1;
	}
}


int main(int argc, char *argv[])
{
	int sock, clientsock;

	sock = socket_inaddr_any_server(PORT, SOCK_STREAM);
	if (sock < 0) {
		fprintf(stderr,"Unable to start listening server...\n");
		return -1;
	}

	while(1) {
		clientsock = accept(sock, NULL, NULL);
	    if (clientsock < 0) {
	      fprintf(stderr, "Unable to accept connection...\n");
	      break;
	  	}

	  	recvHandler(clientsock, commandHandler);
	  	close(clientsock);
	}
	
	close(sock);

    return 0;
}


