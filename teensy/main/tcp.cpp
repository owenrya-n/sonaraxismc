#include "tcp.h"
#include "network_config.h"
#include "imureader.h"
#include "controller.h"
#define SCALE_FACTOR 0.555*2

TelnetServer::TelnetServer() : server(23) {}
Controller controller;
void TelnetServer::begin() {
    Ethernet.begin(mac, ip); 
    server.begin();
    Serial.println("Ready to Connect");
    controller.setup();
    
}
float pos_diff = 0;
float pos_d = 0;
String TelnetServer::handleClient() {
    EthernetClient client = server.available();
    if (!client || !client.connected() || !client.available()) {
        return "";
    }

    String lastMessage = "";
    String incomingString = "";
    char c;

    while (client.available()) {
        c = client.read();
        if (c == '\n' || c == '\r') {
            break;
        }
        incomingString += c;
    }

    lastMessage = incomingString;
    if (lastMessage == "kill") {
        client.stop();
    }
    

    String output = parseClient(lastMessage);
    Serial.println(output);
    client.println(output);
    client.println();

    return lastMessage;
}

String TelnetServer::parseClient(String message) {
    String output = "";
    if (message.startsWith("?")) {
        int closingParenIndex = message.indexOf(';');
        String xString = message.substring(1, closingParenIndex);
        int x = xString.toInt();
        float axisValue;
        switch (x) {
            case 0:
                IMUReader::update();
                axisValue = IMUReader::getXAxisTangent();
                break;
            case 1:
                IMUReader::update();
                axisValue = IMUReader::getYAxisTangent();
                break;
            case 2:
                IMUReader::update();
                axisValue = IMUReader::getZAxisTangent();
                break;
            default:
                axisValue = 999.9; 
                break;
        }
        output = "Axis " + String(x) + " Position: " + String(axisValue);
    } 
    else if (message.startsWith("M")) {
        int commaIndex = message.indexOf(',');
        int closingParenIndex = message.indexOf(';');
        String aString = message.substring(1, commaIndex);
        String bString = message.substring(commaIndex + 1, closingParenIndex);
        int a = aString.toInt();
        int b = bString.toInt()*SCALE_FACTOR;
        switch (a) {
            case 0:
                output = "no motor at axis 2";
                break;
            case 1:
                output = "no motor at axis 1";
                break;
            case 2:
                controller.resetCommandTimeout();
                IMUReader::update();
                IMUReader::update();
                pos_d = IMUReader::getZAxisTangent();
                pos_diff = pos_d*SCALE_FACTOR - b;
                controller.moveTicPosition(pos_diff);
                break;
            default:
                output = "no motor at axis 3";
                break;
        
        }
        output = "set axis "+String(a)+" to position "+String(b);
    } else if (message.startsWith("h")) {
        output = "Available functions:\n\r"
                 "? axis_index - Returns the current angle of the specified axis\n\r"
                 "M axis_index, desired_angle - Set the angle of the specified axis\n\r"
                 "kill - Close the TCP connection";
    } else if (message == "kill") { 
      output = "closing connection";
    }
    return output;
}
