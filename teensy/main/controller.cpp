#include "Controller.h"
#include <Wire.h>

Controller::Controller() {}


#define MAX_SPEED 100000000
#define MAX_ACCEL 100000

int pos = 0.55;
void Controller::setup() {
    // Set up I2C.
    Wire.begin();

    // Give the Tic some time to start up.
    delay(20);

    // Set the Tic's current position to 0, so that when we command
    // it to move later, it will move a predictable amount.
    tic.exitSafeStart();

    tic.deenergize();
    tic.energize();
    tic.setProduct(TicProduct::Tic36v4);
    tic.haltAndSetPosition(0);
    tic.setMaxSpeed(MAX_SPEED);
    tic.setMaxAccel(MAX_ACCEL);
    tic.setMaxDecel(MAX_ACCEL);
    
    Serial.print(tic.getEnergized());
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
    pos = tic.getCurrentPosition();
    tic.setTargetPosition(pos + delta);
    Serial.println("set cmd rec");
}
