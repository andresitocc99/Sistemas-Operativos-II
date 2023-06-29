#include <iostream>
#include <thread>
#include <future>

std::promise<void> promise;
std::future<void> future = promise.get_future();

void hiloB() {
    std::cout << "Hilo B: Esperando notificación de A..." << std::endl;
    future.wait(); // Bloquea el hilo B hasta que se reciba la notificación de A
    std::cout << "Hilo B: Recibida notificación de A. Realizando servicio." << std::endl;
    // Realizar el servicio ofrecido por B
    std::cout << "Hilo B: Servicio realizado. Notificando a A." << std::endl;
    promise.set_value(); // Notificar a A que el servicio ha finalizado
}

void hiloA() {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Simular tiempo de preparación de A
    std::cout << "Hilo A: Solicitando servicio a B." << std::endl;
    promise.set_value(); // Notificar a B que se solicita el servicio
    //future.wait();
    // Solicitar servicio a B
    std::cout << "Hilo A: Servicio recibido de B. Continuando ejecución." << std::endl;
}

int main() {
    std::thread threadB(hiloB);
    std::thread threadA(hiloA);

    threadB.join();
    threadA.join();

    return 0;
}
