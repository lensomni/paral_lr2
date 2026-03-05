#pragma once
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h> 
#include <sys/msg.h> 
#include <stdio.h> 

struct BarrierStatus {
    bool isAllReady;
    int countWait;
};

class Barrier {
private:
    int shmid;
    BarrierStatus* status;
    int car_count;
    int msgid; 

public:
    Barrier(int cars, key_t msg_key);
    ~Barrier();
    void waitAllReady(int car_id);
    void startStage();
    void waitStageEnd();
    int getMsgQueueId() const;
};
