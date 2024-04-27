#ifndef CONTROLLER_LIN_H
#define CONTROLLERLIN__H

#include <Tic.h>
#include <SoftwareSerial.h>

class ControllerLin {
public:
    ControllerLin();

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
