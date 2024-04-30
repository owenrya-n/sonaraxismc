#include <NativeEthernet.h>
#include <SPI.h>
#include "tcp.h"
#include "imureader.h"
#include "controller.h"

//Control Variables
String cmd = "";
int state = 0;
float pos_diff = 0;
float pos_d = 0;

//Instatniate motion axes
Controller x_axis_linear(7,8);
Controller roll(16,17);

//Instatntiate tcp
TelnetServer telnetServer;
IMUReader imu;
void setup() {



  //Setup motion controllers
  x_axis_linear.setup();
  roll.setup();
  
  //Begin telnet server
  telnetServer.begin();

  //Initialize IMU reader and I2C
  imu.begin(); 


}

void loop() {
  //recieve command from client
  cmd = telnetServer.handleClient();
  //parse command to determine desired state
  state = telnetServer.parseClient(cmd);
  //set state and perform action
  statemx(state);

  //check limit switches for collision event
  x_axis_linear.checkInterrupts();
  roll.checkInterrupts();

}

// Main state machine
void statemx(int state) {

  if(state == 000){ //Reset State
    telnetServer.printClient("RST");
  }

  if(state == 200){//Help Menu (help)
    telnetServer.printClient("Available functions:");
    telnetServer.printClient("help - Returns help menu");
    telnetServer.printClient("? axis_index - Returns the current angle of the specified axis");
    telnetServer.printClient("M axis_index, desired_angle - Set the angle of the specified axis");
    telnetServer.printClient("Z axis_index - Perform calibration/homing for specified axis");
  }

  if(state == 501){//Roll Calibration State (Z1)
    roll.resetCommandTimeout();
    imu.update();
    imu.update();
    pos_d = imu.getZAxisTangent();
    pos_diff = pos_d*roll.SCALE_FACTOR;
    roll.ZeroTicPosition(pos_diff);
    telnetServer.printClient("ACK");
  }

  if(state == 001){//Roll Position Query State (?1)
    imu.update();
    imu.update();
    telnetServer.printClient(imu.getZAxisTangent());
  }

  if(state == 101){//Move Roll Axis State (M1,)
    roll.resetCommandTimeout();
    imu.update();
    imu.update();
    pos_d = imu.getZAxisTangent();
    pos_diff = pos_d*roll.SCALE_FACTOR - telnetServer.des_pos;
    roll.moveTicPosition(pos_diff);
    telnetServer.printClient("ACK");
  }

  if(state == 671){//Move X Axis State (M9,)
    x_axis_linear.moveTicPositionLinear(telnetServer.des_pos);
    telnetServer.printClient("ACK");
  }

  if(state == 888){//Electronic ESTOP
    x_axis_linear.stop();
    roll.stop();
    telnetServer.printClient("DIS");
  }

}
