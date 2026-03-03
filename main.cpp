#include "common.h"
#include "barrier.h"
#include "car.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));

    std::cout << "🏎️ СИМУЛЯЦИЯ ГОНОК НАЧИНАЕТСЯ!\n\n";

    key_t barrier_key = ftok(".", 'B');
    Barrier barrier(barrier_key);

    pid_t pids[MAX_CARS];
    for (int i = 0; i < MAX_CARS; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            Car car(i, barrier);
            car.race();
            exit(0);
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

        // 2. Ждём финиша всех машин
        std::vector<Message> results;
        for (int i = 0; i < MAX_CARS; ++i) {
            Message msg;
            if (msgrcv(barrier.msgid, &msg, sizeof(Message) - sizeof(long), MSG_FINISH_STAGE, 0) == -1) {
                perror("msgrcv finish");
                exit(1);
            }
            results.push_back(msg);
        }

        // 3. Сортируем по времени финиша
        std::sort(results.begin(), results.end(), [](const Message& a, const Message& b) {
            return a.finish_time_ms < b.finish_time_ms;
        });

        // 4. Начисляем места и баллы
        int points_table[] = {25, 18, 15, 12, 10};
        for (size_t i = 0; i < results.size(); ++i) {
            results[i].place = i + 1;
            results[i].points = points_table[i];
            total_points[results[i].car_id] += results[i].points;
        }

        // 5. Выводим таблицу
        std::cout << "\nРезультаты этапа " << stage << ":\n";
        std::cout << "┌─────┬────────────┬───────────────┬────────┐\n";
        std::cout << "│ Место │ Машина №   │ Время (мс)    │ Баллы  │\n";
        std::cout << "├─────┼────────────┼───────────────┼────────┤\n";
        for (const auto& r : results) {
            std::cout << "│  " << r.place 
                      << "  │     " << r.car_id + 1 
                      << "     │ " << r.finish_time_ms 
                      << " │  " << r.points << "   │\n";
        }
        std::cout << "└─────┴────────────┴───────────────┴────────┘\n";

        std::cout << "\nПромежуточные баллы:\n";
        for (int i = 0; i < MAX_CARS; ++i) {
            std::cout << "Машина " << i + 1 << ": " << total_points[i] << " баллов\n";
        }
    }

    // Итоговая таблица
    std::cout << "\n=== ИТОГОВАЯ ТАБЛИЦА ===\n";
    std::cout << "┌────────────┬────────────┐\n";
    std::cout << "│ Машина №   │ Общие баллы│\n";
    std::cout << "├────────────┼────────────┤\n";
    for (int i = 0; i < MAX_CARS; ++i) {
        std::cout << "│     " << i + 1 << "      │     " << total_points[i] << "     │\n";
    }
    std::cout << "└────────────┴────────────┘\n";

    // Ожидание завершения
    for (int i = 0; i < MAX_CARS; ++i) {
        waitpid(pids[i], nullptr, 0);
    }

    std::cout << "\n🏆 Гонка завершена!\n";
    return 0;
}