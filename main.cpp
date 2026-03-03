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
// нов ветка
    std::cout << "🏎️ СИМУЛЯЦИЯ ГОНОК НАЧИНАЕТСЯ!\n\n";

    key_t barrier_key = ftok("/tmp", 'B');
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

    int total_points[MAX_CARS] = {0};

    for (int stage = 1; stage <= STAGES; ++stage) {
        std::cout << "\n=== ЭТАП " << stage << " ===\n";

        // 1. Отправляем сигнал старта всем машинам
        Message start_msg{};
        start_msg.mtype = MSG_START_STAGE;
        start_msg.stage = stage;
        for (int i = 0; i < MAX_CARS; ++i) {
            if (msgsnd(barrier.msgid, &start_msg, sizeof(Message) - sizeof(long), 0) == -1) {
                perror("msgsnd start");
                exit(1);
            }
        }

        // Ждём финиша всех машин
        Message results[MAX_CARS];
        for (int i = 0; i < MAX_CARS; ++i) {
            msgrcv(barrier.msgid, &results[i], sizeof(Message) - sizeof(long), MSG_FINISH_STAGE, 0);
        }

        // Сортируем по времени финиша (простая сортировка пузырьком)
        for (int i = 0; i < MAX_CARS - 1; ++i) {
            for (int j = 0; j < MAX_CARS - i - 1; ++j) {
                if (results[j].finish_time_ms > results[j + 1].finish_time_ms) {
                    std::swap(results[j], results[j + 1]);
                }
            }
        }

        // Начисляем баллы и места
        int points_table[] = {25, 18, 15, 12, 10};  // 1–5 место
        for (int i = 0; i < MAX_CARS; ++i) {
            results[i].place = i + 1;
            results[i].points = points_table[i];
            total_points[results[i].car_id] += results[i].points;

            // Отправляем результат машине (если нужно)
            //msgsnd(barrier.msgid, &results[i], sizeof(Message) - sizeof(long), 0);
        }

        // Красивый вывод результатов этапа
        std::cout << "\nРезультаты этапа " << stage << ":\n";
        std::cout << "┌─────┬────────────┬────────────┬────────┐\n";
        std::cout << "│ Место │ Машина №   │ Время (с)  │ Баллы  │\n";
        std::cout << "├─────┼────────────┼────────────┼────────┤\n";
        for (int i = 0; i < MAX_CARS; ++i) {
            std::cout << "│  " << results[i].place 
                      << "  │     " << results[i].car_id + 1 
                      << "     │ " << results[i].finish_time_ms / 1000.0 
                      << " │  " << results[i].points << "   │\n";
        }
        std::cout << "└─────┴────────────┴────────────┴────────┘\n";

        if (stage < STAGES) {
            std::cout << "\nНажмите Enter, чтобы продолжить...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    }

    std::cout << "\n=== ИТОГОВАЯ ТАБЛИЦА ===\n";
    std::cout << "┌────────────┬────────────┐\n";
    std::cout << "│ Машина №   │ Общие баллы│\n";
    std::cout << "├────────────┼────────────┤\n";
    for (int i = 0; i < MAX_CARS; ++i) {
        std::cout << "│     " << i + 1 << "      │     " << total_points[i] << "     │\n";
    }
    std::cout << "└────────────┴────────────┘\n";

    // Арбитр ждёт завершения всех машин
    for (int i = 0; i < MAX_CARS; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
        std::cout << "[Арбитр] Машина " << i + 1 << " завершила гонку.\n";
    }

    std::cout << "\n🏆 Все машины финишировали! Гонка окончена.\n";
    return 0;
}