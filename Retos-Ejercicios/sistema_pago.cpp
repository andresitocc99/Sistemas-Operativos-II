/*Relacionado con Práctica 3: crear
sistema de pago sencillo
(singleton) que atienda peticiones
de una cola. Cada petición debe
incluir id del hilo solicitante,
cantidad a pagar, saldo y
semáforo que bloquee al
pagador. El sistema de pago
debe restar la cantidad del saldo
y enviar notificación.*/

#include <iostream> 
#include <queue> 
#include <mutex> 
#include <condition_variable> 
#include <thread> 
#include <cctype> 
#include <time.h> 
#include <vector>
#include <stdlib.h> 
#include <string.h> 
#include <functional> 
#include <cctype>
#include <future> 

class Pago { 
    private: 
        int id; 
        double cantidad; 
        double saldo; 
        
    public: 
        Pago(int id, double cantidad, double saldo) : id(id), cantidad(cantidad), saldo(saldo) {} 
        void sacarDinero() { saldo -= cantidad;} 
        double getSaldo() { return saldo; } 
        double getID() { return id; 
    } 
}; 

std::queue<Pago> cola; 
std::mutex mtx; 
std::condition_variable cond_var; 

void agregarPago(int n, std::promise<void> && done) { 

    try {
        for (int i = 0; i < n; i++) { 
            Pago pago(i, 20, i*100); 
            std::unique_lock<std::mutex> lock(mtx); cond_var.wait(lock, [] { 
                return cola.size()< 3 ;
            }); 
            cola.push(pago); 
            std::cout << "Agregado el pago: " << i << std::endl; 
            cond_var.notify_one(); 
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
        done.set_value();
    } catch (const std::exception& e){
        done.set_exception(std::current_exception());
    } 
}

void peticiones(std::future<void>&& done) { 
    try {
        while(done.wait_for(std::chrono::seconds(0)) != std::future_status::ready || !cola.empty()) { 
            std::unique_lock<std::mutex> lock2(mtx); 
            cond_var.wait(lock2, [] { 
                return !cola.empty() ;
            }); 
            Pago pago = cola.front(); 
            cola.pop(); 
            pago.sacarDinero(); 
            std::cout << "Pagado pago: " << pago.getID() << ", saldo: " << pago.getSaldo() << std::endl; 
            lock2.unlock(); 
            cond_var.notify_one(); 
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        } 
    } catch(const std::exception& e) {
        std::cout <<"Error en el consumidor: " << e.what() << std::endl;
    }
} 

int main() { 
    int n = 10; 

    std::promise<void> producerDone;
    std::future<void> producerFuture = producerDone.get_future();
    std::thread t_agreg(agregarPago, n, std::move(producerDone)); 
    std::thread t_pet(peticiones, std::move(producerFuture)); 
    t_agreg.join(); 
    t_pet.join(); 
    return 0; 
}