#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <Tic.h>

class Controller {
public:
    Controller();
    void setup();
    void resetCommandTimeout();
    void delayWhileResettingCommandTimeout(uint32_t ms);
    void waitForPosition(int32_t targetPosition);
    void moveTicPosition(int32_t delta);
    void ZeroTicPosition(int32_t zerodelta);

private:
    TicSerial ticSerial;
    TicI2C tic;
    int32_t rangepos;
    int32_t rangeneg;
};

#endif
