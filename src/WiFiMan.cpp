#include "WiFiMan.h"

WiFiMan::WiFiMan()
{
    WiFiMan(false,false,false);
}

WiFiMan::WiFiMan(bool Authentication)
{
    WiFiMan(Authentication,false,false);
}

WiFiMan::WiFiMan(bool authentication,bool serialControl)
{
    WiFiMan(authentication,serialControl,false);
}

WiFiMan::WiFiMan(bool authentication,bool serialControl,bool debug)
{
    DEBUG=debug;
    AUTHENTICATION=authentication;
    SERIALCONTROL=serialControl;
    FORCE_AP = false;
    _mode = MODE::INIT;

    //just incase , disconnect from current connected AP
    WiFi.disconnect();

    //init serial
    Serial.begin(115200);

    serialBuffer = "";

    printDebug("\n",true);
    printDebug("start",true);
    printDebug("Booting sketch...",false);

    

    //default value 
    _wifiSsid = "";
    _wifiPasswd = "";
    _mqttAddr = "";
    _mqttPort = "";
    _mqttUsername = "";
    _mqttPasswd = "";
    _mqttSub = "";
    _mqttPub = "";
    String _mqttId = "";
    String _masterPasswd = "";

    //web ui 
    _title = "ESP8266fw";
    _banner = "ESP8266 wf";
    _build = "1.0beta";
    _branch = "dev";
    _deviceInfo = "ChipID : " +  String(ESP.getChipId());
    _footer = "ChipCE";
    _helpInfo = "Please visit homepage for more information!";

    //server configs
    _maxConnectAttempt = 36;
    _configTimeout = 15;
    _DNS_PORT = 53;
    _apName = "esp8266-id";
    _apPasswd = "";
    _httpUsername = "admin";
    _defaultHttpPasswd = "password";
    _apIp = IPAddress(192, 168, 1, 1);
    _apGateway = IPAddress(192, 168, 1, 1);
    _apSubnet = IPAddress(255, 255, 255, 0);

    _defaultMqttId = "esp8266";
    _action = 0;
}


void WiFiMan::setAuthentication(bool enable)
{
    AUTHENTICATION=enable;
}

bool WiFiMan::deleteConfig()
{
    printDebug("deleteConfig",true);
    if(SPIFFS.begin())
    {
        if(!SPIFFS.exists("/config.json"))
        {
            printDebug("config.json not exists",false);
            return true;
        }

        if(SPIFFS.remove("/config.json"))
        {
            printDebug("Deleted config.json",false);
            return true;
        }
        else
        {
            printDebug("Cannot eleted config.json",false);
            return false;
        }
    }
    else
    {
        printDebug("Failed to mount FS",false);
        return false;
    }
}

void WiFiMan::setDebug(bool enable)
{
    DEBUG=enable;
}


void WiFiMan::forceApMode()
{
    FORCE_AP=true;
}



void WiFiMan::setSerialControl(bool enable)
{
    SERIALCONTROL = enable;
}

void WiFiMan::printDebug(String msg,bool primary)
{
    String debugMsg;
    if(primary)
        debugMsg= "# Debug @";
    else
        debugMsg= "# \t";

    debugMsg += msg;
    if(DEBUG)
        Serial.println(debugMsg);

}

void WiFiMan::printDebug(JsonObject& json)
{
    if(DEBUG)
    {
        if(json.success())
        {
            printDebug("printDebug - json",true);
            String str;
            json.printTo(str);
            str.replace("{","");
            str.replace(",","\n#\t");
            str.replace("}","");
            printDebug(str,false);
        }
        else
        {
            printDebug("Cannot parse json",false);
            return;
        }
    }
}

