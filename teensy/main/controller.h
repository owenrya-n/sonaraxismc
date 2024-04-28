#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Tic.h>
#include <SoftwareSerial.h>

class Controller {
public:
    // Constructor
    Controller(int aio1, int aio2);
    // Member functions
    void setup();
    void resetCommandTimeout();
    void delayWhileResettingCommandTimeout(uint32_t ms);
    void waitForPosition(int32_t targetPosition);
    void moveTicPosition(int32_t delta);
    void ZeroTicPosition(int32_t zerodelta);
    void moveTicPositionLinear(int32_t delta);
    float SCALE_FACTOR = 0.55;

private:
    // Member variables
    SoftwareSerial ticSerial;
    TicSerial tic;
    int32_t rangeneg;
    int32_t rangepos;
};

#endif // CONTROLLER_H
