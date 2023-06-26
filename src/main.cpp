/*********************************************
*   Project: Práctica 3 de Sistemas Operativos II 
*
*   Program name: ssooiigle.c
*
*   Author: Andres Castellanos Cantos
*
*   Date created:10-05-2022
*
*   Porpuse: 
|*********************************************/

#include <iostream>
#include <fstream>
#include <cctype>
#include <regex>
#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>
#include <functional>
#include <definitions.h>
#include <colours.h>
#include <cstdlib>
#include <dirent.h>
#include <condition_variable>
#include <future>
#include <chrono>
#include <signal.h>
#include <exception>
#include "wordSearched.cpp"
#include "Client.cpp"
#include "PaySystem.cpp"

/*Manejador de señal*/
void signal_handler(int signal);
void install_handler();

/*Funciones definidas*/
void create_clients(); // Method that create clients


int number_of_lines(std::string file);
void create_threads(std::string file, Client& c, std::mutex& access_balance,std::mutex& access_log);
void find_word(int thread,std::vector<std::string> assignedLines, int begin, int end, Client& c, std::mutex& access_balance, std::mutex& access_result,std::map<int,std::vector<WordSearched>>& vWords);
void print_result(std::map<int,std::vector<WordSearched>> vWords, Client c,std::mutex& access_log);
void list_dir();
void create_log(int i);
void generateClient(Client c);
void free_resources();
void divide_and_lookfor (std::vector<std::thread> &vThreads_client, int &begin, int &end, int &number_lines, int size_threads, std::map<int, std::vector<std::string>> assignedLines, Client& c, std::mutex& access_balance, std::mutex& access_result, std::map<int, std::vector<WordSearched>>& vWords);

std::map<int, std::vector<std::string>> shareLines(std::string file, int nLines, int sizeOfThreads);
std::vector<std::string> splitLine(std::string line);
std::string analize_word(std::string word);

int payment_id=-1;
/*********MUTEXES**************/
std::mutex search_mutex;

/*******CONDITION VARIABLE*****/
std::condition_variable cv_premium;
std::condition_variable cv_nopremium;
std::condition_variable cv_paysystem;

std::vector<std::string> vLibros;

/*Vector para almacenar hilos buscadores*/
std::vector<std::thread> vtSearches;

PaySystem paysystem (std::ref(cv_paysystem), std::ref(payment_id), std::ref(end_program));

/***************************************************************/
/***************************MAIN********************************/
/***************************************************************/

/* El main se encargara de la creación de hilos y su finalización*/
int main(int argc, char *argv[]){

    install_handler();
    list_dir();
    std::thread pay(&PaySystem::constant_check, &paysystem);
    srand(time(NULL));
    create_clients();       // METHOD THAT CREATE CLIENTS
    free_resources();
    pay.join();
    return 0;
}

/***************************************************************/
/*************************METHODS*******************************/
/***************************************************************/

