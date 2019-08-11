#include "WiFiMan.h"

WiFiMan::WiFiMan()
{
    WiFiMan(false);
}

WiFiMan::WiFiMan(bool authentication)
{
    AUTHENTICATION = authentication;
    WiFi.disconnect();
}


void WiFiMan::setAuthentication(bool enable)
{
    AUTHENTICATION=enable;
}

bool WiFiMan::deleteConfig()
{
    bool delResult = true;
    DEBUG_MSG("#>> deleteConfig\n");
    if(SPIFFS.begin())
    {
        if(SPIFFS.exists("/config.json"))
        {
            if(SPIFFS.remove("/config.json\n"))
                DEBUG_MSG("#__ Deleted config.json\n");
            else
            {
                DEBUG_MSG("#__ Cannot delete config.json\n");
                delResult = false;
            }
        }
        else
        {
            DEBUG_MSG("#__ config.json is not exists\n");
        }

        if (SPIFFS.exists("/customConfig.json")) 
        {
            if(SPIFFS.remove("/customConfig.json"))
                DEBUG_MSG("#__ Deleted customConfig.json.\n");
            else
            {
                DEBUG_MSG("#__ Cannot delete customConfig.json\n");
                delResult = false;
            }
        }
        else
        {
            DEBUG_MSG("#__ customConfig.json is not exists\n");
        }
        
        DEBUG_MSG("#<< deleteConfig-end\n");
        SPIFFS.end();
        return delResult;
    }
    else
    {
        DEBUG_MSG("#__ Failed to mount FS\n");
        DEBUG_MSG("#<< deleteConfig-end\n");
        return false;
    }
}


void WiFiMan::forceApMode()
{
    FORCE_AP=true;
}

bool WiFiMan::readConfig()
{
    DEBUG_MSG("#>> readConfig\n");
    if(SPIFFS.begin())
    {
        if (SPIFFS.exists("/config.json")) 
        {
            DEBUG_MSG("#__ Reading config.json\n");
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) 
            {
                size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                //DynamicJsonBuffer jsonBuffer;
                DynamicJsonDocument jsonDoc(JSON_BUFFER_SIZE);
                //JsonObject& json = jsonBuffer.parseObject(buf.get());
                auto jsonError = deserializeJson(jsonDoc, buf.get());
                JsonObject json = jsonDoc.as<JsonObject>();

                if(jsonError)
                {
                    DEBUG_MSG("#__ Cannot parse json. Wrong format ?\n");
                    DEBUG_MSG("#__ Close config.js\n");
                    configFile.close();
                    DEBUG_MSG("#__ Unmount FS\n");
                    SPIFFS.end();

                    DEBUG_MSG("#__ Trying to delete config.js\n");
                    deleteConfig();
                    DEBUG_MSG("#__ write template for config.js\n");
                    writeConfig("","","","","","","","","","");
                    DEBUG_MSG("#<< readConfig-end\n");
                    return false;
                }
                else
                {
                    #ifdef DEBUG_ESP_PORT
                        DEBUG_MSG("#__ Json : ");
                        //json.printTo(DEBUG_ESP_PORT);
                        serializeJson(json, DEBUG_ESP_PORT);
                        DEBUG_MSG("\n");
                    #endif
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
                    DEBUG_MSG("#<< readConfig-end\n");
                    return true;
                }
            }
            else
            {
                DEBUG_MSG("#__ Cannot open config.json\n");
                DEBUG_MSG("#__ Unmount FS\n");
                SPIFFS.end();
                DEBUG_MSG("#<< readConfig-end\n");
                return false;
            }
        }
        else
        {
            DEBUG_MSG("#__ config.json not exists\n");
            DEBUG_MSG("#__ Unmount FS\n");
            SPIFFS.end();
            DEBUG_MSG("#__ write template for config.js\n");
            writeConfig("","","","","","","","","","");
            DEBUG_MSG("#<< readConfig-end\n");
            return false;
        }
    }
    else
    {
        DEBUG_MSG("#__ Failed to mount FS\n");
        DEBUG_MSG("#<< readConfig-end\n");
        return false;
    }
}

