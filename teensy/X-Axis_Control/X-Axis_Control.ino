#include <Tic.h>
#include <SoftwareSerial.h>
#include <Encoder.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define MAX_SPEED 5000000
#define MAX_ACCEL 50000

const int blink_led = LED_BUILTIN;
const byte x_encoder_A = 2;
const byte x_encoder_B = 3;
const byte button_interrupt_1_check = 9;
const byte button_interrupt_1 = 10;
const byte button_interrupt_2 = 11;
const byte button_interrupt_2_check = 12;
const byte tic1_RX = 7;
const byte tic1_TX = 8;

Encoder x_axis_encoder(x_encoder_A, x_encoder_B); // pin 2 = A+, pin 3 = B+
SoftwareSerial ticSerial2(tic1_RX, tic1_TX); // pin 7 = RX, pin 8 = TX
TicSerial tic1(ticSerial2, 14); // Setting serial device number of a tic to 14

const uint16_t currentLimitWhileStopped = 0;
const uint16_t currentLimitWhileMoving = 1500; // 1000 mA
const float meters_per_step = 0.063*0.0254/200;

String command = "";
int speed = 0;
long position = 0;
float position_meters;
bool new_command = true;
volatile bool interrupt_1_enabled = false;
volatile bool motor_resetting_from_interrupt_1 = false;
volatile int debounce_counter_1 = 0;
volatile bool interrupt_2_enabled = false;
volatile bool motor_resetting_from_interrupt_2 = false;
volatile int debounce_counter_2 = 0;
long x_position_encoder = -999;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ticSerial2.begin(9600);
  pinMode(blink_led, OUTPUT);
  pinMode(button_interrupt_1_check, INPUT_PULLUP);
  pinMode(button_interrupt_1, INPUT_PULLUP);
  pinMode(button_interrupt_2, INPUT_PULLUP);
  pinMode(button_interrupt_2_check, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button_interrupt_1), interrupt1StopMotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_interrupt_2), interrupt2StopMotor, CHANGE);
  
  delay(20);

  tic1.exitSafeStart();

  tic1.deenergize();
  tic1.energize();
  tic1.setProduct(TicProduct::Tic36v4);
  tic1.haltAndSetPosition(0);
  tic1.setMaxSpeed(MAX_SPEED);
  tic1.setMaxAccel(MAX_ACCEL);
  tic1.setMaxDecel(MAX_ACCEL);
  tic1.setStepMode(TicStepMode::Microstep1);

  Serial.println("Setup Complete!");
}

// enum class TicStepMode
// {
//   Full    = 0,
//   Half    = 1,

//   Microstep1  = 0, // Full Step
//   Microstep2  = 1, // 1/2 Step
//   Microstep4  = 2, // 1/4 Step
//   Microstep8  = 3, // 1/8 Step
//   Microstep16 = 4, // 1/16 Step
//   Microstep32 = 5, // 1/32 Step
//   Microstep2_100p = 6, // 1/2 Step 100% -> Not sure what this means
//   Microstep64 = 7, // 1/64 Step
//   Microstep128 = 8, // 1/128 Step
//   Microstep256 = 9, // 1/256 Step
// };

// Interrupt function called whenever the Interrupt switch is pressed. It will stop the motor from moving and start moving it in reverse.
void interrupt1StopMotor() 
{
  // When the switch is pressed, it will halt and hold the motor, while clearing any commands that was running before.
  // To prevent bouncing, a interrupt_1_enabled is set as true after the first toggle, which indicates that the limit switch has been pressed.
  if (!interrupt_1_enabled) {
    tic1.haltAndHold();
    Serial.println("Motor emergency stopped (1)");
    command = "";
    interrupt_1_enabled = true;
  } 
  
  else if (motor_resetting_from_interrupt_1) {
    tic1.haltAndHold();
    Serial.println("Motor reset");
    motor_resetting_from_interrupt_1 = false;
    debounce_counter_1 = 0;
  }
}

void interrupt2StopMotor() 
{
  // When the switch is pressed, it will halt and hold the motor, while clearing any commands that was running before.
  // To prevent bouncing, a interrupt_1_enabled is set as true after the first toggle, which indicates that the limit switch has been pressed.
  if (!interrupt_2_enabled) {
    tic1.haltAndHold();
    Serial.println("Motor emergency stopped (2)");
    command = "";
    interrupt_2_enabled = true;
  } 
  
  else if (motor_resetting_from_interrupt_2) {
    tic1.haltAndHold();
    Serial.println("Motor reset");
    motor_resetting_from_interrupt_2 = false;
    debounce_counter_2 = 0;
  }
}

// TODO:
void resetCommandTimeout()
{
  tic1.resetCommandTimeout();
}

// TODO:
void delayWhileResettingCommandTimeout(uint32_t ms)
{
  uint32_t start = millis();
  do
  {
    resetCommandTimeout();
  } while ((uint32_t)(millis() - start) <= ms);
}

// TODO:
void waitForPosition(int32_t targetPosition)
{
  do
  {
    resetCommandTimeout();
  } while (tic1.getCurrentPosition() != targetPosition);
}

// enum class TicError {
//   IntentionallyDeenergized = 0,
//   MotorDriverError         = 1,
//   LowVin                   = 2,
//   KillSwitch               = 3,
//   RequiredInputInvalid     = 4,
//   SerialError              = 5,
//   CommandTimeout           = 6,
//   SafeStartViolation       = 7,
//   ErrLineHigh              = 8,
//   SerialFraming            = 16,
//   RxOverrun                = 17,
//   Format                   = 18,
//   Crc                      = 19,
//   EncoderSkip              = 20,
// };

// Function that prints out the errors based on their indexes 
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

