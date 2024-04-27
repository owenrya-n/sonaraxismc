#include "controller_lin.h"
#include <Tic.h>
#include <SoftwareSerial.h>

#define MAX_SPEED 100000000
#define MAX_ACCEL 100000

//SoftwareSerial ticSerial0(7,8);
//TicSerial tic(ticSerial0, 14); 

//Controller::Controller() : {};
ControllerLin::ControllerLin() : ticSerial(7, 8), tic(ticSerial, 14) {}

void ControllerLin::setup() {
    Serial.begin(9600);
    ticSerial.begin(9600);

    tic.exitSafeStart();
    tic.deenergize();
    tic.energize();
    tic.setProduct(TicProduct::Tic36v4);
    tic.haltAndSetPosition(0);
    tic.setMaxSpeed(MAX_SPEED);
    tic.setMaxAccel(MAX_ACCEL);
    tic.setMaxDecel(MAX_ACCEL);
    tic.setStepMode(TicStepMode::Half);
}

void ControllerLin::resetCommandTimeout() {
    tic.resetCommandTimeout();
}

void ControllerLin::delayWhileResettingCommandTimeout(uint32_t ms) {
    uint32_t start = millis();
    do {
        resetCommandTimeout();
    } while ((uint32_t)(millis() - start) <= ms);
}

void ControllerLin::waitForPosition(int32_t targetPosition) {
    do {
        resetCommandTimeout();
    } while (tic.getCurrentPosition() != targetPosition);
}

void ControllerLin::moveTicPosition(int32_t delta) {
    float pos = tic.getCurrentPosition();
    float targetPos = pos + delta;
}

