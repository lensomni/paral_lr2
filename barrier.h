#pragma once
#include "common.h"

class Barrier {
private:
    int msgid;
    int car_count;

public:
    Barrier(key_t key, int cars = MAX_CARS);
    ~Barrier();
    void wait(int car_id, int stage);   // каждый автомобиль вызывает после финиша этапа
};