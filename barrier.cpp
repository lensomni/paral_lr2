#include "barrier.h"

Barrier::Barrier(key_t key, int cars) {
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget (barrier)");
        exit(1);
    }
    car_count = cars;
    std::cout << "[Барьер] Создан (msgid = " << msgid << ")\n";
}

Barrier::~Barrier() {
    msgctl(msgid, IPC_RMID, nullptr);
    std::cout << "[Барьер] Удалён\n";
}

void Barrier::wait(int car_id, int stage) {
    Message msg;

    // 1. Сообщаем арбитру, что мы финишировали этап
    msg.mtype = MSG_FINISH_STAGE;
    msg.car_id = car_id;
    msg.stage = stage;
    //msgsnd(msgid, &msg, sizeof(Message) - sizeof(long), 0);
    if (msgsnd(msgid, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
        perror("msgsnd (finish stage)");
        exit(1);
    }

    std::cout << "[Машина " << car_id + 1 << "] Сообщил о финише этапа " << stage << "\n";


    if (msgrcv(msgid, &msg, sizeof(Message) - sizeof(long), MSG_FINISH_STAGE, 0) == -1) {
        perror("msgrcv (barrier wait)");
        exit(1);
    }
    
}