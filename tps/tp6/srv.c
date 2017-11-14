#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "service.h"

int main(int argc, char **argv)
{
	char buff[1024];
	struct sockaddr_in addr = {};
	int sockfd, conn, leido,val=1;

	printf("Direccion: %s\n",argv[1]);
	printf("Puerto: %s\n",argv[2]);

 	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) { 
		perror("socket()");
		exit(EXIT_FAILURE); 
	}

	/* ADDR. FAMILY */
	addr.sin_family= AF_INET;
	/* PORT */
	addr.sin_port = htons(atoi(argv[2]));
	/* ADDRESS */
	inet_pton(AF_INET, argv[1], &addr.sin_addr);
	
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof (int)) == -1){
		perror("setsockopt()");
		exit(EXIT_FAILURE);
	}
	
	if (bind(sockfd, (struct sockaddr *) &addr, sizeof (addr)) < 0) { 
		perror("bind()");
		exit(EXIT_FAILURE); 
	}

	if (listen(sockfd, 5) < 0) { 
		perror("listen()");
		exit(EXIT_FAILURE); 
	}

	while ((conn = accept(sockfd, NULL, 0)) > 0) {
		switch (fork()) {
			case 0:
                close(sockfd);
                service(conn);
				return 0;

			case -1: // error
				perror("fork()");
				return 1;
		}

		close(conn);
	}

	return 0;	
}

