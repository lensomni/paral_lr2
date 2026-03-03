#include "common.h"
#include "barrier.h"
#include "car.h"
#include <sys/wait.h>
#include <vector>

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));

    std::cout << "🏎️  СИМУЛЯЦИЯ ГОНОК НАЧИНАЕТСЯ!\n" << std::endl;

    key_t barrier_key = ftok(".", 'B');
    Barrier barrier(barrier_key);

    std::vector<pid_t> pids(MAX_CARS);

    // Запускаем 5 автомобилей
    for (int i = 0; i < MAX_CARS; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {                 // дочерний процесс
            Car car(i, barrier);
            car.race();
            exit(0);
        }
    }

    // Арбитр ждёт завершения всех машин
    for (int i = 0; i < MAX_CARS; ++i) {
        waitpid(pids[i], nullptr, 0);
    }

    std::cout << "\n🏆 Все автомобили завершили гонки!\n";
    std::cout << "Программа завершена.\n";

    return 0;
}