#pragma once
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <string>

#define MAX_CARS 5
#define STAGES   3

// Типы сообщений
#define MSG_START_STAGE   1     // Арбитр → машины (начало этапа)
#define MSG_FINISH_STAGE  2     // Машина → Арбитр (финиш этапа)

struct Message {
    long mtype;
    int  car_id;
    int  stage;
    int  place;      // место на этапе (будет заполнять арбитр)
    int  points;     // баллы за этап
};