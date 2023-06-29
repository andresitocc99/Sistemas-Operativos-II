/*********************************************
*   Project: Práctica 1 de Sistemas Operativos II 
*
*   Program name: pd.c
*
*   Author: Andrés Castellanos Cantos
*
*   Date created: 14-06-2022
*
*   Porpuse: Eliminación de subdirectorios de los estudiantes.
|*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <definitions.h>

int main(int argc, char *argv[])
{
    char command[100];
    sprintf(command, "rm -rf %s", ESTUDIANTES);
    if (system(command) == -1)
    {
        fprintf(stderr, "[PROCESO D] No se pudo eliminar el directorio.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
