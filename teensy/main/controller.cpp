#include "controller.h"
#define MAX_SPEED 100000000
#define MAX_ACCEL 100000

Controller::Controller() : ticSerial(7, 8), tic(ticSerial, 14) {}

void Controller::setup() {
    Serial.begin(9600);
    ticSerial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

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

void Controller::resetCommandTimeout() {
    tic.resetCommandTimeout();
}

void Controller::delayWhileResettingCommandTimeout(uint32_t ms) {
    uint32_t start = millis();
    do {
        resetCommandTimeout();
    } while ((uint32_t)(millis() - start) <= ms);
}

void Controller::waitForPosition(int32_t targetPosition) {
    do {
        resetCommandTimeout();
    } while (tic.getCurrentPosition() != targetPosition);
}

void Controller::moveTicPosition(int32_t delta) {
    tic.resetCommandTimeout();
    float pos = tic.getCurrentPosition();
    tic.setTargetPosition(pos + delta);
}