bool WiFiMan::writeConfig(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd)
{
    DEBUG_MSG("#>> writeConfig\n");
    DEBUG_MSG("#__ Updating config data\n");
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

    DEBUG_MSG("#__ Writing config.json\n");
    //DynamicJsonBuffer jsonBuffer;
    DynamicJsonDocument jsonDoc(JSON_BUFFER_SIZE);
    //JsonObject& json = jsonBuffer.createObject();
    //JsonObject json = jsonDoc.createObject();

    jsonDoc["wifiSsid"] = _wifiSsid;
    jsonDoc["wifiPasswd"] = _wifiPasswd;
    jsonDoc["mqttAddr"] = _mqttAddr;
    jsonDoc["mqttPort"] = _mqttPort;
    jsonDoc["mqttUsername"] = _mqttUsername;
    jsonDoc["mqttPasswd"] = _mqttPasswd;
    jsonDoc["mqttSub"] = _mqttSub;
    jsonDoc["mqttPub"] = _mqttPub;
    jsonDoc["mqttId"] = _mqttId;
    jsonDoc["masterPasswd"] = _masterPasswd;
    
    
    if(SPIFFS.begin())
    {
        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) 
        {
            DEBUG_MSG("#__ Failed to open config file for writing\n");
            DEBUG_MSG("#__ Unmount FS\n");
            SPIFFS.end();
            DEBUG_MSG("#<< writeConfig-end\n");
            return false;
        }

        //jsonDoc.printTo(configFile);
        serializeJson(jsonDoc, configFile);
        DEBUG_MSG("#__ Save successed!\n");
        configFile.close();
        DEBUG_MSG("#__ Unmount FS\n");
        SPIFFS.end();
        DEBUG_MSG("#<< writeConfig-end\n");
        return true;
    }
    else
    {
        DEBUG_MSG("#__ Failed to mount FS\n");
        DEBUG_MSG("#<< writeConfig-end\n");
        return false;
    }
}

void WiFiMan::start()
{
    #ifdef DEBUG_ESP_PORT
        DEBUG_ESP_PORT.begin(115200);
        DEBUG_ESP_PORT.setDebugOutput(false);
    #endif
    DEBUG_MSG("#>> start\n");

    //turn off indicator led
    setLedState(false);

    //get boot mode
    if(!FORCE_AP)
        FORCE_AP = getBootMode();
    //read config file
    if(readConfig() && !FORCE_AP)
    {
        DEBUG_MSG("#__ Trying to connect to AP\n");
        //success , try to connect to AP
        if(clientMode())
        {
            //connect success.
            //do nothing, leave the work for main program
            DEBUG_MSG("#__ Connected to AP\n");
            DEBUG_MSG("#<< start-end\n");
            resetLedState();
            return;
        }
        else
        {
            //connect failed , fire up softAP
            DEBUG_MSG("#__ Cannot connected to AP\n");
            WiFi.disconnect();
            apMode();
            DEBUG_MSG("#<< start-end\n");
            resetLedState();
            return;
        }
    }
    else
    {
        //failed to read config file or force AP mode is enabled then fire up softAP
        DEBUG_MSG("#__ Cannot read config.json or FORCE_AP is enabled\n");
        FORCE_AP = false;
        apMode();
        DEBUG_MSG("#<< start-end\n");
        resetLedState();
        return;
    }
}

bool WiFiMan::clientMode()
{
    //turn on led during client mode
    setLedState(true);

    DEBUG_MSG("#>> clientMode\n");
    WiFi.mode(WIFI_STA);
    
    _mode=MODE::CLIENT;

    //auto connect
    if(!validConfig())
    {
        DEBUG_MSG("#__ Invalid config. Exit client mode.\n");
        DEBUG_MSG("#<< clientMode-end\n");
        setLedState(false);
        return false;
    }
    else
    {
        DEBUG_MSG("#__ Connectiong to AP using saved config...\n");
        if(connect(_wifiSsid,_wifiPasswd))
        {
            DEBUG_MSG("#__ Connected to A\nP");
            DEBUG_MSG("#<< clientMode-end\n");
            setLedState(false);
            return true;
        }
        else
        {
            DEBUG_MSG("#__ Cannot connected to AP\n");
            DEBUG_MSG("#<< clientMode-end\n");
            setLedState(false);
            return false;
        }
    }
}

