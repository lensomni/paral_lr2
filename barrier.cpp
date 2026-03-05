#include "barrier.h"

Barrier::Barrier(key_t key, int cars) {
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget (barrier)");
        exit(1);
    }
    car_count = cars;
    //std::cout << "[Барьер] Создан (msgid = " << msgid << ")\n";
}

Barrier::~Barrier() {
    msgctl(msgid, IPC_RMID, nullptr);
    //std::cout << "[Барьер] Удалён\n";
}