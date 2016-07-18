#ifndef  SCREEN_H
#define  SCREEN_H

#include <rtc_clock.h>
#include "TFT.h"  
#include "log.h"
#include "globals.h"

inline void initScreen() {
  //initSD();
  //initLCD(); 
  initRTC();
}

inline void updateScreen() {
  updateBattery();
  oldEnergy = updateScreenValues(voltage * currentIn, voltage * currentOut);
}

inline void updateScreenUponDateChange() { // replace this function when sleep mode is completed
  static int oldDay = 0;
  int NewDay = getVirtualDay(); // a day may be shortened for testing purposes
  if (NewDay != oldDay) { 
    Serial.println(F("Updating Graph"));
    updateEnergyBars();
    updateFlash();
    oldEnergy = 0; 
  }
   oldDay = NewDay;
}

#endif