bool WiFiMan::readConfig()
{
    printDebug("readConfig",true);
    if(SPIFFS.begin())
    {
        if (SPIFFS.exists("/config.json")) 
        {
            printDebug("Reading config.json",false);
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) 
            {
                size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buf.get());
                if(json.success())
                {
                    printDebug(json);
                    //parse 
                    _wifiSsid = json["wifiSsid"].as<String>();
                    _wifiPasswd = json["wifiPasswd"].as<String>();
                    _mqttAddr = json["mqttAddr"].as<String>();
                    _mqttPort = json["mqttPort"].as<String>();
                    _mqttUsername = json["mqttUsername"].as<String>();
                    _mqttPasswd = json["mqttPasswd"].as<String>();
                    _mqttSub = json["mqttSub"].as<String>();
                    _mqttPub = json["mqttPub"].as<String>();
                    _mqttId = json["mqttId"].as<String>();
                    _masterPasswd = json["masterPasswd"].as<String>();

                    configFile.close();
                    SPIFFS.end();
                    return true;
                }
                else
                {
                    printDebug("Cannot parse json. Wrong format ?",false);
                    printDebug("Close config.js",false);
                    configFile.close();
                    printDebug("Unmount FS",false);
                    SPIFFS.end();

                    printDebug("Trying to delete config.js",false);
                    deleteConfig();
                    printDebug("write template for config.js",false);
                    writeConfig(_wifiSsid,_wifiPasswd,_mqttAddr,_mqttPort,_mqttUsername,_mqttPasswd,_mqttSub,_mqttPub,_mqttId,_masterPasswd);
                    return false;
                }
            }
            else
            {
                printDebug("Cannot open config.json",false);
                printDebug("Unmount FS",false);
                SPIFFS.end();
                return false;
            }
        }
        else
        {
            printDebug("config.json not exists",false);
            printDebug("Unmount FS",false);
            SPIFFS.end();
            printDebug("write template for config.js",false);
            writeConfig(_wifiSsid,_wifiPasswd,_mqttAddr,_mqttPort,_mqttUsername,_mqttPasswd,_mqttSub,_mqttPub,_mqttId,_masterPasswd);
            return false;
        }
    }
    else
    {
        printDebug("Failed to mount FS",false);
        return false;
    }
}

bool WiFiMan::writeConfig(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd)
{
    printDebug("writeConfig",true);
    printDebug("Updating config data",false);
    _wifiSsid = wifiSsid;
    _wifiPasswd = wifiPasswd;
    _mqttAddr = mqttAddr;
    _mqttPort = mqttPort;
    _mqttUsername = mqttUsername;
    _mqttPasswd = mqttPasswd;
    _mqttSub = mqttSub;
    _mqttPub = mqttPub;
    _mqttId = mqttId;
    _masterPasswd = masterPasswd;

    printDebug("Writing config.json",false);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["wifiSsid"] = _wifiSsid;
    json["wifiPasswd"] = _wifiPasswd;
    json["mqttAddr"] = _mqttAddr;
    json["mqttPort"] = _mqttPort;
    json["mqttUsername"] = _mqttUsername;
    json["mqttPasswd"] = _mqttPasswd;
    json["mqttSub"] = _mqttSub;
    json["mqttPub"] = _mqttPub;
    json["mqttId"] = _mqttId;
    json["masterPasswd"] = _masterPasswd;
    
    
    if(SPIFFS.begin())
    {
        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) 
        {
            printDebug("Failed to open config file for writing",false);
            printDebug("Unmount FS",false);
            SPIFFS.end();
            return false;
        }

        json.printTo(configFile);
        printDebug("Save successed!",false);
        configFile.close();
        printDebug("Unmount FS",false);
        SPIFFS.end();
        return true;
    }
    else
    {
        printDebug("Failed to mount FS",false);
        return false;
    }
}

void WiFiMan::start()
{
    printDebug("start",true);
    //read config file
    if(readConfig() && !FORCE_AP)
    {
        //success , try to connect to AP
        if(clientMode())
        {
            //connect success.
            //do nothing, leave the work for main program
            return;
        }
        else
        {
            //connect failed , fire up softAP
            WiFi.disconnect();
            apMode();
        }
    }
    else
    {
        //failed to read config file or force AP mode is enabled then fire up softAP
        FORCE_AP = false;
        apMode();
    }
}

bool WiFiMan::clientMode()
{
    printDebug("clientMode",true);
    WiFi.mode(WIFI_STA);
    
    _mode=MODE::CLIENT;

    //auto connect
    if(!validConfig())
    {
        printDebug("Invalid config. Exit client mode.",false);
        return false;
    }
    return connect(_wifiSsid,_wifiPasswd);
}

