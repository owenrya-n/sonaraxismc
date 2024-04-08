#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Tic.h>
#include <SoftwareSerial.h>

class Controller {
public:
    Controller();

    void setup();
    void resetCommandTimeout();
    void delayWhileResettingCommandTimeout(uint32_t ms);
    void waitForPosition(int32_t targetPosition);
    void moveTicPosition(int32_t delta);

private:
    SoftwareSerial ticSerial;
    TicSerial tic;
};

#endif
