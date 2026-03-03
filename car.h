#pragma once
#include "barrier.h"

class Car {
private:
    int id;
    Barrier& barrier;
    int speed;

public:
    Car(int id, Barrier& barrier);
    void race();                    // главный цикл автомобиля
    void drive_stage(int stage);    // имитация прохождения этапа
};