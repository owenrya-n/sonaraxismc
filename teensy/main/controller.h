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
    void moveTicPosition(float delta);
    void ZeroTicPosition(int32_t zerodelta);
    void moveTicPositionLinear(float del);
    void stop();
    float SCALE_FACTOR = 0.55;
    void checkInterrupts();
    void ZeroTicPositionLinear();

private:
    // Member variables
    SoftwareSerial ticSerial;
    TicSerial tic;
    int32_t rangeneg;
    int32_t rangepos;
    const byte interrupt_x_1 = 10;
    const byte interrupt_x_2 = 11;
    const byte interrupt_ang_1 = 12;
    const byte interrupt_ang_2 = 13;

    
};

#endif // CONTROLLER_H
