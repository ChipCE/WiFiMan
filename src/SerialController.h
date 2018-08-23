#ifndef __SERIAL_CONTROLLER_H
#define __SERIAL_CONTROLLER_H

#include <Arduino.h>
#include "Boot.h"

class SerialController
{
    private:
        String serialBuffer;
        bool ENABLE;
    public:
        SerialController();
        SerialController(bool enable);
        SerialController(bool enable,bool ignoreInvalid);
        void handleSerial();
        bool executeCommand();
};


#endif

