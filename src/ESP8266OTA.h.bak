#ifndef __ESP8266_OTA_H
#define __ESP8266_OTA_H

/*
#ifdef DEBUG_ESP_PORT
  #define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MSG(...)
#endif
*/

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "WebUI.h"
#include "Theme.h"
#include "Boot.h"
#include "DebugHelper.h"

class ESP8266WebServer;

class ESP8266OTA
{
  private:
    ESP8266WebServer *_server;
    String _username = "";
    String _password = "";
    bool _authenticated;

    String _title = "OTA updater";
    String _banner = "OTA updater";
    String _build = "Build : ";
    String _branch = "Branch : ";
    String _deviceInfo = "ChipId : " + String(ESP.getChipId());
    String _footer = "ESP8266 OTA Updater";

    //apply themes
    void applyTheme(String &page);

  public:
    ESP8266OTA();

    void setup(ESP8266WebServer *server);

    void setup(ESP8266WebServer *server, const char * path);

    void setup(ESP8266WebServer *server, String username, String password);

    void setup(ESP8266WebServer *server, const char * path, String username, String password);

    void setTitle(String title);
    void setBanner(String banner);
    void setBuild(String build);
    void setBranch(String branch);
    void setDeviceInfo(String deviceInfo);
    void setFooter(String footer);

    //setup web UI
    void setUpdaterUi(String title,String banner,String build,String branch,String deviceInfo,String footer);
    //setup web UI , with chip ID auto generated
    void setUpdaterUi(String title,String banner,String build,String branch,String footer);
    //update passsword
    void updatePassword(String password);
};
#endif
