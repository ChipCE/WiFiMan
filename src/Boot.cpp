#include "Boot.h"

void reboot()
{
    ESP.restart();
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
    {
        DEBUG_MSG("#>< Clear : SPIFFS Error!\n");
        return false;
    }
}

bool makeBootFlag()
{
    if(SPIFFS.begin())
    {
        File configFile = SPIFFS.open("/boot.conf", "w");
        if (!configFile) 
        {
            DEBUG_MSG("#>< makeBootFlag : SPIFFS Error! Cannot open /boot.conf\n");
            SPIFFS.end();
            return false;
        }
        else
        {
            configFile.print("skip-auto-connect");
            configFile.close();
            SPIFFS.end();
            return true;
        }
    }
    else
    {
        DEBUG_MSG("#>< makeBootFlag : SPIFFS Error!\n");
        return false;
    }
}

bool clearBootFlag()
{
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
            return true;
        }
    }
    else
    {
        DEBUG_MSG("#>< clearBootFlag : SPIFFS Error!\n");
        return false;
    }
}

bool getBootFlag()
{
    if(SPIFFS.begin())
    {
        if (SPIFFS.exists("/boot.conf")) 
        {
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
        DEBUG_MSG("#>< getBootFlag : SPIFFS Error!\n");
        return false;
    }
}

// ------------------------------------------------------ 

bool getBootMode()
{
    if(getBootFlag())
    {
        //remove the boot flag
        clearBootFlag();
        return true;
    }
    return false;
}

bool rebootToApMode()
{
    //make boot flag
    makeBootFlag();
    //then reboot
    reboot();
}