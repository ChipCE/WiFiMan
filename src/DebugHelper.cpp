#include "DebugHelper.h"

DebugHelper::DebugHelper()
{
    Serial.begin(115200);
    callLevel = -1;
    DEBUG = true;
}

DebugHelper::DebugHelper(bool enable)
{
    if(enable)
    {
        Serial.begin(115200);
    }
    DEBUG = enable;
    callLevel = -1;
}

//void DebugHelper::print(String str){}

//void DebugHelper::print(const char *str){}

void DebugHelper::println(String msg)
{
    if(DEBUG)
    {
        String debugMsg = "#  ";
        for(int i=0;i<=callLevel;i++)
            debugMsg += "  ";
        debugMsg += msg;
        Serial.println(debugMsg);
    }
}

void DebugHelper::println(const char *msg)
{
    if(DEBUG)
    {
        String debugMsg = "#  ";
        for(int i=0;i<=callLevel;i++)
            debugMsg += "  ";
        debugMsg += msg;
        Serial.println(debugMsg);
    }
}

void DebugHelper::printJson(JsonObject &json)
{
    if(DEBUG)
    {
        printFunctionCall("printJson");
        if(json.success())
        {
            String spacer = "\n#  ";
            for(int i=0;i<=callLevel;i++)
                spacer += "  ";
            String str;
            json.printTo(str);
            str.replace("{",spacer);
            str.replace(",",spacer);
            str.replace("}","");
            println(str);
        }
        else
        {
            printLastMsg("Cannot parse json");
            return;
        }
        printLastMsg("printJson-end");
    }
}

void DebugHelper::printFunctionCall(String functionName)
{
    callLevel++;
    if(DEBUG)
    {
        String debugMsg = "#>>";
        for(int i=0;i<callLevel;i++)
            debugMsg += "  ";
        debugMsg += functionName;
        debugMsg += " is called.";
        Serial.println(debugMsg);
    }
}

void DebugHelper::printFunctionCall(const char *functionName)
{
    callLevel++;
    if(DEBUG)
    {
        String debugMsg = "#>>";
        for(int i=0;i<callLevel;i++)
            debugMsg += "  ";
        debugMsg += functionName;
        debugMsg += " is called.";
        Serial.println(debugMsg);
    }
}

void DebugHelper::printLastMsg(String msg)
{
    if(DEBUG)
    {
        String debugMsg = "#<<";
        for(int i=0;i<=callLevel;i++)
            debugMsg += "  ";
        debugMsg += msg;
        Serial.println(debugMsg);
    }
    callLevel--;
}

void DebugHelper::printLastMsg(const char *msg)
{
    if(DEBUG)
    {
        String debugMsg = "#<<";
        for(int i=0;i<=callLevel;i++)
            debugMsg += "  ";
        debugMsg += msg;
        Serial.println(debugMsg);
    }
    callLevel--;
}

void DebugHelper::printSingleFuntionCall(String functionName)
{
    callLevel++;
    if(DEBUG)
    {
        String debugMsg = "#><";
        for(int i=0;i<callLevel;i++)
            debugMsg += "  ";
        debugMsg += functionName;
        debugMsg += " is called.";
        Serial.println(debugMsg);
    }
    callLevel--;
}

void DebugHelper::printSingleFunctionCall(const char *functionName)
{
    callLevel++;
    if(DEBUG)
    {
        String debugMsg = "#><";
        for(int i=0;i<callLevel;i++)
            debugMsg += " ";
        debugMsg += functionName;
        debugMsg += " is called.";
        Serial.println(debugMsg);
    }
    callLevel--;
}