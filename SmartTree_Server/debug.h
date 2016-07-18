#ifndef DEBUG_H
#define DEBUG_H

#include <arduino.h>
#include <rtc_clock.h>

#include "util.h"

bool pollDebugPin();
void enterDebugMode(Print &out);
void debug(Stream& in, Print& out);

#endif