bool WiFiMan::apMode()
{
    printDebug("apMode",true);
    _mode = MODE::AP;

    //setup web server
    setupWebServer();

    printDebug("Set wifi mode : WIFI_AP_STA",false);
    WiFi.mode(WIFI_AP_STA);
    dnsServer.reset(new DNSServer());

    //setup soft ap
    WiFi.softAPConfig(_apIp, _apGateway, _apSubnet);
    String apSSID = _apName + String( ESP.getChipId());

    if(_apPasswd == "")
        WiFi.softAP(apSSID.c_str());
    else
        WiFi.softAP(apSSID.c_str(),_apPasswd.c_str());


    printDebug("SoftAP SIID : "+apSSID,false);
    printDebug("Server IP : "+_apIp.toString(),false);

    delay(500);

    printDebug("Start DNS server",false);
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(_DNS_PORT, "*", WiFi.softAPIP());
    
    //startweb server
    printDebug("Start web server",false);
    webServer->begin();

    int startConfigTime = millis();
    printDebug("Started config potal",false);
    while(millis()-startConfigTime < _configTimeout*60000)
    {
        if(_action)
        {
            //check for action handle
            switch(_action)
            {
                case ACTION_TYPE::CONFIG_SAVED:
                    //config saved
                    connect(_wifiSsid,_wifiPasswd);
                break;

                case ACTION_TYPE::CLEAR_CONFIG:
                    //clear
                    deleteConfig();
                    ESP.restart();
                break;

                case ACTION_TYPE::SYS_RESET:
                    //reset
                    ESP.restart();
                break;

                default:
                break;
            }
            _action = ACTION_TYPE::NONE;
        }

        // if connected , break portal loop
        if(WiFi.status() == WL_CONNECTED)
            break;

        //handle  icomming serial
        if(SERIALCONTROL)
            handleSerial();

        //handle web request
        dnsServer->processNextRequest();
        webServer->handleClient();
        //yield();
    }

    WiFi.softAPdisconnect(true);
    stopWebServer();
    stopDnsServer();
    if(!isConnected())
    {
        printDebug("Config potal timeout",false);
        _mode = MODE::TIMEOUT;
        return false;
    }
    else
        return true;
}

void WiFiMan::handleNotFound()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    printDebug("handleNotFound",true);
    String page = FPSTR(HTTP_HEADERRELOAD);
    page =page + FPSTR(HTTP_INFO);
    page=page + FPSTR(HTTP_FOOTER);
    page.replace("{info}","Error 404 : Page not found </br>Redirect to root");

    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    webServer->send ( 404, "text/html", page );
}

void WiFiMan::handleRoot()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();
    printDebug("handleRoot",true);
    String page = FPSTR(HTTP_HEADER);
    page=page + FPSTR(HTTP_INDEX);
    page=page + FPSTR(HTTP_FOOTER);
    
    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    webServer->send ( 200, "text/html", page );
}



void WiFiMan::handleConfig()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    printDebug("handleConfig",true);
    String page = FPSTR(HTTP_HEADER);
    page=page + FPSTR(HTTP_CONFIG);
    page=page + FPSTR(HTTP_FOOTER);
    
    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    webServer->send ( 200, "text/html", page );
}

void WiFiMan::handleClearSetting()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
        return webServer->requestAuthentication();

    printDebug("handleClearSetting",true);
    String page = FPSTR(HTTP_HEADERRELOAD);
    page =page + FPSTR(HTTP_INFO);
    page=page + FPSTR(HTTP_FOOTER);
    page.replace("{info}","<br/>All setting cleared<br/>Device will restart after 15 seconds.");

    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    page.replace("{url}","/");
    page.replace("{delay}","15");
    webServer->send ( 200, "text/html", page );
    webServer->client().stop();
    _action = ACTION_TYPE::CLEAR_CONFIG;
}

void WiFiMan::handleReset()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    printDebug("handleReset",true);
    String page = FPSTR(HTTP_HEADERRELOAD);
    page =page + FPSTR(HTTP_INFO);
    page=page + FPSTR(HTTP_FOOTER);
    page.replace("{info}","<br/>Device will restart after 15 seconds.");

    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    page.replace("{url}","/");
    page.replace("{delay}","15");
    webServer->send ( 200, "text/html", page );
    _action = ACTION_TYPE::SYS_RESET;
}

