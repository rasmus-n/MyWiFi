#ifndef MYWIFI_H
#define MYWIFI_H

#include "Arduino.h"
#include <string>

class MyWiFi
{
  public:
    MyWiFi() {};
    void setup();
    void loop();
    const std::string& hostname();
    const std::string& server();
    void config();
  private:
    std::string m_hostname;
    std::string m_server = "piport";
};

#endif
