

#define EXAMENES "./modelos_examen"
#define ESTUDIANTES "./Estudiantes"
#define ARCHIVO "./estudiantes_p1.txt"
#define PA_CLASS "PA"
#define PA_PATH "./exec/pa"
#define PB_CLASS "PB"
#define PB_PATH "./exec/pb"
#define PC_CLASS "PC"
#define PC_PATH "./exec/pb"
#define PD_CLASS "PD"
#define PD_PATH "./exec/pc"


void escribir_log(char * mensaje){
  FILE *stlog;
  if((stlog = fopen("./log.txt", "at")) != NULL){
    fprintf(stlog,"%s \n", mensaje);
  }
  fclose(stlog);
}