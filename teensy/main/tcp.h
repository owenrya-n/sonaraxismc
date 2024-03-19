#ifndef TCP_H
#define TCP_H

#include <NativeEthernet.h>
#include "imureader.h"

class TelnetServer {
private:
  EthernetServer server;

public:
  TelnetServer();
  void begin();
  String handleClient();
  void parseClient(EthernetClient &client, String message);
};

#endif
