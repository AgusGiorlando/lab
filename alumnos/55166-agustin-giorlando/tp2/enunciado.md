# COMPUTACION II


# TP2

Fecha de entrega: 23/05/17


## Requerimientos:

Crear una aplicación que procese un archivo por entrada estándar. La aplicación deberá 
lanzar dos procesos. El primero deberá contar la cantidad de palabras mientras que el 
segundo convertirá una serie de palabras específicas a mayúsculas. 

El padre será quien muestre la cantidad de palabras por pantalla, mientras que la salida
del segundo proceso deberá redireccionarse a un archivo nuevo.
 

## Modo de uso

$ cat noticia | ./tp2 -o newfile
$ ./tp2 -i noticia -o newfie


## Objetivos

* Utilizar redirección de entrada y salida.
* Utilizar getopt para manejo de argumentos.
* Utilizar un archivo Makefile para el proceso de compilación y testeo.
* Manejo y sincronización de procesos.


### Tips

* funciones para el manejo de cadenas de caracteres en la biblioteca string.h
* read()
* write()
* pipe()
* fork()
* snprintf()
