#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <semaphore.h>

int buscar(const char *palabra,const char *frase){
	const int fr_size = strlen(frase),pal_size = strlen(palabra);
	int cantidad = 0, letra = 0;
  	for (int i=0;i<fr_size;i++){
  		if (frase[i] == palabra[0]){
			for (int j=0;j<pal_size;j++){
				if (frase[i+j] == palabra[j]){
					letra++;
				}
			}

			if (letra == pal_size){
				cantidad++;
			}

			letra = 0;
		}
	}
return cantidad;
}

int contar(const char *frase){
	int cantidad = 0;
	for (int i=0;i<=80;i++){
		if (frase[i] == ' ' || frase[i] == '.'){
			cantidad++;
		}
	}
return cantidad;
}

int main(int argc, char **argv){
	int in_desc = 0,log_desc = 0;
	int oc,pid1,pid2,pid3,leido;
	int pal1 = 0,pal2 = 0,*mostrar,Hypertext = 0, protocol = 0,HTTP = 0,MIME = 0,gateway = 0,URL = 0,URI = 0;
	char *input,*buff,buffer[80];
	buff = buffer;
	sem_t *padre,*hijo1,*hijo2,*hijo3;

	/*INICIALIZACION DE SEMAFOROS*/
	hijo1 = mmap(NULL,(sizeof(sem_t)+sizeof(int)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
	hijo2 = mmap(NULL,(sizeof(sem_t)+sizeof(int)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
	hijo3 = mmap(NULL,(sizeof(sem_t)+sizeof(int)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
	padre = mmap(NULL,(sizeof(sem_t)+sizeof(int)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
	
	sem_init(hijo1,1,0);
	sem_init(hijo2,1,0);
	sem_init(hijo3,1,0);
	sem_init(padre,1,1);

	/*ARCHIVO DE LOG*/
	if((log_desc = open("./log.txt",O_WRONLY | O_CREAT | O_TRUNC,0664)) < 0){
		perror("Creacion de log");
	}else {
		write(log_desc,"Archivo de log creado\n",22);
	}	

	/*MANEJO DE OPCIONES*/
	while ((oc = getopt(argc, argv, ":i:")) != -1){
		switch(oc){
			case 'i':
			/*INPUT*/
				input = optarg;

				if((in_desc = open(input,O_RDONLY,0664)) < 0){
					perror("Abrir archivo input");
				}else {
					write(log_desc,"Archivo input abierto\n",22);
				}	
				break;

			case ':':
			/*OPCION SIN ARGUMENTOS*/
				fprintf(stderr,"%s: La opcion '-%c' requiere argumentos\n",argv[0],optopt);
				write(log_desc,"Proceso terminado: Opcion sin argumentos\n",39);
				return -1;
				break;

			case '?':
			default:
			/*OPCION NO VALIDA*/
				fprintf(stderr,"%s: La opcion '-%c' es invalida\n",argv[0],optopt);
				write(log_desc,"Proceso terminado: Opcion no valida\n",36);
				return -1;
				break;
		}
	}
	
	/*MEMORIA COMPARTIDA*/
	if ((buff = mmap(NULL,80,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,in_desc,0)) == (void *) -1){
		write(log_desc,"Imposible mapear input\n",24);
		perror("Mapear input");
		return -1;
	} else {
		write(log_desc,"Archivo input mapeado\n",22);
	}	

	if((mostrar = mmap(NULL,(sizeof(int)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0)) == (void *) -1){
		write(log_desc,"Imposible mapear mostrar\n",25);
		perror("Mapear mostrar");
		return -1;
	} else {
		write(log_desc,"Variable mostrar mapeada\n",25);
		*mostrar = 0;
	}

	/*HIJOS*/
	if ((pid1 = fork()) > 0){
		if ((pid2 = fork()) > 0){
			pid3 = fork();
		}
	}

	/*HIJO 1*/
	if (pid1 == 0){
		while(*mostrar == 0){
			sem_wait(hijo1);
			pal1 += contar(buff);
			sem_post(hijo2);
		}
		printf("Cantidad de palabras: %i\n",pal1);
		sem_post(hijo2);
	return 0;

	}else if (pid1 < 0){
		perror("No se pudo crear hijo 1");
		write(log_desc,"No se pudo crear hijo 1\n",25);
	return -1;
	}
	
	/*HIJO 2*/
	if (pid2 == 0){
		while(*mostrar == 0){
			sem_wait(hijo2);
			pal2 += contar(buff);
			sem_post(hijo3);
		}
		printf("Cantidad de palabras: %i\n",pal2);
		sem_post(hijo3);
	return 0;


	}else if (pid2 < 0){
		perror("No se pudo crear hijo 2");
		write(log_desc,"No se pudo crear hijo 2\n",25);
	return -1;
	}

	/*HIJO 3*/
	if (pid3 == 0){
		while(*mostrar == 0){
			sem_wait(hijo3);
			Hypertext += buscar("Hypertext",buff);
			protocol += buscar("protocol",buff);
			HTTP += buscar("HTTP",buff);
			MIME += buscar("MIME",buff);
			gateway += buscar("gateway",buff);
			URL += buscar("URL",buff);
			URI += buscar("URI",buff);
			sem_post(padre);
		}
		printf("Hypertext: %i\n",Hypertext);
		printf("protocol: %i\n",protocol);
		printf("HTTP: %i\n",HTTP);
		printf("MIME: %i\n",MIME);
		printf("gateway: %i\n",gateway);
		printf("URL: %i\n",URL);
		printf("URI: %i\n",URI);
		sem_post(padre);
	return 0;

	}else if (pid3 < 0){
		perror("No se pudo crear hijo 3");
		write(log_desc,"No se pudo crear hijo 3\n",25);
	return -1;
	}

	/*PADRE*/
	if (pid1 > 0 && pid2 > 0 && pid3 > 0){
		while ((leido = read(in_desc,buff,80)) > 0){		
			sem_wait(padre);
			sem_post(hijo1);
		}
	
		sem_wait(padre);
		*mostrar = 1;
		sem_post(hijo1);
	return 0;
	}
	 
    return 0;
}
