/*********************************************
*   Project: Práctica 3 de Sistemas Operativos II 
*
*   Program name: ssooiigle.c
*
*   Author: Andres Castellanos Cantos
*
*   Date created:10-05-2022
*
*   Porpuse: Busqueda de una palabra en un fichero divido en hilos para agilizar el proceso.
|*********************************************/

#ifndef CLIENT
#define CLIENT

#include <iostream>
#include <colours.h>
#include <string>
#include <definitions.h>

class Client{
    private:
        int id;
        int init_balance;
        int balance;
        std::string objective;
        bool premium;
        float time;
        std::string path;
    
    public:
    
        Client(int id, bool premium, int balance, bool ilimited, std::string objective);
        int getId();
        int getBalance();
        int getInitialBalance();
        std::string getObjective();
        bool isPremium();
        void payCredit();
        void restoreCredits(int credits);
        void writeLog(std::string msg);
        void setExecutionTime(float time);
        void toString();
};

Client::Client(int id, bool premium, int balance, bool ilimited, std::string objective) {
    this->id = id;
    this->path="./log/Client_"+std::to_string(this->id)+".txt";
    this-> premium = premium;
    this->init_balance = this->balance;
    this->objective = objective;
    if (ilimited == true) {
        this->balance = -1;
    } else {
        this-> balance = balance;
    }
}

int Client::getId(){
    return this->id;
}
int Client::getBalance(){
    return this->balance;
}

int Client::getInitialBalance(){
    return this->init_balance;
}
bool Client::isPremium(){
    return this->premium;
}
void Client::payCredit(){
    this->balance--;
}

void Client::restoreCredits(int credits){
    this->balance=credits;
}

std::string Client::getObjective(){
    return this->objective;
}

void Client::setExecutionTime(float time){
    this->time=time;
    writeLog("Tiempo de ejecución total: "+std::to_string(time)+" segundos.");
}
void Client::toString(){
    if(this->premium){
        if(this->balance==-1){
            std::cout<<YELLOW<<"[Cliente "<<RED<<this->id<<YELLOW<<"]"<<MAGENTA<<":: Cuenta:"<<YELLOW<<" Premium"<<MAGENTA<<" :: Palabra: "<<CIAN<<this->objective<<MAGENTA<<" :: Saldo: "<<RED<<"UNLIMITED"<<RESET<<std::endl;
            writeLog("[Cliente "+std::to_string(id)+"]:: Cuenta: Premium :: Palabra: "+objective+" :: Saldo: UNLIMITED\n");
        }else{
            std::cout<<YELLOW<<"[Cliente "<<RED<<this->id<<YELLOW<<"]"<<MAGENTA<<":: Cuenta:"<<YELLOW<<" Premium"<<MAGENTA<<" :: Palabra: "<<CIAN<<this->objective<<MAGENTA<<" :: Saldo: "<<RED<<this->balance<<RESET<<std::endl;
            writeLog("[Cliente "+std::to_string(id)+"]:: Cuenta: Premium :: Palabra: "+objective+" :: Saldo:"+ std::to_string(balance)+"\n");
        } 
    }else{
        std::cout<<YELLOW<<"[Cliente "<<RED<<this->id<<YELLOW<<"]"<<MAGENTA<<":: Cuenta:"<<GREEN<<" Gratis"<<MAGENTA<<" :: Palabra: "<<CIAN<<this->objective<<MAGENTA<<" :: Saldo: "<<RED<<this->balance<<RESET<<std::endl;
        writeLog("[Cliente "+std::to_string(id)+"]:: Cuenta: Gratis :: Palabra: "+objective+" :: Saldo:"+ std::to_string(balance)+"\n");
    }
}

/*Funcion para almacenar informacion en el log del cliente correspondiente*/
void Client::writeLog(std::string msg){
    std::ofstream fd;
    fd.open(this->path.c_str(),std::fstream::app);
    fd << "";
    fd << msg;
    fd.close();
}

#endif