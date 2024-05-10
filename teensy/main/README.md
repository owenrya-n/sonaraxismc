# Teensy firmware for Airborne Sonar Motion Control

## Prerequisites
Install a recent version of Arduino IDE and the Teensy flashing utility.

Mac:
```
brew install --cask arduino-ide
brew install --cask teensy
```

Follow [these instructions](https://www.pjrc.com/teensy/tutorial.html) for setting up the Teensy toolchain in the Arduino IDE.

Install the following libraries in the Arduino IDE:
* Tic by Pololu
* SparkFun_ICM-20948_ArduinoLibrary

## Compilation
Open main.ino in the Arduino IDE.

Press the "Verify" button in the Arduino IDE. If successful, the output console should show the flash and RAM usage of the program.

## Flashing
Connect the Teensy to the computer using a microUSB cable. If the VUSB trace was cut on the Teensy, make sure to connect external power to the Teensy first.

Choose the correct port in the Arduino IDE (should say something like "Serial Teensy 4.1").

Press the "Upload" button in the Arduino IDE. The Teensy flashing program should pop up and show the uploading progress, and then reboot the Teensy. The red LED should flash on the Teensy.

After the Teensy reboots, you should be able to see messages in the Serial Monitor.
