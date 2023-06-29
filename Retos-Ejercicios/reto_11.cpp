/* Soluciones con variables de
condición el problema de los
filósofos comensales en la versión
en las que no se producía
interbloqueo gracias al uso de
pares de palillos contemplados
como una unidad indivisible*/

#include <iostream> 
#include <queue> 
#include <mutex> 
#include <condition_variable> 
#include <thread> 
std::queue<int> cola; 
std::mutex mtx; 
std::condition_variable cond_var; 
int BUFFER = 5; 

void consumidor(int n) { 
    for(int i = 0;i < n; i++) { 
        std::unique_lock<std::mutex> lock(mtx); 
        cond_var.wait(lock, [] { 
            return !cola.empty();
        }); 
        int item = cola.front(); 
        cola.pop(); std::cout << "Consumidor: " << item<< std::endl; lock.unlock(); 
        cond_var.notify_one(); 
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
    } 
} 

void productor(int n) { 
    for(int i = 0;i < n; i++) { 
        std::unique_lock<std::mutex> lock(mtx); 
        if (cola.size() == BUFFER){ 
            cond_var.wait(lock, [] { 
            return !cola.size() == BUFFER;
            });
        } 
        cola.push(i); std::cout << "Productor: " << i << std::endl; 
    } 
    cond_var.notify_one(); 
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
} 

int main() { 
    int n = 10; 
    std::thread t_prod(productor, n); 
    std::thread t_cons(consumidor, n);
    t_prod.join(); 
    t_cons.join(); 
    return 0; 
}