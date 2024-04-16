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

    return lastMessage;
}

int TelnetServer::parseClient(String message) {
    String output = "";
    if (message.startsWith("?")) {
        int closingParenIndex = message.indexOf(';');
        String xString = message.substring(1, closingParenIndex);
        int x = xString.toInt();
        float axisValue;
        switch (x) {
            case 1:
                return 001;
                break;
            default:
                des_pos = 0;
                return 000;
                break;
        }
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
                des_pos = b;
                return 101;
                break;
            default:
                des_pos = 0;
                return 000;
                break;
        
        }
        output = "set axis "+String(a)+" to position "+String(b);
    } else if (message.startsWith("h")) {
        return 200;
    }

void TelnetServer::printClient(String cmessage){
    Serial.println(cmessage);
    client.println(cmessage);
}
}