bool WiFiMan::apMode()
{
    //Serial.println(_apIp.toString());
    DEBUG_MSG("#>> apMode\n");
    _mode = MODE::AP;

    //setup web server
    setupWebServer();

    DEBUG_MSG("#__ Set wifi mode : WIFI_AP_STA\n");
    WiFi.mode(WIFI_AP_STA);
    dnsServer.reset(new DNSServer());

    //setup soft ap
    WiFi.softAPConfig(_apIp, _apGateway, _apSubnet);
    String apSSID = _apName + String( ESP.getChipId());

    DEBUG_MSG("#__ SoftAP SIID : %s\n",apSSID.c_str());
    if(_apPasswd == "")
        WiFi.softAP(apSSID.c_str());
    else
        WiFi.softAP(apSSID.c_str(),_apPasswd.c_str());

    DEBUG_MSG("#__ SoftAP SIID : %s Passwd : %s\n",apSSID.c_str(),_apPasswd.c_str());
    DEBUG_MSG("#__ Server IP : %s\n",_apIp.toString().c_str());

    delay(500);

    DEBUG_MSG("#__ Start DNS server\n");
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    //bool dnsResult = dnsServer->start(_DNS_PORT, "*", _apIp);
    if(dnsServer->start(_DNS_PORT, "*", _apIp))
        DEBUG_MSG("#__ DNS success\n");
    else
        DEBUG_MSG("#__ DNS failed\n");
    
    //startweb server
    DEBUG_MSG("#__ Start web server\n");
    webServer->begin();

    int startConfigTime = millis();
    DEBUG_MSG("#__ Started config potal\n");
    while(millis()-startConfigTime < _configTimeout*60000)
    {
        //blink the led 
        handleIndicatorLed();
        handleExtFunc();

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
        {
            DEBUG_MSG("#__ Connected. Break the loop\n");
            break;
        }
        else
        {
             //if password has been changed and device failed to connect to network, reset the device to apply new password.
            if(passwdChanged && _action==ACTION_TYPE::NONE)
            {
                DEBUG_MSG("#__ Password has been chaged, restart...\n");
                ESP.restart();
            }
        }
        //handle web request
        dnsServer->processNextRequest();
        webServer->handleClient();
    }

    WiFi.softAPdisconnect(true);
    stopWebServer();
    stopDnsServer();
    setLedState(false);
    if(!isConnected())
    {
        DEBUG_MSG("#__ Config potal timeout\n");
        _mode = MODE::TIMEOUT;
        DEBUG_MSG("#<< apMode-end\n");
        return false;
    }
    else
    {
        DEBUG_MSG("#__ Connected to AP\n");
        DEBUG_MSG("#<< apMode-end\n");
        return true;
    }
}

void WiFiMan::handleNotFound()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword().c_str()))
            return webServer->requestAuthentication();

    DEBUG_MSG("#><  handleNotFound\n");
    String page = FPSTR(HTTP_HEADERRELOAD);
    page.concat(FPSTR(HTTP_INFO));
    page.concat(FPSTR(HTTP_FOOTER));
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
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword().c_str()))
            return webServer->requestAuthentication();

    DEBUG_MSG("#><  handleRoot\n");
    String page = FPSTR(HTTP_HEADER);
    page.concat(FPSTR(HTTP_INDEX));
    page.concat(FPSTR(HTTP_FOOTER));
    
    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);

    applyTheme(page);

    webServer->send ( 200, "text/html", page );
}

void WiFiMan::handleConfig()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword().c_str()))
            return webServer->requestAuthentication();

    DEBUG_MSG("#><  handleConfig\n");

    String page = FPSTR(HTTP_HEADER);
    page.concat(FPSTR(HTTP_CONFIG_WIFI_DEVICE));

    if(MQTT)
        page.concat(FPSTR(HTTP_CONFIG_MQTT));

    if(AUTHENTICATION)
        page.concat(FPSTR(HTTP_CONFIG_AUTH));

    page.concat(FPSTR(HTTP_CONFIG_END));
    page.concat(FPSTR(HTTP_FOOTER));
    
    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    
    applyTheme(page);

    webServer->send ( 200, "text/html", page );
}

void WiFiMan::handleClearSetting()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword().c_str()))
        return webServer->requestAuthentication();

    DEBUG_MSG("#><  handleClearSetting\n");
    String page = FPSTR(HTTP_HEADERRELOAD);
    page.concat(FPSTR(HTTP_INFO));
    page.concat(FPSTR(HTTP_FOOTER));
    page.replace("{info}","<br/>All setting cleared<br/>Device will restart after 15 seconds.");

    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    page.replace("{url}","/");
    page.replace("{delay}","15");

    applyTheme(page);

    webServer->send ( 200, "text/html", page );
    delay(2000);
    webServer->client().stop();
    _action = ACTION_TYPE::CLEAR_CONFIG;
}

void WiFiMan::handleReset()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword().c_str()))
            return webServer->requestAuthentication();

    DEBUG_MSG("#><  handleReset\n");
    String page = FPSTR(HTTP_HEADERRELOAD);
    page.concat(FPSTR(HTTP_INFO));
    page.concat(FPSTR(HTTP_FOOTER));
    page.replace("{info}","<br/>Device will restart after 15 seconds.");

    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{footer}",_footer);
    page.replace("{url}","/");
    page.replace("{delay}","15");

    applyTheme(page);
    webServer->send ( 200, "text/html", page );
    delay(2000);
    webServer->client().stop();
    _action = ACTION_TYPE::SYS_RESET;
}

