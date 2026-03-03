#include "car.h"

Car::Car(int id, Barrier& barrier) : id(id), barrier(barrier) {}

void Car::drive_stage(int stage) {
    clock_t start = clock();
    std::cout << "🚗 Машина " << (id + 1) 
              << " начала этап " << stage << std::endl;

    for (int p = 0; p <= 100; p += 10) {
        std::cout << "   Машина " << (id + 1) 
                  << " [" << std::string(p/5, '#') 
                  << std::string(20 - p/5, '-') << "] " 
                  << p << "%" << std::endl;
        usleep(120000);  // имитация движения (120 мс)
    }
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;

    std::cout << "🏁 Машина " << (id + 1) 
              << " финишировала этап " << stage << "!" << std::endl;

    Message msg;
    msg.mtype     = MSG_FINISH_STAGE;
    msg.car_id    = id;
    msg.stage     = stage;
    msg.finish_time = static_cast<long>(elapsed * 1000);  // в миллисекундах для точности
    msgsnd(barrier.msgid, &msg, sizeof(Message) - sizeof(long), 0);
}

void Car::race() {
    for (int stage = 1; stage <= STAGES; ++stage) {
        Message start_msg;
        msgrcv(barrier.msgid, &start_msg, sizeof(Message) - sizeof(long), MSG_START_STAGE, 0);
        drive_stage(stage);
        barrier.wait(id, stage);
    }
    std::cout << "🎉 Машина №" << id + 1 << " завершила всю гонку!\n";
}