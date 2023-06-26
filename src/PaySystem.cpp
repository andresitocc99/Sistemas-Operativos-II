/*********************************************
*   Project: Práctica 3 de Sistemas Operativos II 
*
*   Program name: ssooiigle.c
*
*   Author: Andres Castellanos Cantos
*
*   Date created:10-05-2022
*
*   Porpuse: Constructor del sistema de pago
|*********************************************/


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
#include <colours.h>
#include <queue>
#include <utility>
#include "Request.cpp"
#include <future>

class PaySystem {
private:
    std::queue<Request> queue;
    std::mutex mutex_paysystem;
    std::condition_variable& cv_paysystem;
    int& payment_id;
    bool& end_program;
    

public: 
    PaySystem(std::condition_variable& cv_paysystem, int& payment_id, bool& end_program);
    int get_paid();
    void add_payment (int n, int credit, std::promise<void> && done);
    void constant_check (std::future<void>&& done);
};

PaySystem::PaySystem(std::condition_variable& cv_paysystem, int& payment_id, bool& end_program)
    : cv_paysystem(cv_paysystem), payment_id(payment_id), end_program(end_program) {}

int PaySystem::get_paid() {
    return this-> payment_id;
}

void PaySystem::add_payment(int id, int credit, std::promise<void> && done ) {
    try {
        std::unique_lock<std::mutex> lock(mutex_paysystem);
        Request payment(id, credit);
        this->queue.push(std::move(payment));
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        done.set_value();
    } catch (const std::exception& e) {
        done.set_exception(std::current_exception());
    }
}


/*Metodo que se estara ejcutando hasta que cambie la flag de control indicando la finalizacion del programa*/
void PaySystem::constant_check (std::future<void>&& done){
    try {
        while(!end_program && !queue.empty() && done.wait_for(std::chrono::seconds(0)) != std::future_status::ready ){
                std::unique_lock<std::mutex> lock2(mutex_paysystem); 
                    cv_paysystem.wait(lock2, [this] { 
                return !queue.empty() ;
                });
                Request payment = this -> queue.front();
                this->queue.pop();
                this->payment_id = payment.getID();
                std::cout<<YELLOW<<"[PAYSYSTEM]"<<MAGENTA<<" Atendiendo la petición de recarga del Cliente "<<RED<<payment.getID()<<RESET<<std::endl;
                lock2.unlock();
                this->cv_paysystem.notify_all();
                std::this_thread::sleep_for(std::chrono::seconds(1)); 

        }
        if (end_program) { 
            std::cout<<YELLOW<<"[PAYSYSTEM]"<<MAGENTA<<" Shutdown..."<<RESET<<std::endl;
        }
    } catch (const std::exception& e) {
        std::cout <<"Error en el pago de: " << e.what() << std::endl;
    }

}