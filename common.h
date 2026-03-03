#pragma once

#include <sys/msg.h>
#include <sys/ipc.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#define MAX_CARS  5
#define STAGES    3

// Типы сообщений
#define MSG_START_STAGE   1     // Арбитр → всем машинам: "Начался этап N"
#define MSG_FINISH_STAGE  2     // Машина → Арбитру: "Я финишировал этап N"
#define MSG_RESULT        3     // Арбитр → машине: "Твоё место на этапе N — X"

// Структура сообщения
struct Message {
    long mtype;           // тип сообщения (MSG_START_STAGE, etc.)
    int  car_id;          // номер машины (0..4)
    int  stage;           // номер этапа (1..3)
    int  place;           // место на этапе (заполняет арбитр)
    int  points;          // набранные баллы за этап
};