void WiFiMan::handleSave()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    printDebug("handleSave",true);

    String wifiSsid = webServer->arg("wifiSsid").c_str();
    String wifiPasswd = webServer->arg("wifiPasswd").c_str();
    String mqttAddr = webServer->arg("mqttAddr").c_str();
    String mqttPort = webServer->arg("mqttPort").c_str();
    String mqttUsername = webServer->arg("mqttUsername").c_str();
    String mqttPasswd = webServer->arg("mqttPasswd").c_str();
    String mqttSub = webServer->arg("mqttSub").c_str();
    String mqttPub = webServer->arg("mqttPub").c_str();
    String mqttId = webServer->arg("mqttId").c_str();
    String masterPasswd = webServer->arg("masterPasswd").c_str();
    String confirmPasswd = webServer->arg("confirmPasswd").c_str();

    printDebug("wifiSsid : " + wifiSsid,false);
    printDebug("wifiPasswd : " + wifiPasswd,false);
    printDebug("mqttAddr : " + mqttAddr,false);
    printDebug("mqttPort : " + mqttPort,false);
    printDebug("mqttUsername : " + mqttUsername,false);
    printDebug("mqttPasswd : " + mqttPasswd,false);
    printDebug("mqttSub : " + mqttSub,false);
    printDebug("mqttPub : " + mqttPub,false);
    printDebug("mqttId : " + mqttId,false);
    printDebug("masterPasswd : " + masterPasswd,false);
    printDebug("confirmPasswd : " + confirmPasswd,false);

    if(mqttId == "")
    {
        mqttId = _defaultMqttId + "-" + String(ESP.getChipId());
        printDebug("Use default MQTT id : " + mqttId,false);
    }


    String errorMsg = checkInput(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd,confirmPasswd);

    if( errorMsg == "")
    {
        //input seem good, save setting
        writeConfig(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd);
        //update password for OTA updater 
        otaUpdater->updatePasswd(masterPasswd.c_str());

        String page = FPSTR(HTTP_HEADERRELOAD);
        page =page + FPSTR(HTTP_INFO);
        page=page + FPSTR(HTTP_FOOTER);
        page.replace("{info}","<br/>Config saved!");

        page.replace("{title}",_title);
        page.replace("{banner}",_banner);
        page.replace("{build}",_build);
        page.replace("{branch}",_branch);
        page.replace("{deviceInfo}",_deviceInfo);
        page.replace("{footer}",_footer);
        page.replace("{url}","/");
        page.replace("{delay}","15");
        webServer->send ( 200, "text/html", page );
        

        _action = ACTION_TYPE::CONFIG_SAVED;
    }
    else
    {
        //something wrong , display error msg
        printDebug("Invalid input",false);

        String page = FPSTR(HTTP_HEADER);
        page =page + FPSTR(HTTP_EDIT);
        page=page + FPSTR(HTTP_FOOTER);
        page.replace("{info}",errorMsg);

        page.replace("{title}",_title);
        page.replace("{banner}",_banner);
        page.replace("{build}",_build);
        page.replace("{branch}",_branch);
        page.replace("{deviceInfo}",_deviceInfo);
        page.replace("{footer}",_footer);
        webServer->send ( 200, "text/html", page );
        return;
    }
}

void WiFiMan::handlePortal()
{
    //do not call requestAuthentication , or the captive portal will not showup in authentication mode
    if(AUTHENTICATION)
    {
        //send portal page , display device ip address
        printDebug("handlePortal",true);
        String page = FPSTR(HTTP_HEADER);
        page =page + FPSTR(HTTP_PORTAL);
        page=page + FPSTR(HTTP_FOOTER);

        page.replace("{title}",_title);
        page.replace("{banner}",_banner);
        page.replace("{build}",_build);
        page.replace("{branch}",_branch);
        page.replace("{deviceInfo}",_deviceInfo);
        page.replace("{footer}",_footer);
        page.replace("{ip}",_apIp.toString());

        webServer->send ( 200, "text/html", page );
    }
    else
        return handleRoot();
}

void WiFiMan::handleHelp()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    printDebug("handleSave",true);
    String page = FPSTR(HTTP_HEADER);
    page =page + FPSTR(HTTP_HELP);
    page=page + FPSTR(HTTP_FOOTER);

    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{helpInfo}",_helpInfo);
    page.replace("{footer}",_footer);
    page.replace("{url}","/");
    page.replace("{delay}","15");
    webServer->send ( 200, "text/html", page );
}



