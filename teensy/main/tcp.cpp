#include "tcp.h"
#include "network_config.h"
#include "imureader.h"
#include "controller.h"


TelnetServer::TelnetServer() : server(23) {
}


void TelnetServer::begin() {
    Ethernet.begin(mac, ip); 
    server.begin();
    Serial.println("STATUS: Telnet Server Initialized"); 
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
    Serial.println(lastMessage);
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
            case 2:
                return 002;
                break;
             case 3:
                return 003;
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
            case 2:
                return 502;
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
            case 1:
                des_pos = b;
                return 101;
                break;
            case 2:
                des_pos = b;
                return 102;
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
        Serial.println(b);
        output = "set axis "+String(a)+" to position "+String(b);
    } 
    else if (message.startsWith("h")) {
        return 200;
    } 
    else if (message.startsWith("X")) {
        return 888;
    } 
    else {
      return 999;
    }
}


void TelnetServer::printClient(String cmessage){
    //if (server.available()) {
    EthernetClient client = server.available();
    String telmsg = cmessage + ";";
    Serial.println(telmsg);
    client.println(telmsg);
    //new_msg = cmessage;
}

