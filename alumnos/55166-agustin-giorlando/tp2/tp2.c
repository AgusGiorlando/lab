#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char **argv)
{
	char buffer[80];
	int in_desc = 0,log_desc = 0,out_desc = 0;
	int oc,pid1,pid2,ph1[2],ph2[2],h1p[2],leido,lectura_p;
	int i = 0,int_pal = 0;
	char *input,*output,buff_ph1[80],buff_ph2[80],buff_h1p[80];
	char char_pal[5];

	if((log_desc = open("./log.txt",O_WRONLY | O_CREAT | O_TRUNC,0664)) < 0){
		perror("Creacion de log");
	}else {
		write(log_desc,"Archivo de log creado\n",22);
	}	

	memset(buffer,'\0',80);
	memset(buff_ph1,'\0',80);
	memset(buff_ph2,'\0',80);
	memset(buff_h1p,'\0',80);
	memset(char_pal,'\0',5);

	while ((oc = getopt(argc, argv, ":i:o:")) != -1){
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

			case 'o':
			/*OUTPUT*/
				output = optarg;

				if((out_desc = open(output,O_WRONLY | O_CREAT | O_TRUNC,0664)) < 0){
					perror("Creacion de archivo output");
				}else {
					write(log_desc,"Archivo output creado\n",22);
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

	/*HIJO 1*/
	pipe(ph1);
	pipe(h1p);
	pid1 = fork();

	if (pid1 == 0){
		close(ph1[1]);
		close(h1p[0]);

		while ((leido = read(ph1[0],buff_ph1,sizeof buff_ph1)) >0){
			
			if (leido == -1){
				perror("Proceso hijo 1 no puede leer");
				write(log_desc,"Proceso hijo 1 no puede leer\n",29);
				return -1;
			} else {
				for (i = 0;i <= sizeof buff_ph1;i++){
					if (buff_ph1[i] == ' ' || buff_ph1[i] == '.'){
						int_pal ++;
					}
				}
			}

		}
		
		sprintf(char_pal,"%d\n",int_pal);
		write(h1p[1],char_pal,5);
		write(log_desc,"Hijo 1 envia palabras\n",22);

		close(ph1[0]);
		close(h1p[1]);
		return 0;

	}else if (pid1 < 0){
		perror("No se pudo crear hijo 1");
		write(log_desc,"No se pudo crear hijo 1\n",25);
		return -1;
	}
	
	/*HIJO 2*/
	pipe(ph2);
	pid2 = fork();

	if (pid2 == 0){
		close(ph2[1]);

		while ((leido = read(ph2[0],buff_ph2,sizeof buff_ph2)) >0){
			
			if (leido == -1){
				perror("Proceso hijo 2 no puede leer");
				write(log_desc,"Proceso hijo 2 no puede leer\n",29);
			} else {	
				for (i=0;i<=sizeof buff_ph2;i++){
					if (buff_ph2[i] >= 'A' && buff_ph2[i] <= 'Z' && buff_ph2[i+1] != ' '){
							buff_ph2[i+1] = toupper(buff_ph2[i+1]);
					}
				}

				if ((write(out_desc,buff_ph2,leido)) < 0){
					perror("Hijo 2 no puede escribir");
					write(log_desc, "Hijo 2 no puede escribir\n",25);
					return -1;
				} else {
					write(log_desc,"Hijo 2 escribe en output\n",25);
				}

			}
		}

		close(ph2[0]);
		return 0;

	}else if (pid2 < 0){
		perror("No se pudo crear hijo 2");
		write(log_desc,"No se pudo crear hijo 2\n",25);
		return -1;
	}

	/*PADRE*/
	if (pid1 > 0 && pid2 > 0){
		close(ph1[0]);
		close(ph2[0]);
		close(h1p[1]);


		while((lectura_p = read(in_desc,buffer,sizeof buffer)) > 0){
			
			if(lectura_p == -1){
				perror("Proceso padre no puede leer");
				write(log_desc,"Proceso padre no puede leer\n",29);
				return -1;
			} else {

				if ((write(ph1[1],buffer,sizeof buffer)) < 0){
					perror("Imposible escribir en hijo 1");
					write(log_desc,"Imposible escribir en hijo 1\n",29);
					return -1;
				} else {
					write(log_desc,"Padre escribe en hijo 1\n",24);
				}

				if ((write(ph2[1],buffer,sizeof buffer)) < 0){
					perror("Imposible escribir en hijo 2");
					write(log_desc,"Imposible escribir en hijo 2\n",29);
					return -1;
				} else {
					write(log_desc,"Padre escribe en hijo 2\n",24);
				}
			}
		

		}

		close(ph1[1]);
		close(ph2[1]);
		while ((leido = read(h1p[0],buff_h1p,sizeof buff_h1p)) > 0){
			
			if (leido == -1){
				perror("Proceso padre no recibe de hijo 1");
				write(log_desc,"Padre no recibe de hijo 1\n",26);
				return -1;
			}

			write(1,buff_h1p,leido);

		close(h1p[0]);
		}
	} 
    return 0;
}
