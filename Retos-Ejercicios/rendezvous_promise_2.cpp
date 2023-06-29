#include <iostream>
#include <future>
#include <chrono>

// Función que representa el hilo B
void threadB(std::promise<int>& promiseB, std::future<void>& futureB)
{
    std::cout << "Hilo B: Esperando notificación de A..." << std::endl;
    futureB.wait(); // Bloquea el hilo B hasta que A envíe una notificación

    std::cout << "Hilo B: Recibida notificación de A. Realizando el servicio..." << std::endl;
    // Simulación del servicio que realiza B
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int resultado = 42; // El resultado del servicio

    promiseB.set_value(resultado); // Envía el resultado a A

    std::cout << "Hilo B: Servicio completado. Enviando notificación a A..." << std::endl;
    // Simulación del tiempo que lleva enviar la notificación a A
    std::this_thread::sleep_for(std::chrono::seconds(2));

    futureB.wait(); // Bloquea el hilo B hasta que A reciba la notificación del servicio completado

    std::cout << "Hilo B: Finalizando." << std::endl;
}

int main()
{
    std::promise<int> promiseB;
    std::future<int> futureB = promiseB.get_future();
    std::promise<void> promiseA;
    std::future<void> futureA = promiseA.get_future();
    std::thread threadA([&promiseA, &futureB]()
    {
        std::cout << "Hilo A: Solicitando servicio a B..." << std::endl;
        promiseA.set_value(); // Envía una notificación a B

        std::cout << "Hilo A: Esperando resultado del servicio..." << std::endl;
        int resultado = futureB.get(); // Espera el resultado del servicio

        std::cout << "Hilo A: Recibido resultado del servicio: " << resultado << std::endl;

        std::cout << "Hilo A: Notificando a B que el resultado ha sido recibido..." << std::endl;
        promiseA.set_value(); // Envía una notificación a B de que el resultado ha sido recibido
    });

    std::thread threadB(threadB, std::ref(promiseB), std::ref(futureA));

    threadA.join();
    threadB.join();

    return 0;