void WiFiMan::handleSave()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword().c_str()))
            return webServer->requestAuthentication();

    DEBUG_MSG("#>> handleSave\n");

    //get wifi config data
    String wifiSsid = webServer->arg("wifiSsid").c_str();
    String wifiPasswd = webServer->arg("wifiPasswd").c_str();

    //mqttid or deviceId
    String mqttId =  webServer->arg("mqttId").c_str();
    if(mqttId == "")
    {
        mqttId = _defaultMqttId + "-" + String(ESP.getChipId());
        DEBUG_MSG("#__ Use default MQTT id : %s\n" , mqttId.c_str());
    }

    //get mqtt config
    String mqttAddr;
    String mqttPort;
    String mqttUsername;
    String mqttPasswd;
    String mqttSub;
    String mqttPub;
    if(MQTT)
    {
        mqttAddr = webServer->arg("mqttAddr").c_str();
        mqttPort = webServer->arg("mqttPort").c_str();
        mqttUsername = webServer->arg("mqttUsername").c_str();
        mqttPasswd = webServer->arg("mqttPasswd").c_str();
        mqttSub = webServer->arg("mqttSub").c_str();
        mqttPub = webServer->arg("mqttPub").c_str();
    }
    else
    {
        mqttAddr = "";
        mqttPort = "";
        mqttUsername = "";
        mqttPasswd = "";
        mqttSub = "";
        mqttPub = "";
    }

    //Grt Authentication config
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

    //get custom config data
    if(customConfig.count)
    {
        for(int i=0;i<customConfig.count;i++)
        {
            customConfig.args[i].value = webServer->arg(customConfig.args[i].key).c_str();
        }
    }

    DEBUG_MSG("#__ wifiSsid : %s\n" , wifiSsid.c_str());
    DEBUG_MSG("#__ wifiPasswd : %s\n" , wifiPasswd.c_str());
    DEBUG_MSG("#__ mqttAddr : %s\n" , mqttAddr.c_str());
    DEBUG_MSG("#__ mqttPort : %s\n" , mqttPort.c_str());
    DEBUG_MSG("#__ mqttUsername : %s\n" , mqttUsername.c_str());
    DEBUG_MSG("#__ mqttPasswd : %s\n" , mqttPasswd.c_str());
    DEBUG_MSG("#__ mqttSub : %s\n" , mqttSub.c_str());
    DEBUG_MSG("#__ mqttPub : %s\n" , mqttPub.c_str());
    DEBUG_MSG("#__ mqttId : %s\n" , mqttId.c_str());
    DEBUG_MSG("#__ masterPasswd : %s\n" , masterPasswd.c_str());
    DEBUG_MSG("#__ confirmPasswd : %s\n" , confirmPasswd.c_str());

    if(mqttId == "")
    {
        mqttId = _defaultMqttId + "-" + String(ESP.getChipId());
        DEBUG_MSG("#__ Use default MQTT id : %s\n" , mqttId.c_str());
    }

    String errorMsg = checkInput(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd,confirmPasswd);

    if( errorMsg == "")
    {
        //input seem good, save setting
        if(masterPasswd != "")
        {
            writeConfig(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,masterPasswd);//change master password
            passwdChanged = true;
        }
        else
            writeConfig(wifiSsid,wifiPasswd,mqttAddr,mqttPort,mqttUsername,mqttPasswd,mqttSub,mqttPub,mqttId,_masterPasswd);//keep old master password

        //save custom config
        saveCustomConfig();

        //update password for OTA updater
        otaUpdater->updatePassword(_masterPasswd);

        String page = FPSTR(HTTP_HEADERRELOAD);
        page.concat(FPSTR(HTTP_INFO));
        page.concat(FPSTR(HTTP_FOOTER));

        //due Authentication has been changed, to connect to portal after changed password , esp8266 need to be reboot
        if(passwdChanged)
            page.replace("{info}","<br/>Config saved!<br/>Connecting to network...<br/>You will be disconnect from portal if connect success.<br/>If failed, device will reset to apply new password. ");
        else
            page.replace("{info}","<br/>Config saved!<br/>Connecting to network...<br/>You will be disconnect from portal if connect success.");

        page.replace("{title}",_title);
        page.replace("{banner}",_banner);
        page.replace("{build}",_build);
        page.replace("{branch}",_branch);
        page.replace("{deviceInfo}",_deviceInfo);
        page.replace("{footer}",_footer);
        page.replace("{url}","/");
        page.replace("{delay}","30");

        applyTheme(page);

        webServer->send ( 200, "text/html", page );
        _action = ACTION_TYPE::CONFIG_SAVED;

        DEBUG_MSG("#__ config saved\n");
        DEBUG_MSG("#<< handleSave-end\n");
        return;
    }
    else
    {
        //something wrong , display error msg
        DEBUG_MSG("#__ Invalid input\n");

        String page = FPSTR(HTTP_HEADER);
        page.concat(FPSTR(HTTP_EDIT));
        page.concat(FPSTR(HTTP_FOOTER));
        page.replace("{info}",errorMsg);

        page.replace("{title}",_title);
        page.replace("{banner}",_banner);
        page.replace("{build}",_build);
        page.replace("{branch}",_branch);
        page.replace("{deviceInfo}",_deviceInfo);
        page.replace("{footer}",_footer);

        applyTheme(page);

        webServer->send ( 200, "text/html", page );
        DEBUG_MSG("#<< handleSave-end\n");
        return;
    }
}

