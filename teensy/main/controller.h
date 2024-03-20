#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Tic.h>

class Controller {
public:
    Controller();  // Constructor
    void setup();
    void resetCommandTimeout();
    void delayWhileResettingCommandTimeout(uint32_t ms);
    void waitForPosition(int32_t targetPosition);
    void moveTicToPosition(int32_t position);
    
private:
    TicI2C tic;
};

#endif
