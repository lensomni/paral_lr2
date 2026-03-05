#include "common.h"
#include "barrier.h"
#include "car.h"
#include "results.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <cstdlib>
#include <limits>

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(nullptr));

    key_t msg_key = ftok("/tmp", 'B');
    Barrier barrier(MAX_CARS, msg_key);

    pid_t pids[MAX_CARS];
    for (int i = 0; i < MAX_CARS; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            Car car(i, barrier);
            car.race();
            exit(0);
        } else if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }
    }

    CarTotalResult total_results[MAX_CARS];
    for (int i = 0; i < MAX_CARS; ++i) {
        total_results[i].car_id = i + 1;
        total_results[i].total_points = 0;
    }

    for (int stage = 1; stage <= STAGES; ++stage) {
        std::cout << "\033c";
        std::cout << "\n=== ЭТАП " << stage << " ===\n";

        barrier.startStage();
        barrier.waitStageEnd();

        Message results[MAX_CARS];
        for (int i = 0; i < MAX_CARS; ++i) {
            msgrcv(barrier.getMsgQueueId(), &results[i], sizeof(Message) - sizeof(long), MSG_FINISH_STAGE, 0);
        }

        processStageResults(stage, results, total_results);
        printResults(stage, total_results);

        if (stage < STAGES) {
            std::cout << "\033[" << (22 + 7 + MAX_CARS) << ";0H";
            std::cout << "Нажмите Enter, чтобы продолжить...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    }

    return 0;
}