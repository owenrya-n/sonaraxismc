#include "tcp.h"
#include "network_config.h"
#include "imureader.h"
#include "controller.h"

TelnetServer::TelnetServer() : server(23) {}
Controller controller;
void TelnetServer::begin() {
    Ethernet.begin(mac, ip); 
    server.begin();
    Serial.println("Ready to Connect");
    controller.setup();
}

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
    if (message.startsWith("get_axes(")) {
        int closingParenIndex = message.indexOf(')');
        String xString = message.substring(9, closingParenIndex);
        int x = xString.toInt();
        float axisValue;
        switch (x) {
            case 0:
                axisValue = IMUReader::getXAxisTangent();
                break;
            case 1:
                axisValue = IMUReader::getYAxisTangent();
                break;
            case 2:
                axisValue = IMUReader::getZAxisTangent();
                break;
            default:
                axisValue = 999.9; 
                break;
        }
        output = "Axis " + String(x) + " Position: " + String(axisValue);
    } 
    else if (message.startsWith("set_axes(")) {
        int commaIndex = message.indexOf(',');
        int closingParenIndex = message.indexOf(')');
        String aString = message.substring(9, commaIndex);
        String bString = message.substring(commaIndex + 1, closingParenIndex);
        int a = aString.toInt();
        int b = bString.toInt();
        switch (a) {
            case 0:
                controller.moveTicToPosition(b-IMUReader::getXAxisTangent());
                controller.waitForPosition(100);
                break;
            case 1:
                output = "no motor at axis 1";
                break;
            case 2:
                output = "no motor at axis 2";
                break;
            default:
                output = "no motor at axis 3";
                break;
        output = "set axis "+String(a)+" to position "+String(b);
        }
    } else if (message == "h") {
        output = "Available functions:\n"
                 "get_axes(axis_index) - Returns the current angle of the specified axis\n"
                 "set_axes(axis_index, desired_angle) - Set the angle of the specified axis\n"
                 "kill - Close the TCP connection";
    } else if (message == "kill") { 
      output = "closing connection";
    }
    return output;
}
