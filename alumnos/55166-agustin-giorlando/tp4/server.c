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

#include "service.h"

int main(int argc, char **argv){
	
	struct sockaddr_in direccion = {};
	int sockfd,logfd,conn,val = 1;

	/*CREACION DE LOG*/
	if ((logfd = open("./log.txt",O_WRONLY|O_CREAT|O_TRUNC,0664)) < 0) {
		perror("Creacion de log");
	}else{
		write(logfd,"Archivo de log creado\n",22);
	}

	/*CREACION DE SOCKET*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Creacion de socket");
		write(logfd,"ERROR: Creacion de socket\n",26);
		exit(EXIT_FAILURE);
	}else{
		write(logfd,"Socket creado\n",14);
	}

	/*DIRECCION*/
	direccion.sin_family = AF_INET;	//FAMILIA
	direccion.sin_port = htons(5000);	//PUERTO
	direccion.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/*CONFIGURACION DEL SOCKET*/
	if(setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, &val, sizeof (int)) == -1){
		perror("Opciones del socket");
		write(logfd,"ERROR: Opciones del socket\n",27);
		exit(EXIT_FAILURE);
	}else{
		write(logfd,"Socket configurado\n",19);
	}

	/*ENLACE*/
	if (bind(sockfd, (struct sockaddr *) &direccion, sizeof direccion) != 0){
		perror("Enlace");
		write(logfd,"ERROR: Enlace\n",14);
		exit(EXIT_FAILURE);
	}else{
		write(logfd,"Enlace creado\n",14);
	}

	/*SERVIDOR EN ESCUCHA*/
	if(listen(sockfd, 5) != 0){
		perror("Servidor en escucha");
		write(logfd,"ERROR: Servidor en escucha\n",28);
		exit(EXIT_FAILURE);
	}else{
		write(logfd,"Servidor puesto en escucha\n",27);
	}

	/*CONEXIONES*/
	signal(SIGCHLD,SIG_IGN);
	while ((conn = accept(sockfd, NULL, 0)) > 0){
		if(fork() == 0 ){
			write(logfd,"Conexion establecida\n",21);
			switch (service(conn)){
				case 0:
					write(logfd,"Servicio completo correctamente\n",32);
				break;
				case -1:
					write(logfd,"ERROR: 404 Not Found\n",21);
				break;
				case -2:
					write(logfd,"ERROR: 403 Forbidden\n",21);
				break;
				case -3:
					write(logfd,"ERROR: 505 Internal error\n",26);
				break;
			}
		}

	close(conn);
	}
	
return 0;
}
