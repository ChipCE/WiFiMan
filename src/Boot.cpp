#include "Boot.h"

void reboot()
{
    ESP.restart();
}

bool rebootToApMode()
{
    if(SPIFFS.begin())
    {
        File configFile = SPIFFS.open("/boot.conf", "w");
        if (!configFile) 
        {
            SPIFFS.end();
            return false;
        }

        configFile.print("skip-auto-connect");
        configFile.close();
        SPIFFS.end();
        ESP.restart();
        //program will not reach this return :3
        return true;
    }
    else
    {
        return false;
    }
}

bool clear()
{

    if(SPIFFS.begin())
    {
        if(!SPIFFS.exists("/config.json"))
            return true;

        if(SPIFFS.remove("/config.json"))
            return true;
        else
            return false;
    }
    else
        return false;
}