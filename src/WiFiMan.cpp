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
    WiFi.disconnect();
}


void WiFiMan::setAuthentication(bool enable)
{
    AUTHENTICATION=enable;
}

bool WiFiMan::deleteConfig()
{
    debugHelper.printFunctionCall("deleteConfig");
    if(SPIFFS.begin())
    {
        if(!SPIFFS.exists("/config.json"))
        {
            debugHelper.println("config.json is not exists");
            debugHelper.printLastMsg("deleteConfig-end");
            return true;
        }

        if(SPIFFS.remove("/config.json"))
        {
            debugHelper.println("Deleted config.json");
            debugHelper.printLastMsg("deleteConfig-end");
            return true;
        }
        else
        {
            debugHelper.println("Cannot delete config.json");
            debugHelper.printLastMsg("deleteConfig-end");
            return false;
        }
    }
    else
    {
        debugHelper.println("Failed to mount FS");
        debugHelper.printLastMsg("deleteConfig-end");
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


bool WiFiMan::readConfig()
{
    debugHelper.printFunctionCall("readConfig");
    if(SPIFFS.begin())
    {
        if (SPIFFS.exists("/config.json")) 
        {
            debugHelper.println("Reading config.json");
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
                    debugHelper.printJson(json);
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
                    debugHelper.printLastMsg("readConfig-end");
                    return true;
                }
                else
                {
                    debugHelper.println("Cannot parse json. Wrong format ?");
                    debugHelper.println("Close config.js");
                    configFile.close();
                    debugHelper.println("Unmount FS");
                    SPIFFS.end();

                    debugHelper.println("Trying to delete config.js");
                    deleteConfig();
                    debugHelper.println("write template for config.js");
                    writeConfig("","","","","","","","","","");
                    debugHelper.printLastMsg("readConfig-end");
                    return false;
                }
            }
            else
            {
                debugHelper.println("Cannot open config.json");
                debugHelper.println("Unmount FS");
                SPIFFS.end();
                debugHelper.printLastMsg("readConfig-end");
                return false;
            }
        }
        else
        {
            debugHelper.println("config.json not exists");
            debugHelper.println("Unmount FS");
            SPIFFS.end();
            debugHelper.println("write template for config.js");
            writeConfig("","","","","","","","","","");
            debugHelper.printLastMsg("readConfig-end");
            return false;
        }
    }
    else
    {
        debugHelper.println("Failed to mount FS");
        debugHelper.printLastMsg("readConfig-end");
        return false;
    }
}

bool WiFiMan::writeConfig(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd)
{
    debugHelper.printFunctionCall("writeConfig");
    debugHelper.println("Updating config data");
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

    debugHelper.println("Writing config.json");
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
            debugHelper.println("Failed to open config file for writing");
            debugHelper.println("Unmount FS");
            SPIFFS.end();
            debugHelper.printLastMsg("writeConfig-end");
            return false;
        }

        json.printTo(configFile);
        debugHelper.println("Save successed!");
        configFile.close();
        debugHelper.println("Unmount FS");
        SPIFFS.end();
        debugHelper.printLastMsg("writeConfig-end");
        return true;
    }
    else
    {
        debugHelper.println("Failed to mount FS");
        debugHelper.printLastMsg("writeConfig-end");
        return false;
    }
}

void WiFiMan::start()
{
    debugHelper = DebugHelper(DEBUG);
    debugHelper.printFunctionCall("start");
    serialController = SerialController(SERIALCONTROL);
    //get boot mode
    if(!FORCE_AP)
        FORCE_AP = getBootMode();
    //read config file
    if(readConfig() && !FORCE_AP)
    {
        debugHelper.println("Trying to connect to AP");
        //success , try to connect to AP
        if(clientMode())
        {
            //connect success.
            //do nothing, leave the work for main program
            debugHelper.println("Connected to AP");
            debugHelper.printLastMsg("start-end");
            return;
        }
        else
        {
            //connect failed , fire up softAP
            debugHelper.println("Cannot connected to AP");
            WiFi.disconnect();
            apMode();
            debugHelper.printLastMsg("start-end");
            return;
        }
    }
    else
    {
        //failed to read config file or force AP mode is enabled then fire up softAP
        debugHelper.println("Cannot read config.json or FORCE_AP is enabled");
        FORCE_AP = false;
        apMode();
        debugHelper.printLastMsg("start-end");
        return;
    }
}

