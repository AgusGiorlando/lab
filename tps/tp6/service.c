#include "service.h"

int service(int sdc) {
	char buff[1024];
	int leido;
	char *http_header = "HTTP/1.0 200 Ok\r\nContent-Type: text/plain\r\n\r\n";

	leido = read(sdc, buff, sizeof buff);

	printf("El cliente me escribio %s \n", buff);

	write(sdc, http_header, strlen(http_header)); 
	write(sdc, buff, leido);

	close(sdc);
    return 0;
}

