#pragma once
#include "barrier.h"
#include <time.h>

class Car {
private:
    int id;
    Barrier& barrier;

public:
    Car(int id, Barrier& barrier);
    void race();
    void drive_stage(int stage);
};