#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void procesoA() {
    // Realizar alguna operación
    // Notificar que el proceso A está listo
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one();
}

void procesoB() {
    // Esperar a que el proceso A esté listo
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return ready; });
    }

    // Continuar con la ejecución después de que el proceso A esté listo
    std::cout << "Proceso B: El proceso A está listo. Continuando la ejecución." << std::endl;
}

int main() {
    std::thread hiloA(procesoA);
    std::thread hiloB(procesoB);

    hiloA.join();
    hiloB.join();

    return 0;
}
