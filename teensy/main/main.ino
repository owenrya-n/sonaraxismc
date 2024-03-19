#include <NativeEthernet.h>
#include <SPI.h>
#include "tcp.h"
#include "network_config.h"
#include "imureader.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

TelnetServer telnetServer;
EthernetClient client;

enum State {
  IDLE,
  TELNET
};

State currentState = IDLE;
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 30; // Update interval in milliseconds

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Ethernet.begin(mac, ip);
  telnetServer.begin();
  IMUReader::begin(); // Initialize IMU
}

void loop() {
  switch (currentState) {
    case IDLE:
      handleIdleState();
      break;
    case TELNET:
      handleTelnetState();
      break;
  }
}

void handleIdleState() {
  // Check for Telnet connection
  String cmd = telnetServer.handleClient();
  if (cmd.length() > 0) {
    currentState = TELNET;
  }

  // Update IMU data
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= updateInterval) {
    IMUReader::update();
    lastUpdateTime = currentTime;
  }
}

void handleTelnetState() {
  // Handle Telnet communication
  String cmd = telnetServer.handleClient();
  if (cmd.length() > 0) {
    telnetServer.parseClient(client, cmd);
    if (cmd == "kill") {
      client.stop();
      currentState = IDLE;
    }
  }

  // Update IMU data
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= updateInterval) {
    IMUReader::update();
    lastUpdateTime = currentTime;
  }
}