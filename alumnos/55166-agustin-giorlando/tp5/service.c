#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

#include "service.h"

#define http_status "HTTP/1.0 %s\r\n"
#define content_type "Content-Type: %s/%s\r\n\r\n"

int set_status(char const * new_status,int sdc){
	
	char status[1024];
	
	snprintf(status,sizeof(status),http_status,new_status);
	if ((write(sdc,status,strlen(status))) == -1){
	return -1;
	}

return 0;
}

int set_header(char const * type,char const * extension,int sdc){

	char header[1024];

	snprintf(header,sizeof(header),content_type,type,extension);
	if ((write(sdc,header,strlen(header))) == -1){
	return -1;
	}

return 0;
}

void* service (void* parameters){
	struct conn_param *p;
	p = (struct con_param*)parameters;
	
	int sdc = p->conn;

	int leido,fd;
	char request[1024],reply[1024000],*method,*name,*extension,file[20];
 	
	pthread_detach(pthread_self());

	//MUTEX
	if ((pthread_mutex_unlock(&p->mutex)) != 0){
		write(p->logfd,"ERROR: Unlock de mutex\n",23);
		close(fd);
		close(sdc);
	pthread_exit(NULL);
	}


	//LECTURA DE REQUISITO
	if ((leido = read(sdc,request,sizeof request)) < 0){
		write(p->logfd,"ERROR: Lectura de requisito\n",28);
		close(fd);
		close(sdc);
	pthread_exit(NULL);
	}
	
	method = strtok(request," ");
	name = strtok(NULL," /.");
	extension = strtok(NULL," /.");

	strcpy(file,name);
	strcat(file,".");
	strcat(file,extension);
	
	//ESCRITURA DE CABECERA
	if ((fd = open(file,O_RDONLY,0664)) == -1){
		if (errno == 2){	
			if ((set_status("404 Not found",sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de estado\n",27);
			}else{
				write(p->logfd,"404 NOT FOUND\n",14);
			}

			if ((set_header("text",extension,sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de cabecera\n",29);
			}

			if ((write(sdc,"404 NOT FOUND",13)) == -1){
				write(p->logfd,"ERROR: Escritura de mensaje\n",28);
			}

		}else if (errno == 13){
			if ((set_status("403 Forbidden",sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de estado\n",27);
			}else{
				write(p->logfd,"403 FORBIDDEN\n",14);
			}

			if ((set_header("text",extension,sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de cabecera\n",29);
			}

			if ((write(sdc,"403 FORBIDDEN",14)) == -1){
				write(p->logfd,"ERROR: Escritura de mensaje\n",28);
			}

		}else if (errno != 0) {	
			if ((set_status("505 Internal error",sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de estado\n",27);
			}else{
				write(p->logfd,"505 INTERNAL ERROR\n",19);
			}

			if ((set_header("text",extension,sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de cabecera\n",29);
			}

			if ((write(sdc,"505 INTERNAL ERROR",18)) == -1){
				write(p->logfd,"ERROR: Escritura de mensaje\n",28);
			}
		}

		close(fd);
		close(sdc);			
	pthread_exit(NULL);
	}

	if ((set_status("200 OK",sdc)) == -1){
		write(p->logfd,"ERROR: Escritura de estado\n",27);
		close(fd);
		close(sdc);
	pthread_exit(NULL);
	}else{
		write(p->logfd,"200 OK\n",7);
	}

	//ARCHIVO HTML
	if (strcmp(extension,"html") == 0){
		while((leido = read(fd,reply,sizeof reply)) > 0){
			if ((set_header("text",extension,sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de cabecera\n",29);
				close(fd);
				close(sdc);			
			pthread_exit(NULL);
			}

			if ((write(sdc,reply,leido)) == -1){
				write(p->logfd,"ERROR: Escritura HTML\n",22);
				close(fd);
				close(sdc);			
			pthread_exit(NULL);
			}else{
				write(p->logfd,"ARCHIVO HTML\n",13);
			}
		}	
	}

	//ARCHIVO PDF
	if (strcmp(extension,"pdf") == 0){
		while((leido = read(fd,reply,sizeof reply)) > 0){
			if ((set_header("text",extension,sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de cabecera\n",29);
				close(fd);
				close(sdc);			
			pthread_exit(NULL);
			}

			if ((write(sdc,reply,leido)) == -1){
				write(p->logfd,"ERROR: Escritura PDF\n",21);
				close(fd);
				close(sdc);			
			pthread_exit(NULL);
			}else{
				write(p->logfd,"ARCHIVO PDF\n",12);
			}
		}
	}

	//ARCHIVO IMAGEN
	if (strcmp(extension,"png") == 0 || strcmp(extension,"jpg") == 0 || strcmp(extension,"gif") == 0){
		while((leido = read(fd,reply,sizeof reply)) > 0){
			if ((set_header("image",extension,sdc)) == -1){
				write(p->logfd,"ERROR: Escritura de cabecera\n",29);
				close(fd);
				close(sdc);			
			pthread_exit(NULL);
			}
			if ((write(sdc,reply,leido)) == -1){
				write(p->logfd,"ERROR: Escritura IMAGEN\n",24);
				close(fd);
				close(sdc);			
			pthread_exit(NULL);
			}else{
				write(p->logfd,"ARCHIVO IMAGEN\n",15);
			}
		}
	}

	close(fd);
	close(sdc);			
pthread_exit(NULL);
}

