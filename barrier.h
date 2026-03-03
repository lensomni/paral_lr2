#pragma once
#include "common.h"

class Barrier {
public:
    int msgid;
    int car_count;

    Barrier(key_t key, int cars = MAX_CARS);
    ~Barrier();
    void wait_finish(int car_id, int stage);  // машина сообщает о финише
    void wait_start(int stage);               // машина ждёт старта этапа
};