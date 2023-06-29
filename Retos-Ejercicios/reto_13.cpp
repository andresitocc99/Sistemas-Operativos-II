/*Soluciones con variables de
condición el problema de los
filósofos comensales en la versión
en las que no se producía
interbloqueo gracias al uso de
pares de palillos contemplados
como una unidad indivisible*/

#include <iostream> 
#include <fstream> 
#include <vector> 
#include <thread> 
#include <mutex> 
#include <stdlib.h> 
#include <string.h> 
#include <functional> 
#include <cstdlib> 
#include <dirent.h> 
#include <condition_variable> 
#include <future> 
#include <chrono> 
#include <signal.h> 
#include <exception> 
#include <map> 
#include <filesystem> 

int nPalillos=2; 
std::condition_variable cond_var; 
std::mutex palillos; 
std::mutex mtx; 
std::vector<std::thread> filosofos; 

void filosofo(int i) { 
    while(true) { 
        palillos.lock(); 
        std::cout << "El filosofo " << i << " esta pensando" << std::endl; 
        palillos.unlock(); 
        std::this_thread::sleep_for(std::chrono::seconds(5)); 
        std::unique_lock<std::mutex> ul(mtx); 
        cond_var.wait(ul, []{ 
            return nPalillos > 0;
        }); 
        palillos.lock(); 
        nPalillos--; 
        ul.unlock(); 
        std::cout<<"El filosofo "<< i <<" esta comiendo" << std::endl; 
        palillos.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(10)); 
        palillos.lock(); 
        nPalillos++; 
        palillos.unlock(); 
        cond_var.notify_one(); 
    } 
} 
int main() { 
    for (int i = 0; i < 5; i++) { 
        filosofos.push_back(std::thread(filosofo, i)); 
    } 
    std::for_each(filosofos.begin(), filosofos.end(), std::mem_fn(&std::thread::join)); 
    return 0; 
}