bool WiFiMan::clientMode()
{
    debugHelper.printFunctionCall("clientMode");
    WiFi.mode(WIFI_STA);
    
    _mode=MODE::CLIENT;

    //auto connect
    if(!validConfig())
    {
        debugHelper.println("Invalid config. Exit client mode.");
        debugHelper.printLastMsg("clientMode-end");
        return false;
    }
    {
        debugHelper.println("Connectiong to AP using saved config...");
        if(connect(_wifiSsid,_wifiPasswd))
        {
            debugHelper.println("Connected to AP");
            debugHelper.printLastMsg("clientMode-end");
            return true;
        }
        else
        {
            debugHelper.println("Cannot connected to AP");
            debugHelper.printLastMsg("clientMode-end");
            return false;
        }
    }

}

bool WiFiMan::apMode()
{
    //Serial.println(_apIp.toString());
    debugHelper.printFunctionCall("apMode");
    _mode = MODE::AP;

    //setup web server
    setupWebServer();

    debugHelper.println("Set wifi mode : WIFI_AP_STA");
    WiFi.mode(WIFI_AP_STA);
    dnsServer.reset(new DNSServer());

    //setup soft ap
    WiFi.softAPConfig(_apIp, _apGateway, _apSubnet);
    String apSSID = _apName + String( ESP.getChipId());

    if(_apPasswd == "")
        WiFi.softAP(apSSID.c_str());
    else
        WiFi.softAP(apSSID.c_str(),_apPasswd.c_str());


    debugHelper.println("SoftAP SIID : "+apSSID);
    debugHelper.println("Server IP : "+_apIp.toString());


    delay(500);

    debugHelper.println("Start DNS server");
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    //bool dnsResult = dnsServer->start(_DNS_PORT, "*", _apIp);
    if(dnsServer->start(_DNS_PORT, "*", _apIp))
        debugHelper.println("DNS success");
    else
        debugHelper.println("DNS failed");
    
    //startweb server
    debugHelper.println("Start web server");
    webServer->begin();

    int startConfigTime = millis();
    debugHelper.println("Started config potal");
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


        //handle web request
        dnsServer->processNextRequest();
        webServer->handleClient();
        serialController.handleSerial();
    }

    WiFi.softAPdisconnect(true);
    stopWebServer();
    stopDnsServer();
    if(!isConnected())
    {
        debugHelper.println("Config potal timeout");
        _mode = MODE::TIMEOUT;
        debugHelper.printLastMsg("apMode-end");
        return false;
    }
    else
    {
        debugHelper.println("Connected to AP");
        debugHelper.printLastMsg("apMode-end");
        return true;
    }
}

void WiFiMan::handleNotFound()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    debugHelper.printSingleFunctionCall("handleNotFound");
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

    debugHelper.printSingleFunctionCall("handleRoot");
    String page = FPSTR(HTTP_HEADER);
    page=page + FPSTR(HTTP_INDEX);
    page=page + FPSTR(HTTP_FOOTER);
    
    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);

    page = applyTheme(page);

    webServer->send ( 200, "text/html", page );
}



void WiFiMan::handleConfig()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    debugHelper.printSingleFunctionCall("handleConfig");
    String page = FPSTR(HTTP_HEADER);
    if(AUTHENTICATION)
        page=page + FPSTR(HTTP_CONFIG_AUTH);
    else
        page=page + FPSTR(HTTP_CONFIG_NORM);
    page=page + FPSTR(HTTP_FOOTER);
    
    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    
    page = applyTheme(page);

    webServer->send ( 200, "text/html", page );
}

