#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

bool isThreadFinished = false;
std::mutex mutex;
std::condition_variable cv;

void targetThreadFunction() {
    // Realiza la tarea principal del hilo objetivo
    // ...

    // Marca el hilo objetivo como finalizado correctamente
    {
        std::lock_guard<std::mutex> lock(mutex);
        isThreadFinished = true;
    }

    // Notifica al hilo guardián que ha finalizado
    cv.notify_one();
}

void guardianThreadFunction(std::thread& targetThread) {
    // Espera a que el hilo objetivo termine
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, []{ return isThreadFinished; });
    }

    // Verifica si el hilo objetivo ha finalizado correctamente
    if (isThreadFinished) {
        std::cout << "El hilo objetivo ha finalizado correctamente." << std::endl;
    } else {
        std::cout << "El hilo objetivo no ha finalizado correctamente." << std::endl;
    }
}

int main() {
    // Crea el hilo objetivo
    std::thread targetThread(targetThreadFunction);

    // Crea el hilo guardián
    std::thread guardianThread(guardianThreadFunction, std::ref(targetThread));

    // Espera a que el hilo guardián termine
    guardianThread.join();

    return 0;
}
