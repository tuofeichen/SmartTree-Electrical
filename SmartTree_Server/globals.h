#ifndef GLOBALS_H
#define GLOBALS_H

#include <arduino.h>
#include <SdFat.h>
#include <UTFT.h>
#include <UTFT_SdRaw.h>
#include <rtc_clock.h>
#include <DueFlashStorage.h>
#include <efc.h>
#include <flash_efc.h>

#include "util.h"

//extern SdFat sd;
//extern SdFile file;
//extern UTFT GLCD;
//extern UTFT_SdRaw files;
extern DueFlashStorage dueFlashStorage; // consider switch to SD
extern RTC_clock  rtc_clock;

extern byte mode;

extern double voltage;
extern double currentIn;
extern double currentOut;
extern double oldEnergy;

extern byte energyAddress;
extern int oldTime;
extern int totalEnergy;

extern byte preventRefresh;

// Font definition
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

#endif
