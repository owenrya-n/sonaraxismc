#include <NativeEthernet.h>
#include <SPI.h>
#include "tcp.h"
//#include "network_config.h"
#include "imureader.h"
#include "controller.h"

TelnetServer telnetServer;
EthernetClient client;

//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 1, 177);

String cmd = "";
int state = 0;
float pos_diff = 0;
float pos_d = 0;
Controller controller;

void setup() {
  Wire.begin();
  Wire2.begin();
  Wire1.begin();
  IMUReader::begin(); 
  telnetServer.begin();
  controller.setup();

}

void loop() {
  cmd = telnetServer.handleClient();
  state = telnetServer.parseClient(cmd);
  statemx(state);
  controller.resetCommandTimeout();

}

// Main state machine
void statemx(int state) {

  // Error state
  if(state == 000){
    telnetServer.printClient("Error: system encountered unrecognized state");
  }

  // Calibrate axis 1
  if(state == 501){
    controller.resetCommandTimeout();
    IMUReader::update();
    IMUReader::update();
    pos_d = IMUReader::getZAxisTangent();
    pos_diff = pos_d*controller.SCALE_FACTOR;
    controller.ZeroTicPosition(pos_diff);
  }


  // Query axis 1 angle
  if(state == 001){
    IMUReader::update();
    IMUReader::update();
    telnetServer.printClient(IMUReader::getZAxisTangent());
  }


  // Set axis 1 angle
  if(state == 101){
    controller.resetCommandTimeout();
    IMUReader::update();
    IMUReader::update();
    pos_d = IMUReader::getZAxisTangent();
    pos_diff = pos_d*controller.SCALE_FACTOR - telnetServer.des_pos;
    controller.moveTicPosition(pos_diff);
  }


  // Show help menu
  if(state == 200){
    telnetServer.printClient("Available functions:\n\r");
    telnetServer.printClient("? axis_index - Returns the current angle of the specified axis\n\r");
    telnetServer.printClient("M axis_index, desired_angle - Set the angle of the specified axis\n\r");
  }



}
