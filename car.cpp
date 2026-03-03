#include "car.h"

Car::Car(int id, Barrier& barrier) : id(id), barrier(barrier) {}

void Car::drive_stage(int stage) {
    std::cout << "🚗 Машина " << (id + 1) 
              << " начала этап " << stage << std::endl;

    for (int p = 0; p <= 100; p += 10) {
        std::cout << "   Машина " << (id + 1) 
                  << " [" << std::string(p/5, '#') 
                  << std::string(20 - p/5, '-') << "] " 
                  << p << "%" << std::endl;
        usleep(120000);  // имитация движения (120 мс)
    }

    std::cout << "🏁 Машина " << (id + 1) 
              << " финишировала этап " << stage << "!" << std::endl;
}

void Car::race() {
    for (int stage = 1; stage <= STAGES; ++stage) {
        drive_stage(stage);
        barrier.wait(id, stage);     // ждём всех остальных
    }

    std::cout << "🎉 Машина №" << id + 1 << " завершила всю гонку!\n";
}