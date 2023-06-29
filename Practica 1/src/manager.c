/*********************************************
*   Project: Práctica 1 de Sistemas Operativos II 
*
*   Program name: manager.c
*
*   Author: Andrés Castellanos Cantos
*
*   Date created: 01-03-2022
*
*   Porpuse: Gestión de procesos hijos de manera paralela
|*********************************************/

#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <definitions.h>

#define PROCESO_FINALIZADO -1

pid_t GB_pids[4];

void installer_signal();
void finalizar_procesos(int signal);
void ejecutar_proceso (char *proceso, int tuberia[]);
void wait_proceso();
void matar_proceso(pid_t pid);

int main() {
    char respuesta_c[100];
    int tuberia[2];

    installer_signal();
    /* Arrancamos el proceso A y esperamos que finalice*/
    ejecutar_proceso(PA_CLASS,tuberia);
    wait_proceso();

    /* Arrancamos el proceso A y B de manera simultánea y esperamos que ambos finalicen*/
    ejecutar_proceso (PB_CLASS, tuberia);
    ejecutar_proceso (PC_CLASS, tuberia);
    wait_proceso();
    wait_proceso();

    read(tuberia[0],respuesta_c,100);
    escribir_log(respuesta_c);
    escribir_log("FIN");
    return 0;

}

void ejecutar_proceso (char *proceso, int tuberia[]) {
  char *path;
  int pid;
  pipe (tuberia); /* Inicializamos la tuberia */

  if (strcmp(proceso,PA_CLASS)==0) {

      path = PA_PATH;
      pid = fork();
      GB_pids[0] = pid;

  } else if (strcmp(proceso,PB_CLASS)==0) {

      path = PB_PATH;
      pid = fork();
      GB_pids[1] = pid;

  } else if (strcmp(proceso,PC_CLASS)==0) {

      dup2(tuberia[1],STDOUT_FILENO);
      path = PC_PATH;
      pid = fork();
      GB_pids[2] = pid;

  } else if (strcmp(proceso,PD_CLASS)==0) {

      path = PD_PATH;
      pid = fork();
      GB_pids[3] = pid;

  } else {
      fprintf(stderr,"ERROR // PROCESO NO ENCONTRADO\n");
  }

  switch (pid) {
      case -1:
        fprintf(stderr, "[Manager] ERROR CREANDO EL PROCESO");
        exit(EXIT_FAILURE);
      break;

      case 0:
        execl(path,proceso,NULL,NULL);
        fprintf(stderr, "[Manager] ERROR EN LA EJECUCIÓN DEL PROCESO");
      break;

      default:
        break;
  }
}

void wait_proceso () {
    int pid = wait(NULL);
    if (pid == GB_pids[0])
    {
        escribir_log("Creación de directorios finalizada.");
        GB_pids[0] = -1;
    }
    else if (pid == GB_pids[1])
    {
        escribir_log("Copia de modelos de examen, finalizada.");
        GB_pids[1] = -1;
    }
    else if (pid == GB_pids[2])
    {
        escribir_log("Creación de archivos con nota necesaria para alcanzar la nota de corte, finalizada.");
        GB_pids[2] = -1;
    }
    else if (pid == GB_pids[3])
    {
        escribir_log("Proceso interrumpido eliminando directorio");
    }
    else
    {
        escribir_log("Proceso no esperado");
    }

}

void installer_signal () {
    if (signal(SIGINT, finalizar_procesos) == SIG_ERR)
    {
        fprintf(stderr, "[MANAGER] Error instalando la señal");
        exit(EXIT_FAILURE);
    }
}

void finalizar_procesos (int signal) {
    matar_proceso(GB_pids[0]);
    matar_proceso(GB_pids[1]);
    matar_proceso(GB_pids[2]);

    GB_pids[3]=fork();

    switch (GB_pids[3]) {
        case -1:
            fprintf(stderr, "[MANAGER] Error creando el proceso A\n");
            exit(EXIT_FAILURE);
        break;
        case 0:
            execl(PD_PATH, PD_CLASS, NULL, NULL);
            fprintf(stderr, "[MANAGER] Return no esperado. Ha fallado al ejecución del proceso.\n");
        break;

        default:
            wait_proceso();
        break;
    }
}

void matar_proceso(pid_t pid)
{
    if (pid != PROCESO_FINALIZADO)
    {
        if (kill(pid, SIGKILL) == -1)
        {
            fprintf(stderr, "No se puedo matar al proceso %d.\n", pid);
        }
    }
}