#ifndef DEFINITIONS
#define DEFINITIONS


#define LIBROS "./libros"
#define LOG_PATH "./log"

/*Valores estáticos*/
#define NCLIENTS 10
#define NTHREADS 2
#define BUFFER 4

#include "../src/Request.cpp"
#include "../src/Client.cpp"
#include <queue>

int nSimultaneous_premium_searches = 4;
int nSimultaneous_nopremium_searches = 1;


/*Flag para comprobar el id del cliente con el devuelto en la operacion de pago*/

int credits=100;
bool end_program=false;


/*********************MUTEXES*******************/
std::mutex mutex_queue;
std::mutex print_screen_mutex;

/*Cola de peticiones de recarga*/

std::vector<Client> vClients;

std::vector<std::string> WORDS =
{
    "lineas",
    "la",
    "palabra",
    "ley",
    "alta",
    "pregunta"
};

#endif