void WiFiMan::setupWebServer()
{
    printDebug("setupWebServer",true);
    //setup web server
    webServer.reset(new ESP8266WebServer(80));

    //setup ota updater
    otaUpdater.reset(new ESP8266OTA());
    otaUpdater->setUpdaterUi(_title,_banner,_build,_branch,_deviceInfo,_footer);
    if(AUTHENTICATION)
    {
        otaUpdater->setup(webServer.get(),_httpUsername.c_str(),getApPassword());
        printDebug("start otaUpdater : "+_httpUsername+"@"+getApPassword(),false);
    }
    else
    {
        otaUpdater->setup(webServer.get());
        printDebug("start otaUpdater",false);
    }

    //setup web server handles
    webServer->on("/", std::bind(&WiFiMan::handleRoot, this));
    webServer->on("/config", std::bind(&WiFiMan::handleConfig, this));
    webServer->on("/clearsetting", std::bind(&WiFiMan::handleClearSetting, this));
    webServer->on("/reset", std::bind(&WiFiMan::handleReset, this));
    webServer->on("/save", std::bind(&WiFiMan::handleSave, this));
    webServer->onNotFound (std::bind(&WiFiMan::handleNotFound, this));
    webServer->on("/fwlink", std::bind(&WiFiMan::handlePortal, this));
    webServer->on("/generate_204", std::bind(&WiFiMan::handlePortal, this));
    webServer->on("/help", std::bind(&WiFiMan::handleHelp, this));
    //webServer->on("/update...); will be automatically handled by otaUpdater
}

void WiFiMan::setWebUi(String title,String banner,String build,String branch,String deviceInfo,String footer)
{
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _deviceInfo = deviceInfo;
    _footer = footer;
}

void WiFiMan::setWebUi(String title,String banner,String build,String branch,String footer)
{
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _footer = footer;
}

void WiFiMan::stopWebServer()
{
    printDebug("stopWebServer",true);
    printDebug("Stop web server",false);
    webServer->stop();
    webServer.reset();
}

void WiFiMan::stopDnsServer()
{
    printDebug("stopDnsServer",true);
    printDebug("stop Dns server",false);
    dnsServer->stop();
    dnsServer.reset();
}

const char* WiFiMan::getApPassword()
{
    printDebug("getApPassword",true);
    if(_masterPasswd != "")
    {
        printDebug("Return password : "+_masterPasswd,false);
        return _masterPasswd.c_str();
    }
    printDebug("Return password : "+_defaultHttpPasswd,false);
    return _defaultHttpPasswd.c_str();
}

String WiFiMan::checkInput(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd,String confirmPasswd)
{
    String errorMsg = "";
    if(wifiSsid == "")
        errorMsg += "Invalid SSID<br/>"; 
    //skip check for wifiPasswd (unsecure ap)
    if(mqttAddr == "")
        errorMsg += "Invalid MQTT address<br/>"; 
    if(mqttPort == "")
        errorMsg += "Invalid MQTT port<br/>"; 
    if((mqttUsername != "" && mqttPasswd == "") || (mqttUsername == "" && mqttPasswd != ""))
        errorMsg += "Invalid MQTT username or password<br/>"; 
    //skip check for mqtt id , id not set , use esp8266 chipID instead

    if(masterPasswd == "" || masterPasswd == _defaultHttpPasswd)
        errorMsg += "Invalid Password!<br/>"; 
    if(masterPasswd != confirmPasswd)
        errorMsg += "Confirm password not matched<br/>"; 
    return errorMsg;
}

bool WiFiMan::connect(String wifiSsid,String wifiPasswd)
{
    int oldMode = _mode;
    _mode = MODE::CONNECTING;

    printDebug("connect",true);
    WiFi.disconnect(); 
    //check for wifi connection
    if(wifiPasswd == "")
        WiFi.begin(wifiSsid.c_str());
    else
        WiFi.begin(wifiSsid.c_str(),wifiPasswd.c_str());
    
    printDebug("Connecting to AP : " + wifiSsid + "\tPassword :" + wifiPasswd ,false);
    for (int tryCount = 0;(WiFi.status() != WL_CONNECTED) && (tryCount < _maxConnectAttempt);tryCount++) 
    {
        printDebug(".",false);
        delay(500);
    }

    if(WiFi.status() == WL_CONNECTED)
    {
        printDebug("Connected to AP",false);
        printDebug("IP address : " + WiFi.localIP().toString(),false);
        _mode = MODE::CLIENT;
        return true;
    }
    else
    {
        printDebug("Cannot connect to AP. Exit client mode.",false);
        //return to the last mode
        _mode == oldMode;
        return false;
    }
    return false;
}

