/*********************************************
*   Project: Práctica 1 de Sistemas Operativos II 
*
*   Program name: demonio.c
*
*   Author: Andrés Castellanos Cantos
*
*   Date created: 21-06-2022
*
*   Porpuse: Copia de seguridad de los estudiantes en cada minuto
*
*   Revision History: 
|*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <definitions.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define TBUFFER 2048
#define MAX_PATH 2048
#define BACKUP "./backup"


void crearDirectorio(char directorio[]);
void recorrerDirectorio(const char nDirectorio[], const char nDirectorioCopia[]);
void copiar(FILE *original, char nombre_archivo[], const char destino[]);

int main(int argc, char *argv[])
{

    struct stat st = {0};
    if (stat(BACKUP, &st) == -1)
    {
        if (mkdir(BACKUP, 0700) != 0)
        {
            fprintf(stderr, "Error creando el directorio.\n");
            exit(EXIT_FAILURE);
        }
    }

    while (1)
    {
        if (stat(ESTUDIANTES, &st) == 0)
        {
            recorrerDirectorio(ESTUDIANTES, BACKUP);
        }
        sleep(60);
    }

    return 0;
}

void recorrerDirectorio(const char nDirectorio[], const char nDirectorioCopia[])
{
    DIR *directorio;
    char nodo[MAX_PATH + 1], CopiaNodo[MAX_PATH + 1];
    struct dirent *objeto;
    struct stat st;
    FILE *archivo;
    if ((directorio = opendir(nDirectorio)) == NULL)
    {
        fprintf(stderr, "[DEMONIO] Error abriendo el directorio.\n");
        return;
    }
    while ((objeto = readdir(directorio)) != NULL)
    {
        if (strcmp(objeto->d_name, ".") == 0 || strcmp(objeto->d_name, "..") == 0)
            continue;

        sprintf(nodo, "%s/%s", nDirectorio, objeto->d_name);

        if (stat(nodo, &st) == -1)
        {
            fprintf(stderr, "[DEMONIO] Error con %s.\n", nodo);
        }
        if (S_ISREG(st.st_mode))
        {
            if ((archivo = fopen(nodo, "r")) == NULL)
            {
                fprintf(stderr, "[DEMONIO] No se ha podido leer el fichero origen.\n");
                exit(EXIT_FAILURE);
            }
            copiar(archivo, objeto->d_name, nDirectorioCopia);
        }
        else if (S_ISDIR(st.st_mode))
        {
            sprintf(CopiaNodo, "%s/%s", nDirectorioCopia, objeto->d_name);
            crearDirectorio(CopiaNodo);
            recorrerDirectorio(nodo, CopiaNodo);
        }
    }
}
void crearDirectorio(char directorio[])
{
    struct stat st = {0};
    if (stat(directorio, &st) == -1)
    {
        if (mkdir(directorio, 0700) != 0)
        {
            fprintf(stderr, "[DEMONIO] Error creando el directorio %s.\n", directorio);
            exit(EXIT_FAILURE);
        }
    }
}

void copiar(FILE *original, char nombre_archivo[], const char destino[])
{
    FILE *archivo;
    int nbytes;
    char buffEscritura[TBUFFER];
    char archivoCopiado[MAX_PATH + 1];
    sprintf(archivoCopiado, "%s/%s", destino, nombre_archivo);
    if ((archivo = fopen(archivoCopiado, "wb")) == NULL)
    {
        fprintf(stderr, "[DEMONIO] No se ha podido copiar el fichero destino!\n");
        exit(EXIT_FAILURE);
    }
    while (!feof(original))
    {
        nbytes = fread(buffEscritura, 1, sizeof(buffEscritura), original);
        fwrite(buffEscritura, 1, nbytes, archivo);
    }
    fclose(original);
    fclose(archivo);
}