void WiFiMan::handleClearSetting()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
        return webServer->requestAuthentication();

    debugHelper.printSingleFunctionCall("handleClearSetting");
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

    page = applyTheme(page);

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

    debugHelper.printSingleFunctionCall("handleReset");
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

    page = applyTheme(page);

    webServer->send ( 200, "text/html", page );
    _action = ACTION_TYPE::SYS_RESET;
}

void WiFiMan::handleSave()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword()))
            return webServer->requestAuthentication();

    debugHelper.printFunctionCall("handleSave");

    String wifiSsid = webServer->arg("wifiSsid").c_str();
    String wifiPasswd = webServer->arg("wifiPasswd").c_str();
    String mqttAddr = webServer->arg("mqttAddr").c_str();
    String mqttPort = webServer->arg("mqttPort").c_str();
    String mqttUsername = webServer->arg("mqttUsername").c_str();
    String mqttPasswd = webServer->arg("mqttPasswd").c_str();
    String mqttSub = webServer->arg("mqttSub").c_str();
    String mqttPub = webServer->arg("mqttPub").c_str();
    String mqttId = webServer->arg("mqttId").c_str();
    String masterPasswd;
    String confirmPasswd;

    if(AUTHENTICATION)
    {
        masterPasswd = webServer->arg("masterPasswd").c_str();
        confirmPasswd = webServer->arg("confirmPasswd").c_str();
    }
    else
    {
        masterPasswd = "";
        confirmPasswd = "";
    }

    debugHelper.println("wifiSsid : " + wifiSsid);
    debugHelper.println("wifiPasswd : " + wifiPasswd);
    debugHelper.println("mqttAddr : " + mqttAddr);
    debugHelper.println("mqttPort : " + mqttPort);
    debugHelper.println("mqttUsername : " + mqttUsername);
    debugHelper.println("mqttPasswd : " + mqttPasswd);
    debugHelper.println("mqttSub : " + mqttSub);
    debugHelper.println("mqttPub : " + mqttPub);
    debugHelper.println("mqttId : " + mqttId);
    debugHelper.println("masterPasswd : " + masterPasswd);
    debugHelper.println("confirmPasswd : " + confirmPasswd);

    if(mqttId == "")
    {
        mqttId = _defaultMqttId + "-" + String(ESP.getChipId());
        debugHelper.println("Use default MQTT id : " + mqttId);
    }


    String errorMsg = checkInput(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd,confirmPasswd);

    if( errorMsg == "")
    {
        //input seem good, save setting
        if(masterPasswd!="")
            writeConfig(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd);//change master password
        else
            writeConfig(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,_masterPasswd);//keep old master password

        //update password for OTA updater
        otaUpdater->updatePassword(_masterPasswd);

        String page = FPSTR(HTTP_HEADERRELOAD);
        page =page + FPSTR(HTTP_INFO);
        page=page + FPSTR(HTTP_FOOTER);
        page.replace("{info}","<br/>Config saved!<br/>Connecting to network...<br/>You will be disconnect from portal if connect success.");

        page.replace("{title}",_title);
        page.replace("{banner}",_banner);
        page.replace("{build}",_build);
        page.replace("{branch}",_branch);
        page.replace("{deviceInfo}",_deviceInfo);
        page.replace("{footer}",_footer);
        page.replace("{url}","/");
        page.replace("{delay}","15");

        page = applyTheme(page);

        webServer->send ( 200, "text/html", page );
        _action = ACTION_TYPE::CONFIG_SAVED;

        debugHelper.println("config saved");
        debugHelper.printLastMsg("handleSave-end");
        return;
    }
    else
    {
        //something wrong , display error msg
        debugHelper.println("Invalid input");

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

        page = applyTheme(page);

        webServer->send ( 200, "text/html", page );
        debugHelper.printLastMsg("handleSave-end");
        return;
    }
}

