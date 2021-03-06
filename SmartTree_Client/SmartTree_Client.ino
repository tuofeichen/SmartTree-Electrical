#include <SPI.h>
#include <SdFat.h>
#include <UTFT.h>
#include <UTFT_SdRaw.h>
#include <DueTimer.h>
#include "receiver.h"
#include "tft.h"
#include "log.h"
#include <stream.h>[p]
#include <string.h>
#include <stdlib.h>

SdFat sd;
SdFile file;
UTFT GLCD(CTE70, 25, 26, 27, 28);
UTFT_SdRaw files(&GLCD);

Receiver r(Serial1, "BCDEeLMNnRSW"/*these are the valid commands*/);

unsigned int energyBars[8] = {0};
unsigned int energyBarLength = 1;
unsigned int batteryMeter = 0;
int powerIn, oldEnergy, powerOut, totalEnergy;

bool isCardPresent = false;
char fname440[] = "smarttree.raw";
char *logFileNames[4] = { "Cell_0.csv", "Cell_1.csv", "Cell_2.csv", "Cell_3.csv" };

volatile int hb = 0;
volatile int hb_prev = 0;


// TODO write function to write headers for log files if log files are not present
void setup() {
  Serial.begin(19200);
  Serial1.begin(9600);
  Serial.setTimeout(2000);
//  SDInit();
  initSD();
  initLCD();
// watchdog timer to be 4.5 secs
  uint32_t wdp_ms = 1152 ;
  WDT_Enable( WDT, 0x2000 | wdp_ms | ( wdp_ms << 16 ));
//
  pinMode(9, OUTPUT);   //  screen on /off pin
  digitalWrite(9, HIGH);

  pinMode(14, OUTPUT);  //  shutdown pin
  digitalWrite(14, HIGH);
  r.reply(REPLY_READY);
}

void loop() {
  WDT_Restart(WDT);
   Serial.println("Start receiving message");
//  watchdogReset();

  if (r.receiveData()) {
    Serial.println("Get some data") ;
    r.reply(REPLY_SUCCESS);
    r.reply(REPLY_BUSY);
    execute();
    r.reply(REPLY_READY);
  }
  else {
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
      GLCD.clrScr();
      files.load(0, 0, 800, 480, "sc.RAW", 1);
      Serial.println(F("LCD Initialised."));
      break;
    case 'D': // draw normal screen data
      if(r.size() != 4) break;
      powerIn = atoi(r[0]);
      oldEnergy = atoi(r[1]);
      powerOut = atoi(r[2]);
      totalEnergy = atoi(r[3]);
      updateScreenValues(powerIn, oldEnergy, powerOut,totalEnergy);
      break;

    case 'E': // draw error box and msg
      drawErrorBox();
      for(int i = 0; i < r.size(); i++) { // print out error message
         char* temp = new char [strlen(r[i])];
         strcpy(temp, r[i]);
         int len = strlen(temp);
         temp[len-1] = ' '; //(null terminating string produce random characters (already handled on the UTFT side)
         GLCD.print(temp, 120, 120 + 32 * i);
      }
      break;
    case 'L':
      if(r.size() != 2) break;
      logToFile(atoi(r[0]), r[1]);
      break;
    case 'M': // draw battery meter
      if(r.size() != 1) break;
      batteryMeter = atoi(r[0]);
      updateBattery(batteryMeter);
      break;
    case 'N': // draw notice message
      setupPrintMinorError();
      for(int i = 0; i < r.size(); i++) {
         char* temp = new char [strlen(r[i])];
         strcpy(temp, r[i]);
         int len = strlen(temp);
         temp[len-1] = ' '; //(null terminating string produce random characters (already handled on the UTFT side)
         GLCD.print(r[i], 570, 200 + 20 * i);
      }
      break;
    case 'R': // refresh normal screen
      files.load(0, 0, 800, 480, fname440, 1);
      updateScreenValues(powerIn, oldEnergy, powerOut, totalEnergy);
      updateEnergyBars(energyBars, energyBarLength);
      updateBattery(batteryMeter);
      break;
    case 'S': // sleep
      digitalWrite(9, LOW); // chip enable pin
      break;
    case 'W': // wake
      digitalWrite(9, HIGH);
      break;
  }
}
