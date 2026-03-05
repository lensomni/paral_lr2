#include "common.h"
#include "barrier.h"
#include "car.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <iomanip>

struct CarStageResult {
    long finish_time_ms;
    int place;
    int points;
};

struct CarTotalResult {
    int car_id;
    CarStageResult stage_results[STAGES];
    int total_points;
};

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

        for (int i = 0; i < MAX_CARS - 1; ++i) {
            for (int j = 0; j < MAX_CARS - i - 1; ++j) {
                if (results[j].finish_time_ms > results[j + 1].finish_time_ms) {
                    std::swap(results[j], results[j + 1]);
                }
            }
        }

        int points_table[] = {25, 18, 15, 12, 10};
        for (int i = 0; i < MAX_CARS; ++i) {
            int place = i + 1;
            int points = points_table[i];
            total_results[results[i].car_id].stage_results[stage - 1] = {results[i].finish_time_ms, place, points};
            total_results[results[i].car_id].total_points += points;
        }

        std::cout << "\033[22;0H";

        std::cout << "| Машина |";
        for (int s = 1; s <= stage; ++s) {
            std::cout << "      Этап " << s << "     |";
        }
        std::cout << "      Итог       |\n";

        std::cout << "+--------+";
        for (int s = 1; s <= stage; ++s) {
            std::cout << "-----------------+";
        }
        std::cout << "-----------------+\n";

        CarTotalResult sorted_results[MAX_CARS];
        for (int i = 0; i < MAX_CARS; ++i) {
            sorted_results[i] = total_results[i];
        }
        for (int i = 0; i < MAX_CARS - 1; ++i) {
            for (int j = 0; j < MAX_CARS - i - 1; ++j) {
                if (sorted_results[j].total_points < sorted_results[j + 1].total_points) {
                    std::swap(sorted_results[j], sorted_results[j + 1]);
                }
            }
        }

        int final_places[MAX_CARS];
        for (int i = 0; i < MAX_CARS; ++i) {
            for (int j = 0; j < MAX_CARS; ++j) {
                if (total_results[i].car_id == sorted_results[j].car_id) {
                    final_places[i] = j + 1;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CARS; ++i) {
            std::cout << "|   " << std::setw(2) << total_results[i].car_id << "   |";
            for (int s = 0; s < stage; ++s) {
                std::cout << " " << std::setw(2) << total_results[i].stage_results[s].place << " место"
                          << " | " << std::setw(2) << total_results[i].stage_results[s].points << " б |";
            }
            std::cout << " " << std::setw(2) << final_places[i] << " место"
                      << " |" << std::setw(3) << total_results[i].total_points << " б |\n";
        }
        std::cout << "\n\n";

        if (stage < STAGES) {
            std::cout << "\033[" << (22 + 7 + MAX_CARS) << ";0H";
            std::cout << "Нажмите Enter, чтобы продолжить...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    }

    return 0;
}
