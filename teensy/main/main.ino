#include <NativeEthernet.h>
#include <SPI.h>
#include "tcp.h"
#include "network_config.h"
#include "imureader.h"
#include "controller.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

TelnetServer telnetServer;
EthernetClient client;
String cmd = "";

void setup() {
  Wire.begin();
  Wire2.begin();
  Wire1.begin();
  IMUReader::begin(); 
  telnetServer.begin();
  //Wire.begin();
  //Controller.setup();
}

void loop() {
  IMUReader::update();
  cmd = telnetServer.handleClient();
  //Serial.println(cmd);
}
