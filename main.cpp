#include "common.h"
#include "barrier.h"
#include "car.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <cstdlib>

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));

    std::cout << "🏎️ СИМУЛЯЦИЯ ГОНОК НАЧИНАЕТСЯ!\n\n";

    key_t barrier_key = ftok(".", 'B');
    Barrier barrier(barrier_key);

    pid_t pids[MAX_CARS];

    // Запускаем 5 автомобилей
    for (int i = 0; i < MAX_CARS; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {               // дочерний процесс — машина
            Car car(i, barrier);
            car.race();
            exit(0);
        }
        else if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }
    }

    // Арбитр ждёт завершения всех машин
    for (int i = 0; i < MAX_CARS; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
        std::cout << "[Арбитр] Машина " << i + 1 << " завершила гонку.\n";
    }

    std::cout << "\n🏆 Все машины финишировали! Гонка окончена.\n";
    return 0;
}