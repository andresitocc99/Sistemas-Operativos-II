/*********************************************
*   Project: Práctica 1 de Sistemas Operativos II 
*
*   Program name: pc.c
*
*   Author: Andrés Castellanos Cantos
*
*   Date created: 11-03-2022
*
*   Porpuse: Calculo de nota mínima del próximo examen, siendo guardado en un TXT
|*********************************************/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <definitions.h>

int GB_nEstudiantes;
float GB_notaTotal;


void leer_fichero();
void calcular_nota(float nota, char alumno[]);
void escribir_nota (char msg[], char alumno[]);
void calcular_nota_media();

int main (int argc, char *argv[]) {

  GB_nEstudiantes = 0; /* Nº de alumbos */
  GB_notaTotal = 0; /* Guardamos la suma de todas las notas acumuladas */
  leer_fichero(); 
  calcular_nota_media(); /* Se calcula la media usando las notas totales acumuladas y el numero de estudiantes*/
  return 0;
  
}

void leer_fichero () {

  FILE *fp;
  char *alumno;
  char *nota;
  char linea[64];

   if ((fp = fopen(ARCHIVO, "r")) == NULL)
    {
        perror("No se ha podido leer el fichero Alumnos.\n");
        exit(EXIT_FAILURE);
    }
    while (fgets(linea, 64, (FILE *)fp))
    {
        alumno = strtok(linea, " ");
        strtok(NULL, " ");
        nota = strtok(NULL, " ");
        if (alumno != NULL && nota != NULL)
        {
            GB_nEstudiantes +=1;
            calcular_nota (atof(nota),alumno);
        }
    }
    fclose(fp);
}

void calcular_nota (float nota, char alumno[]) {
  float nota_minima = 2* (5-(nota/2));
  GB_notaTotal += nota;
  char msg_nota [124];
  sprintf(msg_nota, "La nota que debes obtener en este nuevo examen para superar la prueba es: %f\n",nota_minima);
  escribir_nota (msg_nota, alumno);
}

void escribir_nota (char msg [], char alumno[]) {
    
    FILE *nota;
    char nota_alumno [64];
    sprintf(nota_alumno, "%s%s%s",ESTUDIANTES, alumno, "nota.txt");
    if ((nota = fopen(nota_alumno,"wb"))==NULL) {
      printf("EError al comunicar la nota\n");
      exit(EXIT_FAILURE);
    }
    fputs(msg,nota);
    fclose(nota);

}

void calcular_nota_media () {
  float nota_media = GB_notaTotal / GB_nEstudiantes;
  printf("La nota media de la clase es: %f", nota_media);
}