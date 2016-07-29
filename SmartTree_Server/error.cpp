#include "error.h"
#include "constants.h"
#include "globals.h"
#include "transmitter.h"
const char *OverheatingString = "Our batteries are overheating!";
const char *SorryString = "Sorry for the inconvenience.";
const char *LowBatteryString = "Low battery. We cannot charge your device.";
const char *CannotChargeString = LowBatteryString + 13; // "We cannot charge your device"

char partialChargingString[] = "*/4 cells have";
char partialChargingString2[] = "stopped charging.";
char undervoltageString[] = "Operating at";
char undervoltageString2[] = "*/4 capacity";
char overloaded2[] = "The Smart Tree is **% overloaded.";
char overloaded3[] = "The Smart Tree is ***% overloaded.";
/*
void drawErrorBox() {
  GLCD.setColor(VGA_BLACK);
  GLCD.drawRect(100, 100, 750, 380);
  GLCD.setColor(VGA_YELLOW);
  GLCD.fillRect(110, 110, 740, 370);
  GLCD.setColor(VGA_RED);
  GLCD.setFont(BigFont);
  GLCD.setBackColor(VGA_TRANSPARENT);
}

void setupPrintMinorError() {
  GLCD.setColor(VGA_BLACK);
  GLCD.setFont(SmallFont);
  GLCD.setBackColor(VGA_TRANSPARENT);
}
*/

#define TEMP_ERROR_H            1
#define UNDERVOLTAGE_ERROR_H    2
#define TEMP_ERROR_L            3
#define OVERCURRENT_OUT_ERROR_H 4
#define OVERCURRENT_OUT_ERROR_L 5
#define UNDERVOLTAGE_ERROR_L    6
#define CHARGE_ERROR            7


bool displayError(volatile Cell cells[], int numel) {

  int temperatureCount = 0;
  int undervoltageCount = 0;
  int overcurrentOutCount = 0;
  int chargingCount = 0;

  for(int i = 0; i < numel; i++) {
    int flag = cells[i].getErrorFlags();
    if(IS_FLAG_SET(flag, TEMPERATURE_ERROR)) temperatureCount++;
    if(IS_FLAG_SET(flag, LOW_VOLTAGE_ERROR)) undervoltageCount++;
    if(IS_FLAG_SET(flag, HIGH_CURRENT_OUT_ERROR)) overcurrentOutCount++;
    if(IS_FLAG_SET(flag, HIGH_VOLTAGE_ERROR) ||
       IS_FLAG_SET(flag, HIGH_CURRENT_IN_ERROR)) chargingCount++;
  }

  // all four has issues
  if(temperatureCount == 4) {
    if ((preventRefresh != TEMP_ERROR_H)) { // debug transmission 
    
      preventRefresh = TEMP_ERROR_H;
      transmitErrorNumber(TEMP_ERROR_H);
//      transmitErrorMessage(3, OverheatingString, CannotChargeString, SorryString);
//      drawErrorBox();
//      GLCD.print(OverheatingString, 120, 120);
//      GLCD.print(CannotChargeString, 120, 150);
//      GLCD.print(SorryString, 120, 180);
    }
    return true;
  } else if(undervoltageCount == 4) {
    if(preventRefresh != UNDERVOLTAGE_ERROR_H) {
      preventRefresh = UNDERVOLTAGE_ERROR_H;
      transmitErrorNumber(UNDERVOLTAGE_ERROR_H);
//      drawErrorBox();
//      GLCD.print(LowBatteryString, 120, 120);
//      GLCD.print(SorryString, 120, 150);
    }
    return true;
  } else if(temperatureCount) {
    if(preventRefresh != TEMP_ERROR_L) {
      preventRefresh = TEMP_ERROR_L;
      transmitErrorNumber(TEMP_ERROR_L);
//      drawErrorBox();
//      GLCD.print(OverheatingString, 120, 120);
//      GLCD.print("Charging is not recommended.", 120, 150);
//      GLCD.print(SorryString, 120, 180);
    }
    return true;
  } else if(overcurrentOutCount >= 3) {
    if(preventRefresh != OVERCURRENT_OUT_ERROR_H) {
      preventRefresh = OVERCURRENT_OUT_ERROR_H;
      transmitErrorNumber(OVERCURRENT_OUT_ERROR_H, overcurrentOutCount);
//      drawErrorBox();
//      overloaded3[18] = (overcurrentOutCount == 4) ? '1' : ' ';
//      overloaded3[19] = (overcurrentOutCount == 4) ? '0' : '7';
//      overloaded3[20] = (overcurrentOutCount == 4) ? '0' : '5';
//      GLCD.print(overloaded3, 120, 120);
//      GLCD.print("Please unplug all devices.", 120, 150);
//      GLCD.print("Normal operation is expected to resume shortly.", 120, 180);
    }
    return true;
  } else if(overcurrentOutCount) {
    if(preventRefresh != OVERCURRENT_OUT_ERROR_L) {
      preventRefresh = OVERCURRENT_OUT_ERROR_L;
      transmitErrorNumber(OVERCURRENT_OUT_ERROR_L, overcurrentOutCount);
//      drawErrorBox();
//      overloaded2[18] = (overcurrentOutCount == 2) ? '5' : '2';
//      overloaded2[19] = (overcurrentOutCount == 2) ? '0' : '5';
//      GLCD.print(overloaded2, 120, 120);
//      GLCD.print("Please unplug large devices such as laptop chargers.", 120, 150);
//      GLCD.print("Normal operation is expected to resume shortly.", 120, 180);
    }
    return true;
  } else if(undervoltageCount) {
    if(preventRefresh != UNDERVOLTAGE_ERROR_L) {
      preventRefresh = UNDERVOLTAGE_ERROR_L;
      transmitNoticeNumber(UNDERVOLTAGE_ERROR_L, undervoltageCount);
//      setupPrintMinorError();
//      undervoltageString2[0] = 4 - undervoltageCount + '0';
//      GLCD.print(undervoltageString, 380, 20);
//      transmitNoticeMessage(1, undervoltageString, undervoltageString2);
    }
    return false;
  } else if(chargingCount) {
    if(preventRefresh != CHARGE_ERROR) {
      preventRefresh = CHARGE_ERROR;
      transmitNoticeNumber(CHARGE_ERROR, chargingCount);
//      setupPrintMinorError();
//      partialChargingString[0] = chargingCount + '0';
//      GLCD.print(partialChargingString, 380, 20);
//      transmitNoticeMessage(2, partialChargingString, partialChargingString2);
    }
    return false;
  } else {
    preventRefresh = 0;
    return false;
  }
}