void WiFiMan::handlePortal()
{
    //do not call requestAuthentication , or the captive portal will not showup in authentication mode
    if(AUTHENTICATION)
    {
        //send portal page , display device ip address
        DEBUG_MSG("#><  handlePortal\n");
        String page = FPSTR(HTTP_HEADER);
        page.concat(FPSTR(HTTP_PORTAL));
        page.concat(FPSTR(HTTP_FOOTER));

        page.replace("{title}",_title);
        page.replace("{banner}",_banner);
        page.replace("{build}",_build);
        page.replace("{branch}",_branch);
        page.replace("{deviceInfo}",_deviceInfo);
        page.replace("{footer}",_footer);
        page.replace("{ip}",_apIp.toString());

        applyTheme(page);

        webServer->send ( 200, "text/html", page );
    }
    else
        return handleRoot();
}

void WiFiMan::handleHelp()
{
    //Authentication
    if(AUTHENTICATION)
        if(!webServer->authenticate(_httpUsername.c_str(),getApPassword().c_str()))
            return webServer->requestAuthentication();

    DEBUG_MSG("#><  handleHelp\n");
    String page = FPSTR(HTTP_HEADER);
    page.concat(FPSTR(HTTP_HELP));
    page.concat(FPSTR(HTTP_FOOTER));

    page.replace("{title}",_title);
    page.replace("{banner}",_banner);
    page.replace("{build}",_build);
    page.replace("{branch}",_branch);
    page.replace("{deviceInfo}",_deviceInfo);
    page.replace("{helpInfo}",_helpInfo);
    page.replace("{footer}",_footer);
    page.replace("{url}","/");
    page.replace("{delay}","15");

    applyTheme(page);

    webServer->send ( 200, "text/html", page );
}

