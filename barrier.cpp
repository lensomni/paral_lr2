#include "barrier.h"

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

void Barrier::wait(int car_id, int stage) {
    Message msg;

    // 1. Сообщаем арбитру, что мы финишировали этап
    msg.mtype = MSG_FINISH_STAGE;
    msg.car_id = car_id;
    msg.stage = stage;
    msgsnd(msgid, &msg, sizeof(Message) - sizeof(long), 0);

    // 2. Ждём, пока ВСЕ 5 машин пришлют сообщение о финише
    for (int i = 0; i < car_count; ++i) {
        msgrcv(msgid, &msg, sizeof(Message) - sizeof(long), MSG_FINISH_STAGE, 0);
    }
}