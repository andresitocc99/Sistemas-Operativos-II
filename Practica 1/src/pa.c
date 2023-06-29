/*********************************************
*   Project: Práctica 1 de Sistemas Operativos II 
*
*   Program name: pa.c
*
*   Author: Andrés Castellanos Cantos
*
*   Date created: 04-03-2022
*
*   Porpuse: Creación de subdirectorios para cada estudiante.
|*********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <definitions.h>

void crearDirectorio(char directorio_raiz[]);
void generarDirectorios();

int main(int argc, char *argv[])
{

    struct stat st = {0};

    if (stat(ESTUDIANTES, &st) == -1)
    {
        crearDirectorio(ESTUDIANTES);
    }
    generarDirectorios();

    return 0;
}

void generarDirectorios()
{
    struct stat st = {0};
    FILE *fp;
    char *alumno;
    char linea[64];
    char nuevo_directorio[64];

    if ((fp = fopen("estudiantes_p1.txt", "r")) == NULL) {
        perror("No se ha podido leer el fichero Alumnos.\n");
        exit(EXIT_FAILURE);
    }
    while (fgets(linea, 64, (FILE *)fp)) {
        alumno = strtok(linea, " ");
        if (alumno != NULL) {
            sprintf(nuevo_directorio, "%s/%s", ESTUDIANTES, alumno);
            if (stat(nuevo_directorio, &st) == -1) {
                crearDirectorio(nuevo_directorio);
            }
        }
    }
    fclose(fp);
}

void crearDirectorio(char directorio[])
{
    if (mkdir(directorio, 0777) != 0)
    {
        fprintf(stderr, "Error creando el directorio %s.\n", directorio);
        exit(EXIT_FAILURE);
    }
}