#include <NativeEthernet.h>
#include <SPI.h>
#include "Encoder_ctrl.h"
#include "tcp.h"
#include "imureader.h"
#include "controller.h"

//Control Variables
String cmd = "";
int state = 0;
float pos_diff = 0;
float pos_d = 0;
float pos_meter = 0;
float pos_step = 0;
float enc_meter = 0;
float enc_step = 0;

//Physical Constants
float leadscrew_pitch = (0.063*0.0254/800); //steps per meter

//Instatniate motion axes
Controller x_axis_linear(7,8,TicStepMode::Half);
Controller roll(16,17,TicStepMode::Half);
Controller pitch(18,19,TicStepMode::Half);
IMUReader imu;
Encoder_ctrl x_enc(2,3);

//Instatntiate tcp
TelnetServer telnetServer;
void setup() {

  //Setup motion controllers
  x_axis_linear.setup();
  roll.setup();
  
  //Begin telnet server
  telnetServer.begin();

  //Initialize IMU reader and I2C
  imu.begin(); 

  //reset Encoder 0
  x_enc.reset();


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
  //telnetServer.printClient("test");
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

  if(state == 502){//Pitch Calibration State (Z1)
    pitch.resetCommandTimeout();
    imu.update();
    imu.update();
    pos_d = imu.getYAxisTangent();
    pos_diff = pos_d*roll.SCALE_FACTOR;
    pitch.ZeroTicPosition(pos_diff);
    telnetServer.printClient("ACK");
  }

  if(state == 503){//LINEAR CALIBRATION
    x_axis_linear.resetCommandTimeout();
    x_axis_linear.ZeroTicPositionLinear();
    x_enc.reset();
    x_axis_linear.moveTicPositionLinear(100);
    telnetServer.printClient("ACK");
  }

  if(state == 001){//Roll Position Query State (?1)
    imu.update();
    imu.update();
    telnetServer.printClient(-imu.getZAxisTangent());
  }

    if(state == 002){//Pitch Position Query State (?2)
    imu.update();
    imu.update();
    telnetServer.printClient(imu.getYAxisTangent());
  }

   if(state == 003){//X Position Query State (?2)
    //UNCOMMENT THIS TO USE ENCODER
    /*enc_step = x_enc.read();
    enc_meter = enc_step/leadscrew_pitch;
    telnetServer.printClient(enc_meter);//need to fix*/

    telnetServer.printClient(-1*x_axis_linear.returnPosition()*leadscrew_pitch);
  }

  if(state == 101){//Move Roll Axis State (M1,)
    roll.resetCommandTimeout();
    imu.update();
    imu.update();
    pos_d = imu.getXAxisTangent();
    pos_diff = pos_d*roll.SCALE_FACTOR - telnetServer.des_pos;
    roll.moveTicPosition(pos_diff);
    telnetServer.printClient("ACK");
  }

  if(state == 102){//Move Pitch Axis State (M2,)
    pitch.resetCommandTimeout();
    imu.update();
    imu.update();
    pos_d = imu.getYAxisTangent();
    pos_diff = pos_d*roll.SCALE_FACTOR - telnetServer.des_pos;
    pitch.moveTicPosition(pos_diff);
    telnetServer.printClient("ACK");
  }

  if(state == 671){//Move X Axis State (M9,)
    /*if we want to change tic pos to encoder
    pos_d = x_enc.read();
    x_axis_linear.refreshTicPosition(pos_d)
    */


    //pos_diff = pos_d*.25*.5 - telnetServer.des_pos*leadscrew_pitch; //this should be the difference in degrees
    pos_meter = telnetServer.des_pos;
    pos_step = pos_meter / leadscrew_pitch;
    x_axis_linear.moveTicPositionLinear(-1*pos_step);//need to fix this
    telnetServer.printClient("ACK");
    //Serial.println(telnetServer.des_pos*leadscrew_pitch);
  }

  if(state == 888){//Electronic ESTOP
    x_axis_linear.stop();
    roll.stop();
    telnetServer.printClient("DIS");
  }

}
