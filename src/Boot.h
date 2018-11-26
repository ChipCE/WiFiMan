#ifndef __BOOT_H
#define __BOOT_H

#include <FS.h>
#include "DebugHelper.h"

/*
#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif
*/

bool clear();
void reboot();
bool makeBootFlag();
bool clearBootFlag();
bool getBootFlag();

//get the boot mode and reset flasg to default state
bool getBootMode();
bool rebootToApMode();
#endif