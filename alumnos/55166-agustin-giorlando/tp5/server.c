#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "service.h"

int main(int argc, char **argv){
	
	struct sockaddr_in direccion = {};
	struct conn_param param;
	int sockfd,val = 1;
	pthread_t tid;

	pthread_mutex_init(&param.mutex,NULL);

	/*CREACION DE LOG*/
	if ((param.logfd = open("./log.txt",O_WRONLY|O_CREAT|O_TRUNC,0664)) < 0) {
		perror("Creacion de log");
	}else{
		write(param.logfd,"Archivo de log creado\n",22);
	}

	/*CREACION DE SOCKET*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Creacion de socket");
		write(param.logfd,"ERROR: Creacion de socket\n",26);
		exit(EXIT_FAILURE);
	}else{
		write(param.logfd,"Socket creado\n",14);
	}

	/*DIRECCION*/
	direccion.sin_family = AF_INET;	//FAMILIA
	direccion.sin_port = htons(5000);	//PUERTO
	direccion.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/*CONFIGURACION DEL SOCKET*/
	if(setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, &val, sizeof (int)) == -1){
		perror("Opciones del socket");
		write(param.logfd,"ERROR: Opciones del socket\n",27);
		exit(EXIT_FAILURE);
	}else{
		write(param.logfd,"Socket configurado\n",19);
	}

	/*ENLACE*/
	if (bind(sockfd, (struct sockaddr *) &direccion, sizeof direccion) != 0){
		perror("Enlace");
		write(param.logfd,"ERROR: Enlace\n",14);
		exit(EXIT_FAILURE);
	}else{
		write(param.logfd,"Enlace creado\n",14);
	}

	/*SERVIDOR EN ESCUCHA*/
	if(listen(sockfd, 5) != 0){
		perror("Servidor en escucha");
		write(param.logfd,"ERROR: Servidor en escucha\n",28);
		exit(EXIT_FAILURE);
	}else{
		write(param.logfd,"Servidor puesto en escucha\n",27);
	}

	/*CONEXIONES*/
	while ((param.conn = accept(sockfd, NULL, 0)) > 0){
		
		/*MUTEX*/
		if ((pthread_mutex_lock(&param.mutex)) != 0){
			perror("Mutex");
			write(param.logfd,"ERROR: Lock de mutex\n",21);
			exit(EXIT_FAILURE);
		}		
		
		/*THREADS*/
		if((pthread_create(&tid,NULL,service,&param)) == 0){	
		write(param.logfd,"Conexion establecida\n",21);
		}else{
			perror("Servicio");
			write(param.logfd,"ERROR: No se pudo iniciar el servicio\n",36);
			exit(EXIT_FAILURE);
		}
	}	
	close(sockfd);
return 0;
}

