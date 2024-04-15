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
    if (pos < rangeneg || pos > rangepos) {
        tic.deenergize();
        Serial.println("ERROR: CURRENT POSITION OTUSIDE ANGULAR RANGE");
    }
    float pos = tic.getCurrentPosition();
    float targetPos = pos + delta;
    if (targetPos >= rangeneg && targetPos <= rangepos) {
        tic.resetCommandTimeout();
        tic.setTargetPosition(targetPos);
    } else {
        Serial.println("ERROR: TARGET POSITION OUTSIDE ANGULAR RANGE");
    }
}

void Controller::ZeroTicPosition(int32_t zerodelta) {
    moveTicPosition(zerodelta);
    int32_t rangepos = 0;
    while (digitalRead(15) == LOW) {
        tic.setTargetPosition(tic.getCurrentPosition() + 1);
        rangepos++;
    }
    Serial.print("Steps taken until pin 15 is high: ");
    Serial.println(rangepos);

    moveTicPosition(zerodelta);
    int32_t rangeneg = 0;
    while (digitalRead(14) == LOW) {
        tic.setTargetPosition(tic.getCurrentPosition() + 1);
        rangeneg++;
    }
    Serial.print("Steps taken until pin 14 is high: ");
    Serial.println(rangeneg);
}
