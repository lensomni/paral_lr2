#include "car.h"
#include "common.h"
#include <sys/time.h>
#include <iostream>

Car::Car(int id, Barrier& barrier) : id(id), barrier(barrier) {
    srand(time(nullptr) + id);
    speed = 100 + (rand() % 51);
}

void Car::drive_stage(int stage) {
    barrier.waitAllReady(id);

    timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int line = 5 + id * 3;
    int pos = 0;

    // std::cout << "\033[" << line     << ";0H\033[K\033[" << pos << "C    ______";
    // std::cout << "\033[" << (line+1) << ";0H\033[K\033[" << pos << "C __/  __  \\__";
    // std::cout << "\033[" << (line+2) << ";0H\033[K\033[" << pos << "C'---O----O----'" ;
    // fflush(stdout);

    int distance_covered = 0;
    while (distance_covered < DISTANCE) {
        int progress = (distance_covered * 100) / DISTANCE;
        pos = (progress * 50) / 100;
        std::cout << "\033[" << line     << ";0H\033[K\033[" << pos << "C    ______";
        std::cout << "\033[" << (line+1) << ";0H\033[K\033[" << pos << "C __/  __  \\__";
        std::cout << "\033[" << (line+2) << ";0H\033[K\033[" << pos << "C'---O----O----'" ;
        fflush(stdout);

        usleep(1000000 / speed);
        distance_covered += speed;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed += (end.tv_nsec - start.tv_nsec) / 1000000.0;

    std::cout << "\033[" << (line + 3) << ";0H\033[K";

    Message msg;
    msg.mtype = MSG_FINISH_STAGE;
    msg.car_id = id;
    msg.stage = stage;
    msg.finish_time_ms = static_cast<long>(elapsed);
    msgsnd(barrier.getMsgQueueId(), &msg, sizeof(Message) - sizeof(long), 0);
}

void Car::race() {
    for (int stage = 1; stage <= STAGES; ++stage) {
        drive_stage(stage);
    }
}
