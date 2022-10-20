#include "MyWiFi.h"

#include <FS.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

#define CONFIG_STRING_LENGTH 16

using namespace std;

extern "C" {
  #include "user_interface.h"
}

bool shouldSaveConfig = false;

void MyWiFi::setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFi.SSID(), WiFi.psk());

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument jsonDoc(512);
        auto error = deserializeJson(jsonDoc, buf.get());
        serializeJson(jsonDoc, Serial);
        if(error)
        {
          Serial.println("failed to load json config");
        } else {
          Serial.println("\nparsed json");

          strcpy(m_hostname, jsonDoc["hostname"]);
          wifi_station_set_hostname(m_hostname);
          strcpy(m_server, jsonDoc["server"]);
        }
      }
    }
    else
    {
      Serial.println("config file not found");
      strcpy(m_hostname, wifi_station_get_hostname());
      sprintf(m_server, "piport");
    }
  } else {
    Serial.println("failed to mount FS");
  }

  strcpy(m_hostname, wifi_station_get_hostname());

  Serial.print("Connecting to WiFi");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the connection to the WiFi network

    delay(1000);
    Serial.print('.');
    if (i++ > 10)
    {
      MyWiFi::config();
    }
  }
  Serial.println("Connected");
}

void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void MyWiFi::config()
{
  WiFiManager wifiManager;

  WiFiManagerParameter hostname_config("Hostname", "hostname", m_hostname, CONFIG_STRING_LENGTH);
  WiFiManagerParameter server_config("Server", "server", m_server, CONFIG_STRING_LENGTH);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&hostname_config);
  wifiManager.addParameter(&server_config);
  wifiManager.setMinimumSignalQuality(30);
  wifiManager.setConfigPortalTimeout(300);

  if (!wifiManager.startConfigPortal(m_hostname))
  {
    ESP.restart();
  }

  if (shouldSaveConfig) {
    DynamicJsonDocument jsonDoc(512);

    strcpy(m_hostname, hostname_config.getValue());
    jsonDoc["hostname"] = m_hostname;

    strcpy(m_server, server_config.getValue());
    jsonDoc["server"] = m_server;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(jsonDoc, Serial);
    serializeJson(jsonDoc, configFile);
    //json.printTo(Serial);
    //json.printTo(configFile);
    configFile.close();

    ESP.restart();
  }

  WiFi.mode(WIFI_STA);

}

void MyWiFi::loop()
{
  if ( digitalRead(0) == LOW )
  {
    Serial.println("push");
    MyWiFi::config();
  }
}