// Function that reads the next command inputted into the Serial Monitor
void readNextCommand() {
  command = Serial.readStringUntil('\n');
  Serial.println("Command entered: " + command);
  if (command == "forward" || command == "reverse") {
    Serial.println("Enter the speed (pulses/second)");
    while (Serial.available() == 0) {}
    speed = Serial.readStringUntil('\n').toFloat();
    if (speed > MAX_SPEED/10000) { // Capping max speed
      speed = 10000;
      Serial.println("Max speed is: 10000");
    }
    Serial.println("Speed set to: " + String(speed));
    speed = speed * 10000;
  } else if (command == "position") {
    Serial.println("Enter the position");
    while (Serial.available() == 0) {}
    position_meters = Serial.readStringUntil('\n').toFloat();
    position = position_meters / meters_per_step;
    Serial.println("Target position set to: " + String(position_meters) + "m");
  } else if (command == "set position") {
    Serial.println("Enter the position");
    while (Serial.available() == 0) {}
    position_meters = Serial.readStringUntil('\n').toFloat();
    position = position_meters / meters_per_step;
    tic1.haltAndSetPosition(position*-1);
    x_axis_encoder.write(position*-1);
    Serial.println("Motor and encoder position set to " + String(position_meters));
  } else if (command == "get position") {
    Serial.println("Motor position value: " + String(tic1.getCurrentPosition()*-1));
    Serial.println("Encoder position value: " + String(x_axis_encoder.read()*-0.25));
    Serial.println("Motor position: " + String(tic1.getCurrentPosition()*-1 * meters_per_step) + "m");
    Serial.println("Encoder position: " + String(x_axis_encoder.read()*-0.25 * meters_per_step) + "m");
    command = "";
  }
  new_command = true;
}

void loop() {
  // put your main code here, to run repeatedly:

  long new_x_position_encoder;
  new_x_position_encoder = x_axis_encoder.read();
  if (new_x_position_encoder != x_position_encoder) {    
    // Serial.println("Encoder position = " + String(new_x_position_encoder * -1));
    x_position_encoder = new_x_position_encoder;
  }
  
  tic1.resetCommandTimeout(); // This needs to be sent constantly within a certain timeout period so the Tic knows it's connected to a device.
                              // Timeout is 1000ms
  
  if (digitalRead(button_interrupt_1) == LOW){ // Only happens once the switch is not pressed anymore.
    interrupt_1_enabled = false; // If it's true, we set the interrupt_1_enabled flag to false
  }

  if (digitalRead(button_interrupt_2) == LOW){ // Only happens once the switch is not pressed anymore.
    interrupt_2_enabled = false; // If it's true, we set the interrupt_1_enabled flag to false
  }

  // State: INTERRUPT_1
  if (interrupt_1_enabled) {
    Serial.println("Interrupt Enabled!");
    if (digitalRead(button_interrupt_1_check) == HIGH) {
      Serial.println("System stopped. Please reconnect limit switches!");
      tic1.haltAndHold();
      motor_resetting_from_interrupt_1 = true;
    } else if (debounce_counter_1 > 0) {
      tic1.setCurrentLimit(currentLimitWhileMoving);
      tic1.setTargetVelocity(-250*10000);
      motor_resetting_from_interrupt_1 = true;
    }
    debounce_counter_1++;
  } 
  // State: INTERRUPT_2
  else if (interrupt_2_enabled) {
    Serial.println("Interrupt Enabled!");
    if (digitalRead(button_interrupt_2_check) == HIGH) {
      Serial.println("System stopped. Please reconnect limit switches!");
      tic1.haltAndHold();
      motor_resetting_from_interrupt_2 = true;
    } else if (debounce_counter_2 > 0) {
      tic1.setCurrentLimit(currentLimitWhileMoving);
      tic1.setTargetVelocity(250*10000);
      motor_resetting_from_interrupt_2 = true;
    }
    debounce_counter_2++;
  } else {

    if (Serial.available() > 0) { // Checks if there is a new input in the Serial Line
      readNextCommand();
    }

    if (new_command) {
      tic1.setTargetVelocity(0);
      delayWhileResettingCommandTimeout(1000);
      tic1.exitSafeStart(); // Need to clear safe start after Teensy waited too long for a command
      // delay(200); // Need 200ms to let the motor start up properly
      new_command = false;
    }

    if (command == "forward") {
      digitalWrite(blink_led, LOW);
      tic1.setCurrentLimit(currentLimitWhileMoving);
      tic1.setTargetVelocity(-1 * speed); // Forward
      Serial.println("Running at velocity: " + String(speed));
    
    } else if (command == "reverse") {
      digitalWrite(blink_led, LOW);
      tic1.setCurrentLimit(currentLimitWhileMoving);
      tic1.setTargetVelocity(speed); // Reverse
      Serial.println("Running at velocity: -" + String(speed));

    } else if (command == "position") {
      tic1.setCurrentLimit(currentLimitWhileMoving);
      tic1.setTargetPosition(position*-1); // Reset to position 0
      Serial.println("Moving to position " + String(position_meters) + "m");
      if (tic1.getCurrentPosition() == position*-1) {
        Serial.println("Position reached!");
        command = ""; // Reset to idle mode
      }
    } else {
      // tic1.haltAndHold(); // Stops motor abruptly without deceleration. Could be used in other places.
      digitalWrite(blink_led, HIGH);
      // Serial.println("Motor not running");
      while (tic1.getCurrentVelocity() != 0) {
        tic1.setCurrentLimit(currentLimitWhileMoving);
        tic1.setTargetVelocity(0);
      }
      tic1.setCurrentLimit(currentLimitWhileStopped);
    }

  }

  uint32_t current_errors = tic1.getErrorsOccurred();
  showErrors(current_errors);
  delay(250);
}
