#ifndef TCP_H
#define TCP_H

#include <NativeEthernet.h>
#include <SPI.h>
#include "imureader.h"

class TelnetServer {
private:
    EthernetServer server;

public:
    TelnetServer();
    void begin();
    String handleClient();
    void printClient(String cmessage);
    float des_pos;
    int parseClient(String message);

private:
    
};

#endif
