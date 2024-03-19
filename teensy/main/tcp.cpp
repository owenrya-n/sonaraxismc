  #include "tcp.h"
  #include "network_config.h" // Include network configuration

  TelnetServer::TelnetServer() : server(23) {}

  float xAxisTangent = 0.0;
  float yAxisTangent = 0.0;
  float zAxisTangent = 0.0;

  void TelnetServer::begin() {
    Ethernet.begin(mac, ip); // Use the mac and ip variables from network_config.h
    server.begin();
  }

  String TelnetServer::handleClient() {
    EthernetClient client = server.available();
    String lastMessage = "";

    if (client) {
      String incomingString = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          incomingString += c;
          if (c == '\n') {
            break;
          }
        }
      }
      lastMessage = incomingString;

      if (lastMessage == "kill") {
        client.stop();
      }
    }
    return lastMessage;
  }

  void TelnetServer::parseClient(EthernetClient &client, String message) {
    Serial.println("parsing");
    if (message.startsWith("get_axes(")) {
      int closingParenIndex = message.indexOf(')');
      String xString = message.substring(9, closingParenIndex);
      int x = xString.toInt();
      float axisValue;
      switch (x) {
        case 0:
          axisValue = xAxisTangent(); %this is where the problem is occuring
          break;
        case 1:
          axisValue = yAxisTangent();
          break;
        case 2:
          axisValue = zAxisTangent();
          break;
        default:
          axisValue = 0.0; 
          break;
      }
      client.print("Axis ");
      client.print(x);
      client.print(" tangent: ");
      client.println(axisValue);
    } else if (message.startsWith("set_axes(")) {
      int commaIndex = message.indexOf(',');
      int closingParenIndex = message.indexOf(')');
      String aString = message.substring(9, commaIndex);
      String bString = message.substring(commaIndex + 1, closingParenIndex);
      int a = aString.toInt();
      int b = bString.toInt();
      // Handle set_axes command
      client.print("Setting axis ");
      client.print(a);
      client.print(" to ");
      client.println(b);
    } else if (message == "h") {
      // Print help message
      client.println("Available functions:");
      client.println("get_axes(axis_index) - Returns the current angle of the specified axis");
      client.println("set_axes(axis_index, desired_angle) - Set the angle of the specified axis");
      client.println("kill - Close the TCP connection");
    } else if (message == "kill") {
      // Close TCP connection
      client.println("Connection closed.");
      client.stop();
    }


  }