void WiFiMan::handlePortal()
{
    //do not call requestAuthentication , or the captive portal will not showup in authentication mode
    if(AUTHENTICATION)
    {
        //send portal page , display device ip address
        debugHelper.printSingleFunctionCall("handlePortal");
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

        page = applyTheme(page);

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

    debugHelper.printSingleFunctionCall("handleHelp");
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

    page = applyTheme(page);

    webServer->send ( 200, "text/html", page );
}




void WiFiMan::setupWebServer()
{
    debugHelper.printFunctionCall("setupWebServer");
    //setup web server
    webServer.reset(new ESP8266WebServer(80));

    //setup ota updater
    otaUpdater.reset(new ESP8266OTA());
    otaUpdater->setUpdaterUi(_title,_banner,_build,_branch,_deviceInfo,_footer);
    if(AUTHENTICATION)
    {
        otaUpdater->setup(webServer.get(),_httpUsername.c_str(),getApPassword());
        debugHelper.println("start otaUpdater : "+_httpUsername+"@"+getApPassword());
    }
    else
    {
        otaUpdater->setup(webServer.get());
        debugHelper.println("start otaUpdater");
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
    debugHelper.printLastMsg("setupWebServer-end");
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
    debugHelper.printSingleFunctionCall("stopWebServer");
    webServer->stop();
    webServer.reset();
}

void WiFiMan::stopDnsServer()
{
    debugHelper.printSingleFunctionCall("stopDnsServer");
    dnsServer->stop();
    dnsServer.reset();
}

const char* WiFiMan::getApPassword()
{
    debugHelper.printFunctionCall("getApPassword");
    if(_masterPasswd != "")
    {
        debugHelper.println("Return password : "+_masterPasswd);
        debugHelper.printLastMsg("getApPassword-end");
        return _masterPasswd.c_str();
    }
    debugHelper.println("Return password : "+_defaultMasterPasswd);
    debugHelper.printLastMsg("getApPassword-end");
    return _defaultMasterPasswd.c_str();
}

String WiFiMan::checkInput(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd,String confirmPasswd)
{
    debugHelper.printFunctionCall("checkInput");
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

    if(AUTHENTICATION)
    {
        if(_masterPasswd=="" &&  masterPasswd =="")
            errorMsg += "New master password mut be set!<br/>"; 
        if(masterPasswd == _defaultMasterPasswd)
            errorMsg += "Invalid Password, cannot use default password!<br/>"; 
        if(masterPasswd != confirmPasswd)
            errorMsg += "Confirm password not matched<br/>"; 
    }
    if(errorMsg!="")
        debugHelper.println("Error : "+errorMsg);
    debugHelper.printLastMsg("checkInput-end");
    return errorMsg;
}

bool WiFiMan::connect(String wifiSsid,String wifiPasswd)
{
    int oldMode = _mode;
    _mode = MODE::CONNECTING;

    debugHelper.printFunctionCall("connect");
    WiFi.disconnect(); 
    //check for wifi connection
    if(wifiPasswd == "")
        WiFi.begin(wifiSsid.c_str());
    else
        WiFi.begin(wifiSsid.c_str(),wifiPasswd.c_str());
    
    debugHelper.println("Connecting to AP : " + wifiSsid + "\tPassword :" + wifiPasswd );
    for (int tryCount = 0;(WiFi.status() != WL_CONNECTED) && (tryCount < _maxConnectAttempt);tryCount++) 
    {
        debugHelper.println(".");
        delay(500);
    }

    if(WiFi.status() == WL_CONNECTED)
    {
        debugHelper.println("Connected to AP");
        debugHelper.println("IP address : " + WiFi.localIP().toString());
        _mode = MODE::CLIENT;
        debugHelper.printLastMsg("connect-end");
        return true;
    }
    else
    {
        debugHelper.println("Cannot connect to AP. Exit client mode.");
        //return to the last mode
        _mode == oldMode;
        debugHelper.printLastMsg("connect-end");
        return false;
    }
}

bool WiFiMan::validConfig()
{
    debugHelper.printFunctionCall("validConfig");
    bool returnCode = true;

    if(_wifiSsid == "")
        returnCode = false;
    if(_mqttAddr == "")
        returnCode = false;
    if(_mqttPort == "")
        returnCode = false;
    if(_mqttPort == "")
        returnCode = false;
    //if(_masterPasswd == "")
    //    returnCode = false;

    if(returnCode)
    {
        debugHelper.println("Config OK");
        debugHelper.printLastMsg("validConfig-end");
        return returnCode;
    }
    else
    {   
        debugHelper.println("Invalid config");
        debugHelper.printLastMsg("validConfig-end");
        return returnCode;
    }
    
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
void WiFiMan::setDefaultMasterPasswd(String passwd)
{
    _defaultMasterPasswd = passwd;
}
//set username to login (this cant be change later)
void WiFiMan::setMasterUsername(String username)
{
    _httpUsername = username;
}


void WiFiMan::disconnect()
{
    WiFi.disconnect(); 
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

void WiFiMan::setHelpInfo(String helpInfo)
{
    _helpInfo = helpInfo;
}

bool WiFiMan::getConfig(Config *conf)
{
    conf->wifiSsid = (char*)malloc((_wifiSsid.length()+1) * sizeof(char));
    strcpy(conf->wifiSsid,_wifiSsid.c_str());

    conf->wifiPasswd = (char*)malloc((_wifiPasswd.length()+1) * sizeof(char));
    strcpy(conf->wifiPasswd,_wifiPasswd.c_str());
    
    conf->mqttAddr = (char*)malloc((_mqttAddr.length()+1) * sizeof(char));
    strcpy(conf->mqttAddr,_mqttAddr.c_str());
    
    conf->mqttPort = _mqttPort.toInt();
    
    conf->mqttUsername = (char*)malloc((_mqttUsername.length()+1) * sizeof(char));
    strcpy(conf->mqttUsername,_mqttUsername.c_str());
    
    conf->mqttPasswd = (char*)malloc((_mqttPasswd.length()+1) * sizeof(char));
    strcpy(conf->mqttPasswd,_mqttPasswd.c_str());
    
    conf->mqttSub = (char*)malloc((_mqttSub.length()+1) * sizeof(char));
    strcpy(conf->mqttSub,_mqttSub.c_str());
    
    conf->mqttPub = (char*)malloc((_mqttPub.length()+1) * sizeof(char));
    strcpy(conf->mqttPub,_mqttPub.c_str());
    
    conf->mqttId = (char*)malloc((_mqttId.length()+1) * sizeof(char));
    strcpy(conf->mqttId,_mqttId.c_str());
    
    conf->masterPasswd = (char*)malloc((_masterPasswd.length()+1) * sizeof(char));
    strcpy(conf->masterPasswd,_masterPasswd.c_str());

    String mdnsName = _mqttId + ".local";
    conf->mdnsName = (char*)malloc((mdnsName.length()+1) * sizeof(char));
    strcpy(conf->mdnsName,mdnsName.c_str());

    conf->localIP = getIp();

    if(WiFi.status() == WL_CONNECTED)
        return true;
    return false;
}

bool WiFiMan::getBootMode()
{
    //printDebug("getBootMode",true);
    if(SPIFFS.begin())
    {
        if (SPIFFS.exists("/boot.conf")) 
        {
            SPIFFS.remove("/boot.conf");
            SPIFFS.end();
            return true;
        }
        else
        {
            SPIFFS.end();
            return false;
        }
    }
    else
    {
        //printDebug("Failed to mount FS",false);
        return false;
    }
}

String WiFiMan::applyTheme(String pageStr)
{
    String page = pageStr;
    
    page.replace("{body-text-color}",FPSTR(body_text_color));
    page.replace("{body-background-image}",FPSTR(body_background_image));
    page.replace("{body-background-color}",FPSTR(body_background_color));
    
    page.replace("{button-text-color}",FPSTR(button_text_color));
    page.replace("{button-background-image}",FPSTR(button_background_image));
    page.replace("{button-backround-color}",FPSTR(button_backround_color));

    page.replace("{header-text-color}",FPSTR(header_text_color));
    page.replace("{header-background-image}",FPSTR(header_background_image));
    page.replace("{header-background-color}",FPSTR(header_background_color));

    page.replace("{footer-text-color}",FPSTR(footer_text_color));
    page.replace("{footer-background-image}",FPSTR(footer_background_image));
    page.replace("{footer-background-color}",FPSTR(footer_background_color));

    return page;
}