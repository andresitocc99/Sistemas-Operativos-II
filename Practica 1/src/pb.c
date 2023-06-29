/*********************************************
*   Project: Práctica 1 de Sistemas Operativos II 
*
*   Program name: pb.c
*
*   Author: Andrés Castellanos Cantos
*
*   Date created: 08-03-2022
*
*   Porpuse: Copia del modelo de examen dentro del directorio correspondiente al estudiante
|*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <definitions.h>

#define A "MODELOA.pdf"
#define B "MODELOB.pdf"
#define C "MODELOC.pdf"

#define BUFFER 2048
#define MAX_PATH 1024


void copiar_archivo(char examen[], char alumno[]);
void leer_fichero();
void asignar_examen (char examen[], char alumno []);

int main (int argc, char *argv[]) {

  leer_fichero();
  return 0;
  
}

void leer_fichero () {

  FILE *fp;
  char *alumno;
  char *fichero;
  char linea[64];

   if ((fp = fopen(ARCHIVO, "r")) == NULL) {
        perror("No se ha podido leer el fichero Alumnos.\n");
        exit(EXIT_FAILURE);
    }
    while (fgets(linea, 64, (FILE *)fp))
    {
        alumno = strtok(linea, " ");
        fichero = strtok(NULL, " ");
        if (alumno != NULL && fichero != NULL)
        {
            asignar_examen(fichero, alumno);
        }
    }
    fclose(fp);
}

void asignar_examen(char examen[], char alumno[]) {
  if (strcmp (examen,"A") == 0) {
    copiar_archivo(A, alumno);
  } else if (strcmp (examen,"B") == 0) {
    copiar_archivo(B,alumno);
  } else if (strcmp (examen,"C") == 0) {
    copiar_archivo(C,alumno);
  }
}


void copiar_archivo (char examen[], char alumno[]) {

    int nBytes;
    char buffer_escritura[BUFFER];

    FILE *archivo_fuente;
    FILE *archivo_objetivo;

    char ruta_examen [MAX_PATH];
    char examen_asignado [MAX_PATH];

    sprintf(ruta_examen, "%s/%s", EXAMENES, examen);
    sprintf(examen_asignado, "%s/%s/%s", ESTUDIANTES, alumno, examen);

    /* Comprobamos si el archivo fuente (el examen escogido) se puede abrir correctamente */

    if ((archivo_fuente = fopen(ruta_examen, "r")) == NULL) {
      fprintf(stderr, "Error al copiar el modelo de examen.\n");
      exit(EXIT_FAILURE);
    }

    /* Comprobamos si el archivo destino se puede escribir en él */
    
    if ((archivo_objetivo = fopen(examen_asignado,"wb")) == NULL) {
      fprintf(stderr,"Error asignando el modelo de examen\n");
      exit(EXIT_FAILURE);
    }

    while(!feof(archivo_fuente)) {
      nBytes = fread(buffer_escritura,1,sizeof(buffer_escritura),archivo_fuente);
      fwrite(buffer_escritura,1,nBytes,archivo_objetivo);
    }
  
    fclose(archivo_fuente);
    fclose (archivo_objetivo);
}