bool WiFiMan::validConfig()
{
    printDebug("validConfig",true);
    if(_wifiSsid == "")
        return false;
    if(_mqttAddr == "")
        return false;
    if(_mqttPort == "")
        return false;
    if(_mqttPort == "")
        return false;
    if(_masterPasswd == "")
        return false;
    printDebug("Config OK!",false);
    return true;
}


String WiFiMan::getWifiSsid()
{
    return _wifiSsid;
}
//get wifi password
String WiFiMan::getWifiPasswd()
{
    return _wifiPasswd;
}
//get mqtt server address
String WiFiMan::getMqttServerAddr() 
{ 
    return _mqttAddr; 
}
//get mqtt server password
String WiFiMan::getMqttServerPasswd() 
{ 
    return _mqttPasswd; 
}
//get mqtt server username
String WiFiMan::getMqttUsername() 
{ 
    return _mqttUsername; 
}
//get mqtt id
String WiFiMan::getMqttId() 
{ 
    return _mqttId;
}
//get mqtt sub topic
String WiFiMan::getMqttSub() 
{ 
    return _mqttSub; 
}
//get mqtt pub  topic
String WiFiMan::getMqttPub() 
{ 
    return _mqttPub; 
}
//get mqtt port
int WiFiMan::getMqttPort() 
{ 
    return atoi(_mqttPort.c_str()); 
}
//get soft AP ip
IPAddress WiFiMan::getSoftApIp() 
{ 
    return WiFi.softAPIP(); 
}
//get ip in client mode
IPAddress WiFiMan::getIp() 
{ 
    return WiFi.localIP(); 
}
//get dns name
String WiFiMan::getDnsName() 
{ 
    return (_mqttId + ".local"); 
}
//get device mac address
String WiFiMan::getMacAddr() 
{ 
    return WiFi.macAddress().c_str(); 
}

//set ap ip/subnet/gateway
void WiFiMan::setApConfig(IPAddress ip, IPAddress gateway, IPAddress subnet)
{
    _apIp = ip;
    _apGateway = gateway;
    _apSubnet = subnet;
}

//set max connect attempt
void WiFiMan::setMaxConnectAttempt(int connectAttempt)
{
    _maxConnectAttempt = connectAttempt;
}
//set timeout of AP mode (min), server will turnoff after timeout
void WiFiMan::setConfigTimeout(int timeout)
{
    _configTimeout = timeout;
}
//set default ap SSID .in ap mode, SSID will be <apName>+<chipID>
void WiFiMan::setApName(String apName)
{
    _apName = apName;
}
//set softAP password
void WiFiMan::setApPasswd(String passwd)
{
    _apPasswd = passwd;
}
//set password use in the first time login.This can be changed in config menu
void WiFiMan::setHttpPassword(String passwd)
{
    _defaultHttpPasswd = passwd;
}
//set username to login (this cant be change later)
void WiFiMan::setHttpUsername(String username)
{
    _httpUsername = username;
}


void WiFiMan::disconnect()
{
    WiFi.disconnect(); 
}


void WiFiMan::handleSerial()
{
    //join serial characters to line
    while (Serial.available() > 0)
    {
        char recievedChar = Serial.read();
        serialBuffer += recievedChar; 

        // Process message when new line character is recieved
        if ((recievedChar == '\n') || (recievedChar == '\r'))
        {
            printDebug("handleSerial",true);
            printDebug(serialBuffer,false);

            //check for valid command ?

            //send to command handler
            handleSerial(serialBuffer);
            // Clear recieved buffer
            serialBuffer = ""; 
        }
    }
}


