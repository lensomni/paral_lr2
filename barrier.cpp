#include "barrier.h"
#include <iostream>

Barrier::Barrier(key_t key, int cars) {
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    car_count = cars;
}

Barrier::~Barrier() {
    msgctl(msgid, IPC_RMID, nullptr);
}


void Barrier::wait_finish(int car_id, int stage) {
    Message msg{};
    msg.mtype = MSG_FINISH_STAGE;
    msg.car_id = car_id;
    msg.stage = stage;
    if (msgsnd(msgid, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
        perror("msgsnd finish");
        exit(1);
    }
    // Ждём сигнала от арбитра о начале следующего этапа (если не последний этап)
    if (stage < STAGES) {
        Message start_msg;
        if (msgrcv(msgid, &start_msg, sizeof(Message) - sizeof(long), MSG_START_STAGE, 0) == -1) {
            perror("msgrcv start");
            exit(1);
        }
    }
}


void Barrier::wait_start(int stage) {
    Message msg;
    if (msgrcv(msgid, &msg, sizeof(Message) - sizeof(long), MSG_START_STAGE, 0) == -1) {
        perror("msgrcv start");
        exit(1);
    }
    if (msg.stage != stage) {
        std::cerr << "Ошибка: получен старт не того этапа!\n";
    }
}