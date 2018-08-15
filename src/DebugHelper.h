#ifndef __DEBUG_HELPER_H
#define __DEBUG_HELPER_H
#include "Arduino.h"
#include <ArduinoJson.h>

class DebugHelper
{
    private:
        int callLevel;
        bool DEBUG;
    public:
        DebugHelper();
        DebugHelper(bool enable);
        //void print(String str,bool );
        //void print(const char *str);
        void println(String msg);
        void println(const char *msg);
        void printJson(JsonObject& json);

        void printFunctionCall(String functionName);
        void printFunctionCall(const char *functionName);
        void printSingleFuntionCall(String functionName);
        void printSingleFunctionCall(const char *functionName);

        void printLastMsg(String msg);
        void printLastMsg(const char *msg);
};

#endif