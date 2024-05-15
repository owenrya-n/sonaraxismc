#include "controller.h"
#include <Tic.h>
#include <SoftwareSerial.h>

#define MAX_SPEED 5000000
#define MAX_ACCEL 50000

Controller::Controller(int aio1, int aio2, TicStepMode stp) : ticSerial(aio1, aio2), tic(ticSerial, 14) {
  step = stp;
}

void Controller::setup() {

    //interrupt setup
    pinMode(interrupt_x_1, INPUT_PULLUP);
    pinMode(interrupt_x_2, INPUT_PULLUP);
    pinMode(interrupt_ang_1, INPUT_PULLUP);
    pinMode(interrupt_ang_2, INPUT_PULLUP);
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
    tic.setStepMode(step);//TicStepMode::Half
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

float Controller::returnPosition() {
    return tic.getCurrentPosition();
    Serial.println(-1*tic.getCurrentPosition());
    Serial.print(tic.getPositionUncertain());
}

void Controller::moveTicPosition(float delta) {
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

void Controller::ZeroTicPositionLinear(){
    while (digitalRead(10) == HIGH) {
        tic.setTargetPosition(tic.getCurrentPosition() - 1);
    }

}


void Controller::moveTicPositionLinear(float del) {
    tic.setCurrentLimit(1000);
    //float pos = tic.getCurrentPosition();
    //float targetPos = pos + del;
    tic.setTargetPosition(del);
    Serial.println(del + tic.getCurrentPosition());
    //delay(1000);
    //tic.resetCommandTimeout();
}

void Controller::stop() {
    Serial.println("Emergency Stop Triggered");
    tic.setCurrentLimit(0);
    tic.deenergize();
}

void Controller::checkInterrupts(){
    if((digitalRead(interrupt_x_1) == HIGH) || (digitalRead(interrupt_x_2) == HIGH)){
        Serial.println("ERROR: Interrupt Triggered");
        tic.setCurrentLimit(0);
        tic.deenergize();
    }
}

void Controller::refreshTicPosition(float position){
  //add try catch
  tic.haltAndSetPosition(position);
  Serial.println("Tic Position Synced to Encoder");
}

void showErrors(uint32_t errors) {
  if (errors & (1 << (uint8_t)TicError::IntentionallyDeenergized)) {
    Serial.println("Error: Intentionally Deenergized");
  }
  if (errors & (1 << (uint8_t)TicError::MotorDriverError)) {
    Serial.println("Error: Motor Driver Error");
  }
  if (errors & (1 << (uint8_t)TicError::LowVin)) {
    Serial.println("Error: Low Vin");
  }
  if (errors & (1 << (uint8_t)TicError::KillSwitch)) {
    Serial.println("Error: Kill Switch");
  }
  if (errors & (1 << (uint8_t)TicError::RequiredInputInvalid)) {
    Serial.println("Error: Required Input Invalid");
  }
  if (errors & (1 << (uint8_t)TicError::SerialError)) {
    Serial.println("Error: Serial Error");
  }
  if (errors & (1 << (uint8_t)TicError::CommandTimeout)) {
    Serial.println("Error: Command Timeout");
  }
  if (errors & (1 << (uint8_t)TicError::SafeStartViolation)) {
    Serial.println("Error: Safe Start Violation");
  }
  if (errors & (1 << (uint8_t)TicError::ErrLineHigh)) {
    Serial.println("Error: Error Line High");
  }
  if (errors & (1 << (uint8_t)TicError::SerialFraming)) {
    Serial.println("Error: Serial Framing");
  }
  if (errors & (1 << (uint8_t)TicError::RxOverrun)) {
    Serial.println("Error: Rx Overrun");
  }
  if (errors & (1 << (uint8_t)TicError::Format)) {
    Serial.println("Error: Format");
  }
  if (errors & (1 << (uint8_t)TicError::Crc)) {
    Serial.println("Error: CRC");
  }
  if (errors & (1 << (uint8_t)TicError::EncoderSkip)) {
    Serial.println("Error: Encoder Skip");
  }
}