void WiFiMan::handleSerial(String msg)
{
    //command in json format
    /*
    {
        "command":"...", //canbe status,clear,save or reboot
        "arg":{"wifiSsid":"Wifi-AP",...."confirmPasswd":"******"}
    }
    */
    printDebug("handleSerial-command",true);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(msg);
    if(json.success())
    {
        String command = json["command"].as<String>();
        String args = json["args"].as<String>();
        printDebug("Command: " + command,false);
        printDebug("Args: " + args,false);
        
        if(command == "status")
        {
            printDebug("handleSerial-status", false);
            //send system status back to application
            handleSerialStatus();
            return ;
        }
        else if(command == "reboot")
        {
            printDebug("handleSerial-reboot", false);
            Serial.println("$ Module will restart.");
            //reboot the device
            ESP.restart();
            return ;
        }
        else if(command == "clear")
        {
            printDebug("handleSerial-clear", false);
            Serial.println("$ Setting was deleted. Module will restart.");
            //clear all setting and reboot
            deleteConfig();
            ESP.restart();
            return ;
        }
        else if(command == "save")
        {
            printDebug("handleSerial-save", false);
            //try to save setting and restart if success
            if(handleSerialSave(args))
            {
                Serial.println("$ Setting saved. Module will restart.");
                ESP.restart();
            }
            else
                Serial.println("$ Error : Cannot save setting.");
            return ;
        }
        else
        {
            printDebug("handleSerial-invalid", false);
            return;
        }
    }
    else
    {
        printDebug("Invalid command format.",false);
    }
}

bool WiFiMan::handleSerialSave(String args)
{
    printDebug("handleSerialSave",true);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(args);
    if(json.success())
    {
        String wifiSsid = json["wifiSsid"].as<String>();
        String wifiPasswd = json["wifiPasswd"].as<String>();
        String mqttAddr = json["mqttAddr"].as<String>();
        String mqttPort = json["mqttPort"].as<String>();
        String mqttUsername = json["mqttUsername"].as<String>();
        String mqttPasswd = json["mqttPasswd"].as<String>();
        String mqttSub = json["mqttSub"].as<String>();
        String mqttPub = json["mqttPub"].as<String>();
        String mqttId = json["mqttId"].as<String>();
        String masterPasswd = json["masterPasswd"].as<String>();
        String confirmPasswd = json["confirmPasswd"].as<String>();

        printDebug("wifiSsid : " + wifiSsid,false);
        printDebug("wifiPasswd : " + wifiPasswd,false);
        printDebug("mqttAddr : " + mqttAddr,false);
        printDebug("mqttPort : " + mqttPort,false);
        printDebug("mqttUsername : " + mqttUsername,false);
        printDebug("mqttPasswd : " + mqttPasswd,false);
        printDebug("mqttSub : " + mqttSub,false);
        printDebug("mqttPub : " + mqttPub,false);
        printDebug("mqttId : " + mqttId,false);
        printDebug("masterPasswd : " + masterPasswd,false);
        printDebug("confirmPasswd : " + confirmPasswd,false);

        if(masterPasswd!=_masterPasswd)
        {
            printDebug("Master password not matched.",false);
            Serial.println("$ Cannot save config.Master password not matched.");
            return false;
        }

        if(mqttId == "")
        {
            mqttId = _defaultMqttId + "-" + String(ESP.getChipId());
            printDebug("Use default MQTT id : " + mqttId,false);
        }

        String errorMsg = checkInput(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd,confirmPasswd);

        if( errorMsg == "")
        {   
            //arg seem ok ,save it
            bool result = writeConfig(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd);
            if(result)
            {
                Serial.println("$ Config saved");
                return true;
            }
            else
            {   
                Serial.println("$ Cannot save config");
                return false;
            }
        }
        else
        {
            printDebug("invalid arg format",false);
            Serial.println("$ Cannot save config.Invalid args format.");
            return false;
        }
    }
    else
    {
        printDebug("invalid args format",false);
        return false;
    }
    return true;
}

