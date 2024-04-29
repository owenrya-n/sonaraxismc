#include "controller.h"
#include <Tic.h>
#include <SoftwareSerial.h>

#define MAX_SPEED 100000000
#define MAX_ACCEL 50000

Controller::Controller(int aio1, int aio2) : ticSerial(aio1, aio2), tic(ticSerial, 14) {}

void Controller::setup() {
    //Serial.begin(9600);
    ticSerial.begin(9600);
    delay(500);
    //tic.enterSafeStart();
    
    //tic.exitSafeStart();
    tic.deenergize();
    tic.energize();
    tic.setProduct(TicProduct::Tic36v4);
    //tic.haltAndSetPosition(0);
    tic.setMaxSpeed(MAX_SPEED);
    tic.setMaxAccel(MAX_ACCEL);
    tic.setMaxDecel(MAX_ACCEL);
    tic.setStepMode(TicStepMode::Half);
    if (tic.getEnergized()) {
      Serial.println("STATUS: A Motor Controller Initialized Successfully");
    } else {
      Serial.println("ERROR: A Motor Controller Failed to Initialize");
    }
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
    float pos = tic.getCurrentPosition();
    if (pos < rangeneg || pos > rangepos) {
        tic.haltAndHold();
        Serial.println("ERROR: CURRENT POSITION OTUSIDE ANGULAR RANGE");
    }

    float targetPos = pos + delta;
    if (targetPos >= rangeneg && targetPos <= rangepos) {
        tic.resetCommandTimeout();
        tic.setTargetPosition(targetPos);
    } else {
        Serial.println("ERROR: TARGET POSITION OUTSIDE ANGULAR RANGE");
    }
}

void Controller::ZeroTicPosition(int32_t zerodelta) {
    rangeneg = -9999;
    rangepos = 9999;
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
    while (digitalRead(16) == LOW) {
        tic.setTargetPosition(tic.getCurrentPosition() + 1);
        rangeneg++;
    }
    Serial.print("Steps taken until pin 14 is high: ");
    Serial.println(rangeneg);
}

void Controller::moveTicPositionLinear(int32_t delta) {
    tic.setCurrentLimit(1000);
    float pos = tic.getCurrentPosition();
    float targetPos = pos + delta;
    tic.setTargetPosition(targetPos);
    //tic.resetCommandTimeout();
}

void Controller::stop() {
    Serial.println("Emergency Stop Triggered");
    tic.setCurrentLimit(0);
    tic.deenergize();
}
