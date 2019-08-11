# WiFiMan + ESP8266OTA
Wifiman is Wifi manager for ESP8266 with customizable web UI.   
ESP8266 is a modified version of ESP8266HTTPUpdateServer customizable web UI.   

## Change logs
### v1.4.1
- Added void setExtFunc(void (*f)(void)) to execute custom action during config-mode.   
- Added simple example for SonOff Basic.   
### v1.4.0
- Migrating from  ArduinoJson 5 to  ArduinoJson 6
- Added void setConnectDelay(unsigned int delayms) : Set delay between connects attempts.
### v1.3.2
- Added void setIndicatorLedPin(int pinNumber,bool onState) function.
    Set simple led indicator. The led will stay on when the esp trying to connect to the network and blink when the esp in ap(config) mode.
- Added bool activeState argument for funtion setConfigPin.
### v1.3.1
- Added setConfigPin(int pinNumber,bool activeState) function.
Set auto-connect interrupt pin. Hold this pin in active state for more than 500ms will skip auto-connect process(only works when the device trying to connect to AP using saved config). This function must be called before .start()
- Added bool activeState argument for funtion setConfigPin.   
### v1.3.0
- Removed serial control liblary and it's functions.
- Optimized memory usage.
### v1.2.0
- Added "Custom config" function.
### v1.1.0
- Merge ESP8266OTA with WiFiMan   
If you have the old ESP8266OTA library installed, please remove it to avoid conflig with new WiFiMan library.   
- Rework with Web UI for more customizable and better memory usage.   
Web UI now can be customized with Theme.h file.   
Some sample of Theme.h are available in themes folder.

## How to install 
- Method 1 : Install from Arduino IDE library manager 
- Method 2 : Manual download/clone this repo and put in arduino library folder

## Require library
- <a href="https://github.com/bblanchon/ArduinoJson">bblanchon's ArduinoJson</a>

## About password setting
- Soft AP password : Password of ESP8266 when in AP mode .Can be set with setApPasswd("yourPassword").If default password is not set, AP will fireup without password.
- Master password : password to access HTTP config portal.This password must be change after first login. Default master username and password can be set with setMasterUsername("yourUsername") and setDefaultMasterPasswd("your-password").If default username and password are not set , their value will be "admin" and "password".
*To keep old password ,leave "New password" and "Confirm password" fields empty(config page).

## API
### Constructor 
- WiFiMan();   
    Create default WiFiMan object without authentication. Authentication can be set with WiFiMan(bool authentication); or enable using WiFiMan.setAuthentication(true).   
- WiFiMan(bool authentication);   



### Config portal
- void setAuthentication(bool enable);   
    Enable authentication for http access.
- void setWebUi(String title,String banner,String build,String branch,String deviceInfo,String footer);   
    Set web UI of config portal.
- void setWebUi(String title,String banner,String build,String branch,String footer);   
    Set web UI of config portal, deviceInfo field will be automatically filled with ESP8266 ChipID.
- void setHelpInfo(String helpInfo);   
    Set content of "/help" page.
- void setApConfig(IPAddress ip,IPAddress gateway,IPAddress subnet);   
    Set Soft AP IP,Gateway,Subnet.If not configured , the default value is 192.168.1.1 192.168.1.1 255.255.255.0 .
- void setMaxConnectAttempt(int connectAttempt);   
    Limit the max connect attemp to other AP in client mode.Default is 36.
- void setConfigTimeout(int timeout);   
    Set timeout limit of config portal in minute.Default is 15 minutes.
- void setApName(String apName);  
    Set soft AP SSID.Default is "esp8266-id[ChipID]".
- void setApPasswd(String passwd);   
    Set soft AP password.
- void setMasterUsername(String username);   
    Set config portal username.
- void setDefaultMasterPasswd(String passwd);   
    Set config portal default password.
- void addCustomArg(String label,String name,String length,String type,String placeholder,String addition);   
    Add custom config parameter to config page.
- void disableMqttConfig()   
    Disable MQTT configuration in web UI.
- void setConnectDelay(unsigned int delayms)   
    set delay between connects attempts. The default value is 500ms.
  
### Get config parameters
- String getWifiSsid();   
    Get AP SSID
- String getWifiPasswd();   
    Get AP password
- IPAddress getSoftApIp();   
    Get soft AP ip 
- String getMqttServerAddr();   
    Get mqtt server address
- String getMqttServerPasswd();   
    Get mqtt server password
- String getMqttUsername();   
    Get mqtt server username
- String getMqttId(); and String getDeviceId();   
    Get mqtt id or device id.Lis library use device id as mqtt id.
- String getMqttSub();   
    Get mqtt sub topic
- String getMqttPub();   
    Get mqtt pub  topic 
- int getMqttPort();   
    Get mqtt port
- IPAddress getIp();   
    Get ip in client mode
- String getDnsName();   
    Get mDNS name 
- String getMacAddr();   
    Get device mac address
- bool getConfig(Config *conf);   
    Get all config parameters. Return true if the config is valid(success connected to ap)
- bool getCustomConfig(CustomConfig *customConf);   
    Get custom config parameters. Return true if success, false if there is no custom config or cannot read customConfig file.

### Controls

- void start();
    Start WiFiMan , all config APIs must be called before this function.
- bool deleteConfig();
    Delete saved config file (config.json).This function must be called before call start().
- void forceApMode();
    Force device into Soft Access Point mode without trying to connect to saved config.
- void setConfigPin(int pinNumber,bool activeState);
    Set auto-connect interrupt pin. Hold this pin in active state for more than 500ms will skip auto-connect process(only works when the device trying to connect to AP using saved config). This function must be called before .start()
- void setIndicatorLedPin(int pinNumber,bool onState);
    Set led indicator. The led will stay on when the esp trying to connect to the network and blink when the esp in ap(config) mode. This pin can be re-use later.
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


### Global controls
Global control functions are not member of WiFiMan class, and can be called anywhere in sketch.
- void reboot()   
    Reboot ESP8266.
- bool rebootToApMode()   
    Reboot ESP8266 and go straight to Config mode without trying to auto-connect using saved setting.
- bool clear()   
    Clear all saved setting and reboot ESP8266.

### Theming
- UI texts can be set with setWebUi(...).
- UI color and backdround can be edit in Theme.h file.


## Q/A
### How to enable debug
Enable Debug port in Arduino IDE (Tools > Debug port)

### How to reconfig esp8266 after connected to AP?
There are 4 way to reconfig ESP8266 after connected to Access Point.
- .deleteConfig()   
Delete saved config will force esp8266 into config mode. This function must be called before .start().
- .forceApMode()   
Force esp8266 into config mode. This function is same as deleteConfig(), but will not delete saved config. This function must be called before .start().
- .setConfigPin(int pinNumber,bool activeState)   
Set auto-connect interrupt pin. Hold this pin in active state for more than 500ms will skip auto-connect process(only works when the device trying to connect to AP using saved config). This function must be called before .start()
- rebootToApMode()   
Reboot esp8266 and go to config mode.This method is not a member of WiFiMan class and can be called anywhere even when WiFiMan is out of scoop.
Caution : rebootToApMode use ESP.restart() to reboot the device . ESP.restart() may cause ESP8266 to crash at the first restart after serial flashing.For more information , please check [ESP8266 Issues](https://github.com/esp8266/Arduino/issues/1722)   
    
### What are #$<>_ characters in Serial output messenger mean?
- #>> (debug) Function has been called.   
- #<< (debug) End of function.   
- #__ (debug) Debug output.   
- #>< (debug) Function without debug output has been called.   
