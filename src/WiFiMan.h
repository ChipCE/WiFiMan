#ifndef __WIFI_MAN_H
#define __WIFI_MAN_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266OTA.h>
#include "WebUI.h"
#include "Config.h"
#include "DebugHelper.h"
#include "SerialController.h"
#include "Boot.h"

enum ACTION_TYPE {NONE,CONFIG_SAVED,CLEAR_CONFIG,SYS_RESET};
enum MODE {INIT,CONNECTING,CLIENT,AP,TIMEOUT};

class WiFiMan
{
  private:
    bool DEBUG = false;;
    bool AUTHENTICATION = false;;
    bool SERIALCONTROL = false;;
    bool FORCE_AP = false;

    //mode
    int _mode = MODE::INIT;

    //action flag
    int _action = 0;

    //for simple debug print
    DebugHelper debugHelper;
    //Serial controller
    SerialController serialController;
    
    /* server config */
    //number of connect attempt to AP
    int _maxConnectAttempt = 36;
    //timeout in AP mode (min), server will turnoff after timeout
    int _configTimeout = 15;
    //DNS server port in ap mode
    byte _DNS_PORT = 53;
    //in ap mode, SSID will be <defaultAPName>+<chipID>
    String _apName = "esp8266-id";
    String _apPasswd = "";
    
    //serial buffer
    String serialBuffer = "";

    //password use in the first time login to config portal.This can be changed in config menu
    String _defaultMasterPasswd = "password";
    //username to login (this cannot be changed in config portal)
    String _httpUsername = "admin";

    //Ap setting 
    IPAddress _apIp = IPAddress(192, 168, 1, 1);;
    IPAddress _apGateway = IPAddress(192, 168, 1, 1);
    IPAddress _apSubnet = IPAddress(255, 255, 255, 0);
    String _defaultMqttId = "esp8266";

    /* config , this data will be stored in config.json */
    String _wifiSsid = "";
    String _wifiPasswd = "";
    String _mqttAddr = "";
    String _mqttPort = "";
    String _mqttUsername = "";
    String _mqttPasswd = "";
    String _mqttSub = "";
    String _mqttPub = "";
    String _mqttId = "";
    String _masterPasswd = "";

    //web ui 
    String _title = "ESP8266";
    String _banner = "Config portal";
    String _build = "Build : ";
    String _branch = "Branch : ";
    String _deviceInfo = "ChipId : " + String(ESP.getChipId());
    String _footer = "ESP8266 WiFiMan";
    String _helpInfo = "Please visit homepage for more information!";

    //controller objects
    std::unique_ptr<DNSServer> dnsServer;
    std::unique_ptr<ESP8266WebServer> webServer;
    std::unique_ptr<ESP8266OTA> otaUpdater;

    //web handles
    void handleRoot();
    void handleConfig();
    void handleClearSetting();
    void handleReset();
    void handleNotFound();
    void handleSave();
    void handlePortal();
    void handleHelp();
    //void handleUpdate(); will be automatically handled by otaUpdater

    //web and dns server control
    void setupWebServer();
    void stopWebServer();
    void stopDnsServer();

    

    //return ap and web access password , if masterPassword is not set, return default password
    const char* getApPassword();
    //read config.json
    bool readConfig();
    //write new setting to config.json, new config also be reload
    bool writeConfig(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd);
    //check if user input are valid
    bool validConfig();
    //client-mode connect to AP
    bool clientMode();
    //ap-mode , if client-mode failed to connect to AP , softAP will start
    bool apMode();
    //check user input and return error msg if nedded
    String checkInput(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd,String confirmPasswd); 
    //connect to ap , send multicast dns for webserver if server is alive
    bool connect(String wifiSsid,String wifiPasswd);
    //get boot mode, if true : skip auto connect, false : normal mode
    bool getBootMode();
    
    
  public:
    WiFiMan(bool authentication,bool serialControl,bool debug);
    WiFiMan(bool authentication,bool serialControl);
    WiFiMan(bool authentication);
    WiFiMan();
    //~WiFiMan();

    //start wifi manager 
    void start();
    //delete config.json .Call this method before start() to clear all setting
    bool deleteConfig();
    //for disconnect from AP
    void disconnect();
    //check connection status
    bool isConnected();
    //get device mode
    int getStatus();
    

    //enable/disable serial debug
    void setDebug(bool enable);
    //enable/disable webserver authentication
    void setAuthentication(bool enable);
    //set serial control
    void setSerialControl(bool enable);
    //force config mode 
    void forceApMode();
    //Change WebUI of config portal
    void setWebUi(String title,String banner,String build,String branch,String deviceInfo,String footer);
    //Change WebUI of config portal,deviceInfo will be fill with chipID
    void setWebUi(String title,String banner,String build,String branch,String footer);
    // set text of "help" page
    void setHelpInfo(String helpInfo);
    //set ap ip/subnet/gateway
    void setApConfig(IPAddress ip,IPAddress gateway,IPAddress subnet);
    //set max connect attempt
    void setMaxConnectAttempt(int connectAttempt);
    //set timeout of AP mode (min), server will turnoff after timeout
    void setConfigTimeout(int timeout);
    //set default ap SSID .in ap mode, SSID will be <apName>+<chipID>
    void setApName(String apName);
    //set softAP password 
    void setApPasswd(String passwd);
    //set password use in the first time login.This can be changed in config menu
    void setDefaultMasterPasswd(String passwd);
    //set username to login (this cant be changed later)
    void setMasterUsername(String username);

    
    //get SSID
    String getWifiSsid();
    //get wifi password
    String getWifiPasswd();
    //get mqtt server address
    String getMqttServerAddr();
    //get mqtt server password
    String getMqttServerPasswd();
    //get mqtt server username
    String getMqttUsername();
    //get mqtt id
    String getMqttId();
    //get mqtt sub topic
    String getMqttSub();
    //get mqtt pub  topic 
    String getMqttPub();
    //get mqtt port
    int getMqttPort();
    //get soft AP ip 
    IPAddress getSoftApIp();
    //get ip in client mode
    IPAddress getIp();
    //get dns name 
    String getDnsName();
    //get device mac address
    String getMacAddr();
    //get all the config . return true if the config is valid(success connected to ap)
    bool getConfig(Config *conf);

};
#endif