void WiFiMan::handleSerialStatus()
{
    Serial.println("$******* IPMI status report *******");
    delay(10);
    Serial.println("$ Build: "+_build);
    delay(10);
    Serial.println("$ Branch: "+_branch);
    delay(10);
    Serial.println("$ Device Info: "+_deviceInfo);
    delay(10);
   
    switch(_mode)
    {
        case MODE::AP:
            Serial.println("$ Connection status : Config Gateway - SoftAP");
            delay(10);
            Serial.println("$ Gateway IP: "+_apIp.toString());
            delay(10);
            Serial.println("$**** End of IPMI status report ****");
            break;

        case MODE::CLIENT:
            Serial.println("$ Connection status : Connected");
            delay(10);
            Serial.println("$ Ip Address: "+WiFi.localIP().toString());
            delay(10);
            Serial.println("$ SSID: "+_wifiSsid);
            delay(10);
            Serial.println("$ MQTT server: "+_mqttAddr);
            delay(10);
            Serial.println("$ MQTT port: "+_mqttPort);
            delay(10);
            Serial.println("$ MQTT subscribe: "+_mqttSub);
            delay(10);
            Serial.println("$ MQTT publish: "+_mqttPub);
            delay(10);
            Serial.println("$ MQTT ID: "+_mqttId);
            delay(10);
            Serial.println("$**** End of IPMI status report ****");
            break;

        case MODE::CONNECTING:
            Serial.println("$ Connection status : Trying to connect to "+_wifiSsid);
            delay(10);
            Serial.println("$**** End of IPMI status report ****");
            break;

        case MODE::TIMEOUT:
            Serial.println("$ Connection status : Gateway timeout");
            delay(10);
            Serial.println("$**** End of IPMI status report ****");
            break;

        case MODE::INIT:
            Serial.println("$ Connection status : Error.");
            delay(10);
            Serial.println("$**** End of IPMI status report ****");
            break;
    }
}

bool WiFiMan::isConnected()
{
    if(WiFi.status() == WL_CONNECTED)
        return true;
    else
        return false;
}

int WiFiMan::getStatus()
{
    return _mode;
}

bool WiFiMan::setJsonConfig(String args)
{
    printDebug("setJsonConfig",true);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(args);
    if(json.success())
    {
        String wifiSsid = json["wifiSsid"].as<String>();
        String wifiPasswd = json["wifiPasswd"].as<String>();
        String mqttAddr = json["mqttAddr"].as<String>();
        String mqttPort = json["mqttPort"].as<String>();
        String mqttUsername = json["mqttUsername"].as<String>();
        String mqttPasswd = json["mqttPasswd"].as<String>();
        String mqttSub = json["mqttSub"].as<String>();
        String mqttPub = json["mqttPub"].as<String>();
        String mqttId = json["mqttId"].as<String>();
        String masterPasswd = json["masterPasswd"].as<String>();
        String confirmPasswd = json["confirmPasswd"].as<String>();

        printDebug("wifiSsid : " + wifiSsid,false);
        printDebug("wifiPasswd : " + wifiPasswd,false);
        printDebug("mqttAddr : " + mqttAddr,false);
        printDebug("mqttPort : " + mqttPort,false);
        printDebug("mqttUsername : " + mqttUsername,false);
        printDebug("mqttPasswd : " + mqttPasswd,false);
        printDebug("mqttSub : " + mqttSub,false);
        printDebug("mqttPub : " + mqttPub,false);
        printDebug("mqttId : " + mqttId,false);
        printDebug("masterPasswd : " + masterPasswd,false);
        printDebug("confirmPasswd : " + confirmPasswd,false);

        if(masterPasswd!=_masterPasswd)
        {
            printDebug("Master password not matched.",false);
            Serial.println("$ Cannot save config.Master password not matched.");
            return false;
        }

        if(mqttId == "")
        {
            mqttId = _defaultMqttId + "-" + String(ESP.getChipId());
            printDebug("Use default MQTT id : " + mqttId,false);
        }

        String errorMsg = checkInput(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd,confirmPasswd);

        if( errorMsg == "")
        {   
            //arg seem ok ,save it
            bool result = writeConfig(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd);
            if(result)
            {
                Serial.println("$ Config saved");
                return true;
            }
            else
            {   
                Serial.println("$ Cannot save config");
                return false;
            }
        }
        else
        {
            printDebug("invalid arg format",false);
            Serial.println("$ Cannot save config.Invalid args format.");
            return false;
        }
    }
    else
    {
        printDebug("invalid args format",false);
        return false;
    }
    return true;
}

void WiFiMan::setHelpInfo(String helpInfo)
{
    _helpInfo = helpInfo;
}