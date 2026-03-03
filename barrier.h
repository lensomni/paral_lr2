#pragma once
#include "common.h"

class Barrier {
public:
    int msgid;
    int car_count;

public:
    Barrier(key_t key, int cars = MAX_CARS);
    ~Barrier();
    void wait(int car_id, int stage); 
};