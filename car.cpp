#include "car.h"

Car::Car(int id, Barrier& barrier) : id(id), barrier(barrier) {
    srand(time(NULL) + id);
    speed = 100 + (rand() % 51);  
}

void Car::drive_stage(int stage) {
    timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // std::cout << "Машина " << (id + 1) << " начала этап " << stage
    //           << " (скорость: " << speed << " м/с)" << std::endl;

    int distance_covered = 0;
    while (distance_covered < DISTANCE) {

        int progress = (distance_covered * 100) / DISTANCE;
        std::cout << "   Машина " << (id + 1)
                  << " [" << std::string(progress / 5, '#')
                  << std::string(20 - progress / 5, '-') << "] "
                  << progress << "%" << std::endl;

        usleep(1000000 / speed);  
        distance_covered += speed;  
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed += (end.tv_nsec - start.tv_nsec) / 1000000.0;

    // std::cout << "Машина " << (id + 1)
    //           << " финишировала этап " << stage
    //           << " за " << elapsed << " мс" << std::endl;

    Message msg;
    msg.mtype = MSG_FINISH_STAGE;
    msg.car_id = id;
    msg.stage = stage;
    msg.finish_time_ms = static_cast<long>(elapsed);
    msgsnd(barrier.msgid, &msg, sizeof(Message) - sizeof(long), 0);
}

void Car::race() {
    for (int stage = 1; stage <= STAGES; ++stage) {
        Message start_msg;
        msgrcv(barrier.msgid, &start_msg, sizeof(Message) - sizeof(long), MSG_START_STAGE, 0);
        drive_stage(stage);
        //barrier.wait(id, stage);
    }
    //std::cout << "Машина №" << id + 1 << " завершила всю гонку!\n";
}