#include "barrier.h"

Barrier::Barrier(int cars, key_t msg_key) : car_count(cars) {

    shmid = shmget(IPC_PRIVATE, sizeof(BarrierStatus), 0666);
    status = (BarrierStatus*)shmat(shmid, nullptr, 0);
    status->isAllReady = false;
    status->countWait = 0;

    msgid = msgget(msg_key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget (barrier)");
        exit(1);
    }
}

Barrier::~Barrier() {
    shmdt(status);
    shmctl(shmid, IPC_RMID, nullptr);
    msgctl(msgid, IPC_RMID, nullptr);
}

void Barrier::waitAllReady(int car_id) {
    status->countWait++;
    while (!status->isAllReady) {
        usleep(1000);
    }
    status->countWait--;
}

void Barrier::startStage() {
    while (status->countWait < car_count) {
        usleep(1000);
    }
    status->isAllReady = true;
}

void Barrier::waitStageEnd() {
    while (status->countWait != 0) {
        usleep(1000);
    }
    status->isAllReady = false;
}

int Barrier::getMsgQueueId() const {
    return msgid;
}
