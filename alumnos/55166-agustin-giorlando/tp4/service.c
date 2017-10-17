#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "service.h"

#define http_status "HTTP/1.0 %s\r\n"
#define content_type "Content-Type: %s/%s\r\n\r\n"

void set_status(char const * new_status,int sdc){
	
	char status[1024];
	
	snprintf(status,sizeof(status),http_status,new_status);
	write(sdc,status,strlen(status));
}

void set_header(char const * type,char const * extension,int sdc){
	char header[1024];

	snprintf(header,sizeof(header),content_type,type,extension);
	write(sdc,header,strlen(header));
}

int service(int sdc){
	
	int leido,fd;
	char request[1024],reply[1024000],*method,*name,*extension,file[20];

	/*LECTURA DE REQUISITO*/
	leido = read(sdc,request,sizeof request);

	method = strtok(request," ");
	name = strtok(NULL," /.");
	extension = strtok(NULL," /.");

	strcpy(file,name);
	strcat(file,".");
	strcat(file,extension);
	
	/*ESCRITURA DE CABECERA*/
	if (strcmp(method,"GET") == 0){
		if((fd = open(file,O_RDONLY,0664)) != 0){
			if(errno == 2){	
				set_status("404 Not found",sdc);
				set_header("text",extension,sdc);
				write(sdc,"404 NOT FOUND",13);
			return -1;
			}else if(errno == 13){
				set_status("403 Forbidden",sdc);
				set_header("text",extension,sdc);
				write(sdc,"403 FORBIDDEN",14);
			return-2;
			}
		}	
	
	}else{
		set_status("505 Internal error",sdc);
		set_header("text",extension,sdc);
		write(sdc,"505 INTERNAL ERROR",18);
	return -3;
	}

	set_status("200 OK",sdc);

	/*ARCHIVO HTML*/
	if (strcmp(extension,"html") == 0){
		while((leido = read(fd,reply,sizeof reply)) > 0){
			set_header("text",extension,sdc);
			write(sdc,reply,leido);
		}
	return 0;
	}

	/*ARCHIVO PDF*/
	if (strcmp(extension,"pdf") == 0){
		while((leido = read(fd,reply,sizeof reply)) > 0){
			set_header("application",extension,sdc);
			write(sdc,reply,leido);
		}
	return 0;
	}

	/*ARCHIVO IMAGEN*/
	if (strcmp(extension,"png") == 0 || strcmp(extension,"jpg") == 0 || strcmp(extension,"gif") == 0){
		while((leido = read(fd,reply,sizeof reply)) > 0){
			set_header("image",extension,sdc);
			write(sdc,reply,leido);
		}
	return 0;
	}
	close(sdc);

return 0;
}
