#include <SPI.h>
#include <SdFat.h>
#include <UTFT.h>
#include <UTFT_SdRaw.h>

#include "receiver.h"
#include "tft.h"
#include "log.h"
#include <stream.h>
#include <string.h>
#include <stdlib.h>


SdFat sd;
SdFile file;
UTFT GLCD(CTE70, 25, 26, 27, 28);
UTFT_SdRaw files(&GLCD);

Receiver r(Serial/*Change to Serial1 after debugging*/, "BCDEeLMNnRSW"/*these are the valid commands*/);

unsigned int energyBars[8] = {0};
unsigned int energyBarLength = 0;
unsigned int batteryMeter = 0;
int powerIn, oldEnergy, powerOut;

bool isCardPresent = false;

char fname440[] = "sc.RAW";

char *logFileNames[4] = { "Cell_0.csv", "Cell_1.csv", "Cell_2.csv", "Cell_3.csv" };

// TODO write function to write headers for log files if log files are not present

void setup() {
  Serial.begin(19200);
  Serial.setTimeout(2000);
  SDInit();
  initLCD();
  //Serial.begin(9600);
  //Serial.setTimeout(2000);
  
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  r.reply(REPLY_READY);
}

void loop() {
  
  if(r.receiveData()) {
    r.reply(REPLY_SUCCESS);
    r.reply(REPLY_BUSY);
    execute();
    r.reply(REPLY_READY);
  } else {
    r.reply(REPLY_FAILED);
  }
  
  
}


void execute() {
  switch(r.type()) {
    case 'B': // draw energy bar
      energyBarLength = min(r.size(), 8);
      for(int i = 0; i < energyBarLength; i++) {
        energyBars[i] = atoi(r[i]);
      }
      updateEnergyBars(energyBars, energyBarLength);
      break;
    case 'C': // clear notice msg area
      GLCD.setColor(VGA_WHITE);
      GLCD.fillRect(570, 200, 799, 280);
      break;
    case 'D': // draw normal screen data
      if(r.size() != 3) break;
      powerIn = atoi(r[0]);
      oldEnergy = atoi(r[1]);
      powerOut = atoi(r[2]);
      updateScreenValues(powerIn, oldEnergy, powerOut);
      break;
    case 'E': // draw error box and msg
      drawErrorBox();
      for(int i = 0; i < r.size(); i++) {
         GLCD.print(r[i], 120, 120 + 32 * i);
      }
      break;
      // TODO error numbers instead of msgs
    case 'L':
      if(r.size() != 2) break;
      logToFile(logFileNames[atoi(r[0])], r[1]);
      break;
    case 'M': // draw battery meter
      if(r.size() != 1) break;
      batteryMeter = atoi(r[0]);
      updateBattery(batteryMeter);
      break;
    case 'N': // draw notice message
      setupPrintMinorError();
      for(int i = 0; i < r.size(); i++) {
         GLCD.print(r[i], 570, 200 + 20 * i);
      }
      break;
    case 'R': // refresh normal screen
      files.load(0, 0, 800, 480, fname440, 1);
      updateScreenValues(powerIn, oldEnergy, powerOut);
      updateEnergyBars(energyBars, energyBarLength);
      updateBattery(batteryMeter);
      break;
    case 'S': // sleep
      digitalWrite(9, LOW);
      break;
    case 'W': // wake
      digitalWrite(9, HIGH);
      break;
  }
}
