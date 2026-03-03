#include "common.h"
#include "barrier.h"
#include "car.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <cstdlib>
#include <limits>

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));


    key_t barrier_key = ftok("/tmp", 'B');
    Barrier barrier(barrier_key);

    pid_t pids[MAX_CARS];

    for (int i = 0; i < MAX_CARS; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {             
            Car car(i, barrier);
            car.race();
            exit(0);
        }
        else if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }
    }

    int total_points[MAX_CARS] = {0};

    for (int stage = 1; stage <= STAGES; ++stage) {
        std::cout << "\033c";
        std::cout << "\n=== ЭТАП " << stage << " ===\n";
                
        Message start_msg{};
        start_msg.mtype = MSG_START_STAGE;
        start_msg.stage = stage;
        for (int i = 0; i < MAX_CARS; ++i) {
            if (msgsnd(barrier.msgid, &start_msg, sizeof(Message) - sizeof(long), 0) == -1) {
                perror("msgsnd start");
                exit(1);
            }
        }

        Message results[MAX_CARS];
        for (int i = 0; i < MAX_CARS; ++i) {
            msgrcv(barrier.msgid, &results[i], sizeof(Message) - sizeof(long), MSG_FINISH_STAGE, 0);
        }

        for (int i = 0; i < MAX_CARS - 1; ++i) {
            for (int j = 0; j < MAX_CARS - i - 1; ++j) {
                if (results[j].finish_time_ms > results[j + 1].finish_time_ms) {
                    std::swap(results[j], results[j + 1]);
                }
            }
        }

        int points_table[] = {25, 18, 15, 12, 10}; 
        for (int i = 0; i < MAX_CARS; ++i) {
            results[i].place = i + 1;
            results[i].points = points_table[i];
            total_points[results[i].car_id] += results[i].points;

        }

        std::cout << "\nРезультаты этапа " << stage << ":\n";
        std::cout << "+-------+------------+------------+--------+\n";
        std::cout << "| Место | Машина №  |   Время    | Баллы  |\n";
        std::cout << "+-------+------------+------------+--------+\n";

        for (int i = 0; i < MAX_CARS; ++i) {
            printf("|   %2d  |     %2d     |  %6.2f с  |   %2d   |\n",
                results[i].place,
                results[i].car_id + 1,
                results[i].finish_time_ms / 1000.0,
                results[i].points);
        }
        std::cout << "+-------+------------+------------+--------+\n";

        if (stage < STAGES) {
            std::cout << "\nНажмите Enter, чтобы продолжить...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            
        }
    }

    for (int i = 0; i < MAX_CARS; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
        //std::cout << "[Арбитр] Машина " << i + 1 << " завершила гонку.\n";
    }

    struct CarResult {
        int car_id;
        int points;
    };
    CarResult final_results[MAX_CARS];

    for (int i = 0; i < MAX_CARS; ++i) {
        final_results[i].car_id = i + 1;
        final_results[i].points = total_points[i];
    }

    for (int i = 0; i < MAX_CARS - 1; ++i) {
        for (int j = 0; j < MAX_CARS - i - 1; ++j) {
            if (final_results[j].points < final_results[j + 1].points) {
                std::swap(final_results[j], final_results[j + 1]);
            }
        }
    }

    std::cout << "\n=== ИТОГОВАЯ ТАБЛИЦА ===\n";
    std::cout << "+------------+--------------+--------------+\n";
    std::cout << "| Место     | Машина №     | Общие баллы |\n";
    std::cout << "+------------+--------------+--------------+\n";

    for (int i = 0; i < MAX_CARS; ++i) {
        printf("|    %2d      |     %2d       |      %3d      |\n",
            i + 1,
            final_results[i].car_id,
            final_results[i].points);
    }
    std::cout << "+------------+--------------+--------------+\n";

    return 0;
}