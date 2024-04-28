#include "tcp.h"
#include "network_config.h"
#include "imureader.h"
#include "controller.h"


TelnetServer::TelnetServer() : server(23) {
}


void TelnetServer::begin() {
    Ethernet.begin(mac, ip); 
    server.begin();
    Serial.println("Status: Telnet Server Initialized"); 
}


String TelnetServer::handleClient() {
    EthernetClient client = server.available();
    if (!client || !client.connected() || !client.available()) {
        return "";
    } //else {
    //  Serial.println("Status: Command recieved from client");
    //}
    String lastMessage = "";
    String incomingString = "";
    String prev_msg = "///";
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
    else if (message.startsWith("Z")) {
        int closingParenIndex = message.indexOf(';');
        String xString = message.substring(1, closingParenIndex);
        int x = xString.toInt();
        switch (x) {
            case 1:
                return 501;
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
        int b = bString.toInt()*0.55;
        switch (a) {
            case 0:
                des_pos = b;
                return 101;
                break;
            case 3:
                des_pos = b;
                return 671;
                break;
            default:
                des_pos = 0;
                return 000;
                break;
        }
        output = "set axis "+String(a)+" to position "+String(b);
    } 
    else if (message.startsWith("h")) {
        return 200;
    } 
    else {
      return 999;
    }
}


void TelnetServer::printClient(String cmessage){
    //if (server.available()) {
    EthernetClient client = server.available();
    Serial.println(cmessage);
    client.println(cmessage);
    //new_msg = cmessage;
}

