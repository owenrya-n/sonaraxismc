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
    void ZeroTicPosition(int32_t zerodelta);
    SoftwareSerial ticSerial;
    TicSerial tic;
    float rangeneg;
    float rangepos;
    float SCALE_FACTOR = 0.55;
    

private:

};

#endif
