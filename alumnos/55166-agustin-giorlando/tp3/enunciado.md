# COMPUTACION II


# TP3

Fecha de entrega: 13/06/17


## Requerimientos:

Crear una aplicación que procese un archivo por entrada estándar. La aplicación deberá 
lanzar tres procesos. El proceso padre cargará en memoria compartida el contenido de
un archivo, el cual será analizado luego por los tres procesos. Dos procesos deberán
contar la cantidad de palabras, mientras que el restante deberá contar la cantidad de
palabras que aparecen de un conjunto dado de palabras. El proceso padre notificará
con una señal a los hijos, luego de haber cargado el archivo en memoria compartida.

Lista de palabras: Hypertext, protocol, HTTP, MIME, gateway, URL y URI. 

## Modo de uso

$ ./tp3 -i archivo


## Objetivos

* Utilizar getopt para manejo de argumentos.
* Utilizar un archivo Makefile para el proceso de compilación y testeo.
* Manejo y sincronización de procesos.
* Uso de señales.


### Tips

* funciones para el manejo de cadenas de caracteres en la biblioteca string.h
* read()
* write()
* fork()
* snprintf()
