# WiFiMan
Wifi manager for ESP8266 with configurable web UI and ability to config mqtt, OTA update,config via serial   

## About password setting
- Soft AP password : Password of ESP8266 when in AP mode .Can be set with setApPasswd("yourPassword").If default password is not set, AP will fireup without password.
- HTTP access password : Password use to access HTTP config page.Default password and username can be set with setHttpPassword("youPassword") and setHttpUsername("yourUsername").If default password and username are not set, HTTP config page can be access without authentication.After masterPassword is set, HTTP access will use masterPassword instead of defaultPassword
- Master password : Password to save/edit device config. The default master password is blank, you must change it after first login.

## How to install 
- Method 1 : Install from Arduino IDE library manager 
- Method 2 : Manual download/clone this repo and put in arduino library folder

## Require library
- <a href="https://github.com/ChipTechno/ESP8266OTA">ESP8266OTA</a>
- <a href="https://github.com/bblanchon/ArduinoJson">bblanchon's ArduinoJson v5.13.2</a>

## API
- Constructor 
    WiFiMan(bool authentication,bool serialControl,bool debug);
    WiFiMan(bool authentication,bool serialControl);
    WiFiMan(bool authentication);
    WiFiMan();
    
- Config portal
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
    void setHttpPassword(String passwd);
    //set username to login (this cant be change later)
    void setHttpUsername(String username);
    //save config from json format
    bool setJsonConfig(String args);
  
- Get config parameters
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
