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

private:
    String parseClient(String message);
};

#endif
