# WiFiMan
Wifi manager for ESP8266 with configurable web UI and ability to config mqtt, OTA update,config via serial   

## About password setting
- Soft AP password : Password of ESP8266 when in AP mode .Can be set with setApPasswd("yourPassword").If default password is not set, AP will fireup without password.
- HTTP access password : Password use to access HTTP config page.Default password and username can be set with setHttpPassword("youPassword") and setHttpUsername("yourUsername").If authentication is not enabled,HTTP config portal can be access without authentication.If default password is not set the default password will be "password". After masterPassword is set, HTTP access will use masterPassword instead of defaultHttpPassword
- Master password : password to access HTTP config portal.This password must be change after first login.
*If master password has been changed.In config portal ,leave "New password" and "Confirm password" fields empty will keep the old password without change it.

## How to install 
- Method 1 : Install from Arduino IDE library manager 
- Method 2 : Manual download/clone this repo and put in arduino library folder

## Require library
- <a href="https://github.com/ChipTechno/ESP8266OTA">ESP8266OTA</a>
- <a href="https://github.com/bblanchon/ArduinoJson">bblanchon's ArduinoJson v5.13.2</a>

## API
### Constructor 
- WiFiMan();   
    Create default WiFiMan object without authentication,serial control and debug.
    Authentication,serial control and debug can be set with WiFiMan(bool authentication,bool serialControl,bool debug); or enable using config portal APIs.   
- WiFiMan(bool authentication);   
- WiFiMan(bool authentication,bool serialControl,bool debug);   
- WiFiMan(bool authentication,bool serialControl);   


### Config portal
- void setDebug(bool enable);   
    Enable serial debug.
- void setAuthentication(bool enable);   
    Enable authentication for http access.
- void setSerialControl(bool enable);   
    Enable control via serial.
- void setWebUi(String title,String banner,String build,String branch,String deviceInfo,String footer);   
    Set web UI of config portal.
- void setWebUi(String title,String banner,String build,String branch,String footer);   
    Set web UI of config portal, deviceInfo field will be automatically filled with ESP8266 ChipID.
- void setHelpInfo(String helpInfo);   
    Set content of "/help" page.
- void setApConfig(IPAddress ip,IPAddress gateway,IPAddress subnet);   
    Set Soft AP IP,Gateway,Subnet.If not configured , the default value is 192.168.1.1 192.168.1.1 255.255.255.0
- void setMaxConnectAttempt(int connectAttempt);   
    Limit the max connect attemp to other AP in client mode.Default is 36.
- void setConfigTimeout(int timeout);   
    Set timeout limit of config portal in minute.Default is 15 minutes.
- void setApName(String apName);  
    Set soft AP SSID.Default is "esp8266-id[ChipID]"
- void setApPasswd(String passwd);   
    Set soft AP password.
- void setHttpUsername(String username);   
    Set config portal username
- void setHttpPassword(String passwd);   
    Set config portal password
  
### Get config parameters
- String getWifiSsid();   
    Get AP SSID
- String getWifiPasswd();   
    Get AP password
- String getMqttServerAddr();   
    Get mqtt server address
- String getMqttServerPasswd();   
    Get mqtt server password
- String getMqttUsername();   
    Get mqtt server username
- String getMqttId();   
    Get mqtt id
- String getMqttSub();   
    Get mqtt sub topic
- String getMqttPub();   
    Get mqtt pub  topic 
- int getMqttPort();   
    Get mqtt port
- IPAddress getSoftApIp();   
    Get soft AP ip 
- IPAddress getIp();   
    Get ip in client mode
- String getDnsName();   
    Get mDNS name 
- String getMacAddr();   
    Get device mac address
- bool getConfig(Config *conf);   
    Get all config parameters. Return true if the config is valid(success connected to ap)

### Controls
- void start();   
    Start WiFiMan , all config API must be called before this function.
- bool deleteConfig();   
    Delete saved config file (config.json).This must be called before call start().
- void forceApMode();   
    Force device into Soft Access Point mode without trying to connect to saved config.
- void disconnect();  
    Force disconnect from AP.
- bool isConnected();   
    Check connection status.
- int getStatus();   
    Get device status   
    - 0 INIT   
    - 1 CONNECTING : AP mode,Trying to connect to AP   
    - 2 CLIENT : Client mode,connected to AP   
    - 3 AP : Soft AP mode   
    - 4 TIMEOUT : Config portal timeout  
- rebootToApMode();
Reboot esp8266 and go to config mode.This method is not a member of WiFiMan class and can be called anywhere even when WiFiMan is out of scoop.

### Global controls
Global control functions are not member of WiFiMan class, and can be called anywhere in sketch.
- bool reboot()   
    Reboot ESP8266.
- bool rebootToApMode()   
    Reboot ESP8266 and go straight to Config mode without trying to auto-connect using saved setting.
- bool clear()   
    Clear all saved setting and reboot ESP8266.

### Serial control
ESP8266 can be controlled by Serial command when in Config mode
#### Available commands
- #$> reboot   
    Reboot ESP8266.
- #$> config   
    Reboot ESP8266 and go straight to Config mode without trying to auto-connect using saved setting.
- #$> clear   
    Clear all saved setting and reboot ESP8266.
#### Serial control in client mode
ESP8266 can be controlled by Serial command when in Client mode(connect to AP) by define SerialControl object and call <SerialControlObjectName>.handleSerial(); in loop function.

## Q/A
### How to reconfig esp8266 after connected to AP?
There are 2 way to reconfig ESP8266 after connected to Access Point.Use rebootToApMode() or .forceApMode().
- <WiFiManClassName>.forceApMode()
This method force WiFiMan to skip auto-connect and go straight to Config mode.forceApMode() must be called before .start() called.
- rebootToApMode()
Reboot esp8266 and go to config mode.This method is not a member of WiFiMan class and can be called anywhere even when WiFiMan is out of scoop.
Caution : rebootToApMode use ESP.restart() to reboot the device . ESP.restart() may cause ESP8266 to crash at the first restart after serial flashing.For more information , please check [ESP8266 Issues](https://github.com/esp8266/Arduino/issues/1722)   
    
### What are #$<>! characters in Serial output messenger mean?
- #>> (debug) Funtion has been called.   
- #<< (debug) End of function.   
- #__ (debug) Debug output.   
- #>< (debug) Funtion without debug output has been called.   
- #$> (serial-control) Execute serial command. Usage : "#$> <command>" , Ex "#$> reboot".   
- #$< (serial-control) Result when execute serial command.   