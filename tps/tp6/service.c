#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "service.h"


int service(int sdc) {
	char buff[1024],*metodo;
	int leido;
	char *http_header = "HTTP/1.0 200 Ok\r\nContent-Type: text/plain\r\n\r\n";
	char *http400_header = "HTTP/1.0 400 BAD REQUEST\r\nContent-Type: text/plain\r\n\r\n";

	leido = read(sdc, buff, sizeof buff);

	metodo = strtok(buff," ");

	if (strcmp(metodo,"GET") == 0){
		printf("El cliente me escribio %s \n", buff);
		write(sdc, http_header, strlen(http_header)); 
		write(sdc, buff, leido);
	}else{
		write(sdc, http400_header, strlen(http_header)); 
		write(sdc, buff, leido);
	}
	close(sdc);
    return 0;
}