void WiFiMan::setupWebServer()
{
    DEBUG_MSG("#>> setupWebServer\n");
    //setup web server
    webServer.reset(new ESP8266WebServer(80));

    //setup ota updater
    otaUpdater.reset(new ESP8266OTA());
    otaUpdater->setUpdaterUi(_title,_banner,_build,_branch,_deviceInfo,_footer);
    if(AUTHENTICATION)
    {
        otaUpdater->setup(webServer.get(),_httpUsername.c_str(),getApPassword());
        DEBUG_MSG("#__ start otaUpdater : %s@%s\n",_httpUsername.c_str(),getApPassword().c_str());
    }
    else
    {
        otaUpdater->setup(webServer.get());
        DEBUG_MSG("#__ start otaUpdater\n");
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
    DEBUG_MSG("#<< setupWebServer-end\n");
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
    DEBUG_MSG("#><  stopWebServer\n");
    webServer->stop();
    webServer.reset();
}

void WiFiMan::stopDnsServer()
{
    DEBUG_MSG("#><  stopDnsServer\n");
    dnsServer->stop();
    dnsServer.reset();
}

String WiFiMan::getApPassword()
{
    DEBUG_MSG("#>> getApPassword\n");
    if(_masterPasswd != "")
    {
        DEBUG_MSG("#__ Return password : %s\n",_masterPasswd.c_str());
        DEBUG_MSG("#<< getApPassword-end\n");
        return _masterPasswd;
    }
    DEBUG_MSG("#__ Return default password : %s\n",_defaultMasterPasswd.c_str());
    DEBUG_MSG("#<< getApPassword-end\n");
    return _defaultMasterPasswd;
}

String WiFiMan::checkInput(String wifiSsid,String wifiPasswd,String mqttAddr,String mqttPort,String mqttUsername,String mqttPasswd,String mqttSub,String mqttPub,String mqttId,String masterPasswd,String confirmPasswd)
{
    DEBUG_MSG("#>> checkInput\n");
    String errorMsg = "";
    if(wifiSsid == "")
        errorMsg.concat("Invalid SSID<br/>"); 
    //skip check for wifiPasswd (unsecure ap)

    if(MQTT)
    {
        if(mqttAddr == "")
            errorMsg.concat("Invalid MQTT address<br/>"); 
        if(mqttPort == "")
            errorMsg.concat("Invalid MQTT port<br/>"); 
        if((mqttUsername != "" && mqttPasswd == "") || (mqttUsername == "" && mqttPasswd != ""))
            errorMsg.concat("Invalid MQTT username or password<br/>"); 
    }
    //skip check for mqtt id , id not set , use esp8266 chipID instead


    if(AUTHENTICATION)
    {
        if(_masterPasswd=="" &&  masterPasswd =="")
            errorMsg.concat("New master password mut be set!<br/>"); 
        if(masterPasswd == _defaultMasterPasswd)
            errorMsg.concat("Invalid Password, cannot use default password!<br/>"); 
        if(masterPasswd != confirmPasswd)
            errorMsg.concat("Confirm password not matched<br/>"); 
    }
    if(errorMsg!="")
        DEBUG_MSG("#__ Error : %s\n" , errorMsg.c_str());
    DEBUG_MSG("#<< checkInput-end\n");
    return errorMsg;
}

bool WiFiMan::connect(String wifiSsid,String wifiPasswd)
{
    int oldMode = _mode;
    _mode = MODE::CONNECTING;

    DEBUG_MSG("#>> connect\n");
    WiFi.disconnect(); 
    //check for wifi connection
    if(wifiPasswd == "")
        WiFi.begin(wifiSsid.c_str());
    else
        WiFi.begin(wifiSsid.c_str(),wifiPasswd.c_str());
    
    DEBUG_MSG("#__ Connecting to AP : SSID : %s \tPassword : %s\n" , wifiSsid.c_str() ,  wifiPasswd.c_str() );
    for (int tryCount = 0;(WiFi.status() != WL_CONNECTED) && (tryCount < _maxConnectAttempt);tryCount++) 
    {
        DEBUG_MSG("#__ .\n");
        delay(_connect_delay);
        handleExtFunc();
        if(handleConnectInterrupt())
            return false;
    }

    if(WiFi.status() == WL_CONNECTED)
    {
        DEBUG_MSG("#__ Connected to AP\n");
        DEBUG_MSG("#__ IP address : %s\n" , WiFi.localIP().toString().c_str());
        _mode = MODE::CLIENT;
        DEBUG_MSG("#<< connect-end\n");
        return true;
    }
    else
    {
        DEBUG_MSG("#__ Cannot connect to AP. Exit client mode.\n");
        //return to the last mode
        _mode == oldMode;
        DEBUG_MSG("#<< connect-end\n");
        return false;
    }
}

bool WiFiMan::validConfig()
{
    DEBUG_MSG("#>> validConfig\n");
    bool returnCode = true;

    if(_wifiSsid == "")
        returnCode = false;
    if(MQTT)
    {
        if(_mqttAddr == "")
            returnCode = false;
        if(_mqttPort == "")
            returnCode = false;
    }

    if(returnCode)
    {
        DEBUG_MSG("#__ Config OK\n");
        DEBUG_MSG("#<< validConfig-end\n");
        return returnCode;
    }
    else
    {   
        DEBUG_MSG("#__ Invalid config\n");
        DEBUG_MSG("#<< validConfig-end\n");
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
//get mqtt id / device id
String WiFiMan::getMqttId() 
{ 
    return _mqttId;
}
String WiFiMan::getDeviceId()
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
    //conf->wifiSsid = (char*)malloc((_wifiSsid.length()+1) * sizeof(char));
    strcpy(conf->wifiSsid,_wifiSsid.c_str());

    //conf->wifiPasswd = (char*)malloc((_wifiPasswd.length()+1) * sizeof(char));
    strcpy(conf->wifiPasswd,_wifiPasswd.c_str());
    
    //conf->mqttAddr = (char*)malloc((_mqttAddr.length()+1) * sizeof(char));
    strcpy(conf->mqttAddr,_mqttAddr.c_str());
    
    conf->mqttPort = _mqttPort.toInt();
    
    //conf->mqttUsername = (char*)malloc((_mqttUsername.length()+1) * sizeof(char));
    strcpy(conf->mqttUsername,_mqttUsername.c_str());
    
    //conf->mqttPasswd = (char*)malloc((_mqttPasswd.length()+1) * sizeof(char));
    strcpy(conf->mqttPasswd,_mqttPasswd.c_str());
    
    //conf->mqttSub = (char*)malloc((_mqttSub.length()+1) * sizeof(char));
    strcpy(conf->mqttSub,_mqttSub.c_str());
    
    //conf->mqttPub = (char*)malloc((_mqttPub.length()+1) * sizeof(char));
    strcpy(conf->mqttPub,_mqttPub.c_str());
    
    //conf->mqttId = (char*)malloc((_mqttId.length()+1) * sizeof(char));
    strcpy(conf->mqttId,_mqttId.c_str());
    
    //conf->masterPasswd = (char*)malloc((_masterPasswd.length()+1) * sizeof(char));
    strcpy(conf->masterPasswd,_masterPasswd.c_str());

    String mdnsName = _mqttId + ".local";
    //conf->mdnsName = (char*)malloc((mdnsName.length()+1) * sizeof(char));
    strcpy(conf->mdnsName,mdnsName.c_str());

    conf->localIP = getIp();

    if(WiFi.status() == WL_CONNECTED)
        return true;
    return false;
}

void WiFiMan::applyTheme(String &page)
{
    DEBUG_MSG("#>< Apply theme\n");
    page.replace("{custom-css}",FPSTR(custom_css));

    //testing - add custom config
    if( customConfig.count > 0)
        page.replace("{custom-arg}",httpCustomArg);
    else
        page.replace("{custom-arg}","");
}

void WiFiMan::addCustomArg(String label,String name,String length,String type,String placeholder,String addition)
{
    DEBUG_MSG("#>< addCustomArg\n");
    String arg = FPSTR(HTTP_CUSTOM_ARG);

    //add to arg pool
    arg.replace("{arg-label}",label);
    arg.replace("{arg-name}",name);
    arg.replace("{arg-length}",length);
    arg.replace("{arg-type}",type);
    arg.replace("{arg-place-holder}",placeholder);
    arg.replace("{arg-addition}",addition);
    httpCustomArg.concat(arg);

    //add to custom arg struct
    customConfig.args[customConfig.count].key = name;
    customConfig.count++;
}

bool WiFiMan::saveCustomConfig()
{
    DEBUG_MSG("#>> saveCustomConfig\n");
    if(customConfig.count)
    {
        //make json data
        //DynamicJsonBuffer jsonBuffer;
        DynamicJsonDocument jsonDoc(JSON_BUFFER_SIZE);
        //JsonObject& json = jsonBuffer.createObject();
        //JsonObject& json = jsonDoc.createObject();

        String keyList = "";
        for(int i=0;i<customConfig.count;i++)
        {
            DEBUG_MSG("#__ %s : %s \n",customConfig.args[i].key.c_str(),customConfig.args[i].value.c_str());
            jsonDoc[customConfig.args[i].key] = customConfig.args[i].value;
            
            //append key list
            keyList.concat(customConfig.args[i].key);
            keyList.concat(",");
        }

        //write list of key and counter to json
        jsonDoc["key-count"] = customConfig.count;
        jsonDoc["key-list"] = keyList;

        DEBUG_MSG("#__ Writing customConfig.json\n");
        if(SPIFFS.begin())
        {
            File configFile = SPIFFS.open("/customConfig.json", "w");
            if (!configFile) 
            {
                DEBUG_MSG("#__ Failed to open customConfig file for writing\n");
                DEBUG_MSG("#__ Unmount FS\n");
                SPIFFS.end();
                DEBUG_MSG("#<< saveCustomConfig-end\n");
                return false;
            }

            //jsonDoc.printTo(configFile);
            serializeJson(jsonDoc, configFile);
            DEBUG_MSG("#__ Save successed!\n");
            configFile.close();
            DEBUG_MSG("#__ Unmount FS\n");
            SPIFFS.end();
            DEBUG_MSG("#<< saveCustomConfig-end\n");
            return true;
        }
        else
        {
            DEBUG_MSG("#__ Failed to mount FS\n");
            DEBUG_MSG("#<< saveCustomConfig-end\n");
            return false;
        }

    }
    else
    {
        DEBUG_MSG("#__ There is no custom config to save.\n");
        DEBUG_MSG("#__ Trying to delete old customConfig.json if exists.\n");

        //delete old customConfig.json
        if(SPIFFS.begin())
        {
            if (SPIFFS.exists("/customConfig.json")) 
                SPIFFS.remove("/customConfig.json");
            DEBUG_MSG("#__ Deleted customConfig.json.\n");
            SPIFFS.end();
        }
        else
        {
            DEBUG_MSG("#__ Cannot mount FS.\n");
            DEBUG_MSG("#<< saveCustomConfig-end\n");
            return false;
        }

        DEBUG_MSG("#<< saveCustomConfig-end\n");
        return true;
    }
}

bool WiFiMan::getCustomConfig(CustomConfig *customConf)
{
    DEBUG_MSG("#>> getCustomConfigJson\n");
    if(SPIFFS.begin())
    {
        File customConfigFile = SPIFFS.open("/customConfig.json", "r");
        if (customConfigFile) 
        {
            DEBUG_MSG("#__ Reading customConfig.json.\n");
            size_t size = customConfigFile.size();
            std::unique_ptr<char[]> buf(new char[size]);

            customConfigFile.readBytes(buf.get(), size);

            //DynamicJsonBuffer jsonBuffer;
            //parse
            DynamicJsonDocument jsonDoc(JSON_BUFFER_SIZE);
            auto jsonError = deserializeJson(jsonDoc, buf.get());
            JsonObject json = jsonDoc.as<JsonObject>();


            if(jsonError)
            {
                customConfigFile.close();
                SPIFFS.end();
                DEBUG_MSG("#__ Cannot parse json.\n");
                DEBUG_MSG("#<< readCustomConfigJson-end\n");
                return false;
            }
            else
            {
                customConfigFile.close();
                SPIFFS.end();
                DEBUG_MSG("#__ Parse json : success.\n");

                // parse json to customConfig object
                //get counter
                customConf->count = json["key-count"].as<signed int>();
                //get key
                String keyList = json["key-list"].as<String>();
                for(int i=0;i<customConf->count;i++)
                {
                    int index = keyList.indexOf(',');
                    customConf->args[i].key = keyList.substring(0,index);
                    keyList.replace(customConf->args[i].key+",","");
                }
                //get value
                for(int i=0;i<customConf->count;i++)
                {
                    customConf->args[i].value = json[customConf->args[i].key].as<String>();
                }

                DEBUG_MSG("#<< readCustomConfigJson-end\n");
                return true;
            }
        }
        else
        {
            SPIFFS.end();
            DEBUG_MSG("#__ Cannot open customConfig.json\n");
            DEBUG_MSG("#<< readCustomConfigJson-end\n");
            return false;
        }
    }
    else
    {
        DEBUG_MSG("#__ Failed to mount FS\n");
        DEBUG_MSG("#<< readCustomConfigJson-end\n");
        return false;
    }
}

void WiFiMan::disableMqttConfig()
{
    MQTT = false;
}


void WiFiMan::setConfigPin(int pinNumber,bool activeState)
{
    _configPin = pinNumber;
    _configPinActiveState = activeState;
    if(_configPin >= 0)
    {
        if(activeState)
            pinMode(_configPin,INPUT);
        else
            pinMode(_configPin,INPUT_PULLUP);
    }
}

//handle config button
bool WiFiMan::handleConnectInterrupt()
{
    if(_configPin >= 0)
        if(digitalRead(_configPin) == _configPinActiveState)
            return true;
    return false;
}

void WiFiMan::setIndicatorLedPin(int pinNumber,bool onState)
{
    _indicatorLedPin = pinNumber;
    _indicatorLedOnState = onState;
    pinMode(pinNumber,OUTPUT);
    digitalWrite(pinNumber,!onState);
}

void WiFiMan::setLedState(bool state)
{
    if(_indicatorLedPin>-1)
        if(state)
            digitalWrite(_indicatorLedPin,_indicatorLedOnState);
        else
            digitalWrite(_indicatorLedPin,!_indicatorLedOnState);
}

void WiFiMan::handleIndicatorLed()
{
    //ledBlinkInterval
    if(_indicatorLedPin>-1)
    {
        if(millis()-ledTimer > ledBlinkInterval)
        {
            ledTimer = millis();
            setLedState(!ledState);
            ledState = !ledState;
        }
    }
}

void WiFiMan::setConnectDelay(unsigned int delayms)
{
    _connect_delay = delayms;
}

void WiFiMan::resetLedState()
{
    if(_indicatorLedPin != -1)
        pinMode(_indicatorLedPin,INPUT);
}


void WiFiMan::setExtFunc(void (*f)(void))
{
    extFuncEnabled = true;
    extFunc = f;
}

void WiFiMan::handleExtFunc()
{
    if(extFuncEnabled)
        extFunc();
}
