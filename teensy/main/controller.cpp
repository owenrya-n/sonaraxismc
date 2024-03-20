#include "Controller.h"
#include <Wire.h>

Controller::Controller() {}

void Controller::setup() {
    // Set up I2C.
    Wire1.begin();

    // Give the Tic some time to start up.
    delay(20);

    // Set the Tic's current position to 0, so that when we command
    // it to move later, it will move a predictable amount.
    tic.haltAndSetPosition(0);

    // Tells the Tic that it is OK to start driving the motor.  The
    // Tic's safe-start feature helps avoid unexpected, accidental
    // movement of the motor: if an error happens, the Tic will not
    // drive the motor again until it receives the Exit Safe Start
    // command.  The safe-start feature can be disbled in the Tic
    // Control Center.
    tic.exitSafeStart();
    
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

void Controller::moveTicToPosition(int32_t position) {
    tic.setTargetPosition(position);
}
