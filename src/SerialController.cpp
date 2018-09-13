#include "SerialController.h"

SerialController::SerialController()
{
    ENABLE = true;
    if(ENABLE)
    {
        Serial.begin(115200);
        serialBuffer = "";
        //clear unwanted buffer
        Serial.flush();
        while(Serial.available() > 0)
            char t = Serial.read();
    }
}

SerialController::SerialController(bool enable)
{
    ENABLE = enable;
    if(ENABLE)
    {
        Serial.begin(115200);
        serialBuffer = "";
        //clear unwanted buffer
        Serial.flush();
        while(Serial.available() > 0)
            char t = Serial.read();
    }
}


void SerialController::handleSerial()
{
    if(ENABLE)
    {
        //append serial characters to line
        while (Serial.available() > 0)
        {
            char recievedChar = Serial.read();
            serialBuffer += recievedChar; 

            // Process message when new line character is recieved
            if ((recievedChar == '\n') || (recievedChar == '\r'))
            {
                serialBuffer += '\0';
                executeCommand();
                serialBuffer = ""; 
            }
        }
    }
}

bool SerialController::executeCommand()
{
    if(strncmp(serialBuffer.c_str(),"#$> reboot",10)==0)
    {
        Serial.println("#$< Execute reboot command");
        serialBuffer = "";
        ESP.restart();
        return true;
    }

    if(strncmp(serialBuffer.c_str(),"#$> config",10)==0)
    {
        Serial.println("#$< Reboot to config mode");
        serialBuffer = "";
        rebootToApMode();
        return true;
    }

    if(strncmp(serialBuffer.c_str(),"#$> clear",9)==0)
    {
        Serial.println("#$< Clear all setting and reboot");
        serialBuffer = "";
        clear();
        ESP.restart();
        return true;
    }

    if(strncmp(serialBuffer.c_str(),"#$> status",10)==0)
    {
        Serial.println("#$< Generic IPMI : Config mode");
        serialBuffer = "";
        return true;
    }

    Serial.println("#$< Invalid command");
    serialBuffer = "";
    return false;
}