void install_handler(){
    if(signal(SIGINT, signal_handler)== SIG_ERR){
        std::cerr<<RED<<"Error instaling the signal handler"<<RESET<<std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void free_resources(){
    end_program=true;
    vLibros.clear();
    vClients.clear();
}

void signal_handler(int signal){
    std::cout<<std::endl<<YELLOW<<"[SIGNAL HANDLER] Programa finalizado por el usuario..."<<RESET<<std::endl;
    free_resources();
    std::exit(EXIT_FAILURE);
}

/* Method that generate and print all clients, depending on NCLIENTS*/
void create_clients () {
    for (int i=0; i<NCLIENTS; i++) {
        create_log(i);
        int client_type = (rand() & 3)+1;
        if (client_type == 1){
            vClients.push_back(Client(i, false, 50,false, WORDS[(rand()%WORDS.size())]));
        } else if (client_type == 2) {
            int limit = (rand() % 101) + 50;
            vClients.push_back(Client(i, true, limit,false, WORDS[(rand()%WORDS.size())]));
        } else {
            vClients.push_back(Client(i, true, -1,true, WORDS[(rand()%WORDS.size())]));
        }
        vtSearches.push_back(std::thread(generateClient,vClients[i]));
        print_screen_mutex.lock();
        vClients[i].toString();
        print_screen_mutex.unlock();

    }
    std::for_each(vtSearches.begin(), vtSearches.end(), std::mem_fn(&std::thread::join));   
}

/* Crear un archivo para que el cliente escriba la información relativa a este*/ 
void create_log(int i){
    std::string name_fichero = "./log/Client_"+std::to_string(i)+".txt";
    try{
        std::ofstream(name_fichero, std::ios::out);
    }catch(const std::exception& e){
        std::cout<<e.what()<<std::endl;
    }
}
/* Almacenamos en un vector los libros disponibles para buscar en ellos*/ 
void list_dir(){
    std::string dirPath="./libros";
    DIR * directorio;
    struct dirent *elemento;
    std::string elem;
    if((directorio = opendir(dirPath.c_str()))){
        while((elemento = readdir(directorio))){
            elem = elemento->d_name;
            if(elem != "." && elem!=".."){
                vLibros.push_back(elem);
            }  
        }
    }
    closedir(directorio);    
}
/* En base al cliente enviado por parametros se inicia las busquedas en cada uno de los libros (un hilo por libro)*/ 


void generateClient(Client c){
    std::vector<std::thread> vSearch;
    std::mutex access_balance;
    std::mutex access_log;
    std::unique_lock<std::mutex> unique_lock (search_mutex); 

    print_screen_mutex.lock();
    std::cout << ORANGE << "[Cliente_" << c.getId() << "] Esperando buscador disponible..." << RESET << std::endl;
    print_screen_mutex.unlock();

    unsigned t0, t1;
    if (c.isPremium()) {
        cv_premium.wait(unique_lock, [] {
            return nSimultaneous_premium_searches > 0;
        });
        nSimultaneous_premium_searches--;
    } else {
        cv_nopremium.wait(unique_lock, [] {
            return nSimultaneous_nopremium_searches > 0;
        });
        nSimultaneous_nopremium_searches--;
    }
    unique_lock.unlock();

    print_screen_mutex.lock();
    std::cout << GREEN << "[cliente_" << c.getId() << "] Inicia busqueda" << RESET << std::endl;
    print_screen_mutex.unlock();


    t0=std::clock();
    for (std::size_t i = 0; i < vLibros.size(); i++){
        vSearch.push_back(std::thread(create_threads, vLibros[i], std::ref(c), std::ref(access_balance), std::ref(access_log)));
    }
    
    std::for_each(vSearch.begin(), vSearch.end(), std::mem_fn(&std::thread::join));

    unique_lock.lock();
    if (c.isPremium()) {
        std::cout << MAGENTA << "[Cliente_" << c.getId() << "] Finalizada la busqueda con exito." << RESET << std::endl;
        nSimultaneous_premium_searches++;
        cv_premium.notify_one();
    } else {
        std::cout << MAGENTA << "[Cliente_" << c.getId() << "] Finalizada la busqueda con exito." << RESET << std::endl;
        nSimultaneous_nopremium_searches++;
        cv_nopremium.notify_one();
    }
    unique_lock.unlock();


    t1=std::clock();
    
    c.setExecutionTime(double(t1-t0)/CLOCKS_PER_SEC); //Calculamos el tiempo total que ha tardado en buscar en todos los libros
}


/*Lanzamos en base al numero de lineas, distintos hilos que se encargaran de buscar en los libros en zonas distintas*/
void create_threads(std::string file, Client& c, std::mutex& access_balance, std::mutex& access_log){
    int nLines = number_of_lines(file);;
    int sizeForThreads = nLines/NTHREADS;
    std::mutex access_result;
    std::map<int, std::vector<std::string>> assignedLines;
    std::map<int,std::vector<WordSearched>> vWords;
    int begin, end;
    std::vector<std::thread> vThreads_client;
    
    if(nLines<NTHREADS){
        std::cerr << ERROR("[ERROR]-- ")<<WARNING(UNDERLINE("More threads than lines") )  <<std::endl;
        exit(EXIT_FAILURE);
    }
    
    assignedLines=shareLines(file,nLines,sizeForThreads);

    divide_and_lookfor(vThreads_client, begin, end, nLines, sizeForThreads, assignedLines, std::ref(c), std::ref(access_balance), std::ref(access_result), std::ref(vWords));
    print_result(vWords, c,std::ref(access_log));
}

/* Devuelve el número de lineas de un archivo.*/
int number_of_lines(std::string file){
    int numLines = 0;
    std::string dir = "./libros/"+file;
    std::ifstream File(dir); 

    while (File.good()) 
    { 
        std::string line; 
        std::getline(File, line); 
        ++numLines; 
    } 
    return numLines;
}

/* Asignara a cada hilo el número correspondiente de linea a leer*/
std::map<int,std::vector<std::string>> shareLines(std::string file, int nLines, int sizeOfThreads){
    std::ifstream File("./libros/"+file);
    std::string line;
    std::map<int, std::vector<std::string>> result;
    for(int i = 0; i < NTHREADS; i++){
        for (int j = 0; j < sizeOfThreads; j++){
            std::getline(File, line);
            result[i].push_back(line);
        }
        if(i==NTHREADS-1 && nLines % NTHREADS!= 0){
                for (int k = sizeOfThreads * NTHREADS; k < nLines; k++) {
                    std::getline(File, line);
                    result[i].push_back(line);
                }
                
            }
    }
    return result;   
}

void divide_and_lookfor (std::vector<std::thread> &vThreads_client, int &begin, int &end, int &number_lines, int size_threads, std::map<int, std::vector<std::string>> assignedLines, Client& c, std::mutex& access_balance, std::mutex& access_result, std::map<int,std::vector<WordSearched>>& vWords) {
    
    std::vector<std::thread> vThreads;
    for (int i = 0; i < NTHREADS; i++) {
        begin=i*size_threads+1;
        end=begin+size_threads-1;

        if(number_lines%NTHREADS!= 0 && i==NTHREADS-1){ //Aquí se realiza un ajuste para el ultimo hilo en el caso que no sea exacta la división de total de lineas entre el número de hilos.
            end = number_lines;
        }
        vThreads.push_back(std::thread(find_word, i, assignedLines[i], begin, end, std::ref(c), std::ref(access_balance), std::ref(access_result), std::ref(vWords)));
    }
    std::for_each(vThreads.begin(), vThreads.end(), std::mem_fn(&std::thread::join));
}

/* Es la función que ejecutaran los hilos y buscaran la palabra objetivo en el trozo de lineas asignado*/
void find_word(int thread,std::vector<std::string> assignedLines, int begin, int end, Client& c,std::mutex& access_balance, std::mutex& access_result, std::map<int,std::vector<WordSearched>>& vWords){
    
    std::string word;
    std::string solution[3]; //vector de 3 posiciones para guardar la palabra anterior (0), la palabra encontrada (1) y la palabra siguiente (2).
    std::vector<std::string> line;
    
    for(std::size_t nLine = 0; nLine < assignedLines.size(); nLine++){
        line = splitLine(assignedLines[nLine]);
        solution[0]=""; //En caso que se salte a una nueva linea se reseteara el valor de la palabra anterior
        for(std::size_t position = 0; position< line.size(); position++){
            if(!analize_word(c.getObjective()).compare(analize_word(line[position]))){
                access_balance.lock();
                if (c.getBalance()!=-1){    //Premium con saldo ilimitado
                    if(c.getBalance()==0 && !c.isPremium()){  // Cuenta gratuita con 0 saldo, termina bucle
                        break;

                    } else if(c.getBalance()==0 && c.isPremium()){
                        std::cout<<YELLOW<<"[Cliente "<<RED<<c.getId()<<YELLOW<<"] "<<MAGENTA<<"se quedo sin saldo. Esperando a que PaySystem lo atienda..."<<RESET<<std::endl;
                        std::unique_lock<std::mutex> ul(mutex_queue);
                        std::thread payment_thread (&PaySystem::add_payment, &paysystem, c.getId(), c.getBalance());
                        payment_thread.join();

                        cv_paysystem.wait(ul,[&c]{
                            return c.getId()==payment_id;
                        });
                        std::cout << GREEN << "[Cliente_" << c.getId() << "] Ha realizado un pago de 100 creditos." << RESET << std::endl;
                        c.restoreCredits(credits);
                        payment_id = -1;
                        ul.unlock();
                    
                    }else{
                        c.payCredit();
                        if(c.getBalance() == 0 && !c.isPremium()) {
                            std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep a second since the execution of the free accounts are very fast
                            std::cout << RED << "[Cliente_" << c.getId() << "] Tiene cuenta gratuita y se ha quedado sin saldo. Saliendo del sistema..." << RESET << std::endl;
                        }
                    }
                }
                access_balance.unlock();
                
                solution[1]=line[position];
                if(position+1==line.size()){ // En el caso que se encuentre la palabra objetivo al final de la linea, el valor de la palabra siguiente sera nulo 
                    solution[2]="";
                }else{
                    solution[2]=line[position+1];
                }
                WordSearched wordFounded(c.getObjective(), thread, begin, end, nLine+begin, solution[0], solution[1], solution[2]);
                std::lock_guard<std::mutex> semaphore(access_result); //Aquí bloquearemos el acceso a la estructura de datos
                vWords[thread].push_back(wordFounded);
            }
            solution[0]=line[position];//Las palabras ya leidas paran a la varible palabra anterior.
        }
        if(c.getBalance()==0 && !c.isPremium()){
            //std::cout<<YELLOW<<"[Cliente "<<RED<<c.getId()<<YELLOW<<"] "<<MAGENTA<<"se quedo sin saldo y no es premium. Saliendo del sistema..."<<RESET<<std::endl;
            break;
        }
    }
}

/* Devolvera una linea en un vector para recorrerla sin espacios*/
std::vector<std::string> splitLine(std::string line){
    std::string word;
    std::vector<std::string> result;
    std::stringstream buffer(line);
    while(std::getline(buffer, word, ' ')){
        result.push_back(word);
    }
    return result;
}

/* Se encarga de formatear una palabra para compararla con la con la palabra objetivo*/
std::string analize_word(std::string word){
    std::string result;
    for (std::size_t i = 0; i < word.length(); i++) { //Esto sera para pasarlos a minuscula
        word[i] = tolower(word[i]);
    }
    std::remove_copy_if(word.begin(), word.end(), std::back_inserter(result), std::ptr_fun<int, int>(&std::ispunct));//Y esto para eliminar simbilos que no sean letras
    return result;
}

/* Almacenamo la información de los clientes en su correspondiente log*/
void print_result(std::map<int,std::vector<WordSearched>> vWords, Client c, std::mutex& access_log){
    for (std::size_t i = 0; i < vWords.size(); i++){
        for (std::size_t j = 0; j < vWords[i].size(); j++){
            access_log.lock();
            c.writeLog(vWords[i][j].returnString());
            access_log.unlock();
        }   
    }
}

