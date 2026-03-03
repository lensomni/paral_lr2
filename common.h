#pragma once

#include <sys/msg.h>
#include <sys/ipc.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#define MAX_CARS  5
#define STAGES    3
#define DISTANCE  10000  // метров

#define MSG_START_STAGE   1
#define MSG_FINISH_STAGE  2

struct Message {
    long mtype;
    int  car_id;
    int  stage;
    long finish_time_ms;  // миллисекунды
    int  place;
    int  points;
};