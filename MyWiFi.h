#ifndef MYWIFI_H
#define MYWIFI_H

#include "Arduino.h"
//#include <string>

class MyWiFi
{
  public:
    MyWiFi() {sprintf(m_server, "piport");};
    void setup();
    void loop();
//    const std::string& hostname();
    const char* hostname() {return m_hostname;};
//    const std::string& server();
    const char* server() {return m_server;};
    void config();
  private:
    char m_hostname[16];
    char m_server[16];
};

#endif
