#include "car.h"
#include <iostream>
#include <unistd.h>
#include <time.h>

Car::Car(int id, Barrier& b) : id(id), barrier(b) {}

void Car::drive_stage(int stage) {
    timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    std::cout << "🚗 Машина " << (id + 1) << " начала этап " << stage << std::endl;
std::cout.flush();
    for (int p = 0; p <= 100; p += 10) {
        std::cout << "   Машина " << (id + 1)
                  << " [" << std::string(p/5, '#')
                  << std::string(20 - p/5, '-') << "] "
                  << p << "%" << std::endl;
        usleep(300000);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000LL +
                      (end.tv_nsec - start.tv_nsec) / 1000000;

    std::cout << "🏁 Машина " << (id + 1) << " финишировала этап " << stage 
              << " за " << elapsed_ms << " мс" << std::endl;

    Message msg{};
    msg.mtype = MSG_FINISH_STAGE;
    msg.car_id = id;
    msg.stage = stage;
    msg.finish_time_ms = elapsed_ms;
    if (msgsnd(barrier.msgid, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
        perror("msgsnd finish");
        exit(1);
    }
}

void Car::race() {
    for (int stage = 1; stage <= STAGES; ++stage) {
        // Ждём сигнала от арбитра
        barrier.wait_start(stage);

        drive_stage(stage);

        // Сообщаем о финише и ждём завершения этапа всеми
        barrier.wait_finish(id, stage);
    }
    std::cout << "🎉 Машина №" << id + 1 << " завершила гонку!\n";
}