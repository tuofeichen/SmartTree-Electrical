#include <SPI.h>
#include <SdFat.h>
#include <UTFT.h>
#include <UTFT_SdRaw.h>

#include "TFT.h"

// Font definition
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

extern SdFat sd;
extern SdFile file;
extern UTFT GLCD;
extern UTFT_SdRaw files;

void initSD() {
  while (!Serial) { }
  Serial.println(F("Initialising SD card..."));
  bool mysd = 0;
  // see if the card is present and can be initialized:
  while (!mysd) {
    if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) {
      Serial.println(F("Card failed, or not present"));
    } else {
      mysd = 1;
      Serial.println(F("Card initialised."));
    }
  }
}

void initLCD() {
  Serial.println(F("Initialising LCD."));
  GLCD.InitLCD();
  GLCD.clrScr();
  GLCD.setFont(SmallFont);

  // Draw Background
  char fname440[] = "sc.RAW";
  GLCD.clrScr();
  files.load(0, 0, 800, 480, fname440, 1);
  Serial.println(F("LCD Initialised."));
}

void updateBattery(int chargePercent) {
  SET_WHITE();
  GLCD.fillRect(BATX, BATY, BATX + 100, BATY + 17);
  // Battery Icon Color Change
  if (chargePercent <= 20) {
    GLCD.setColor(VGA_RED);
  } else if (chargePercent >= 40) { 
    GLCD.setColor(VGA_GREEN);
  } else {
    SET_ORANGE();
  }
  // Update Icon
  GLCD.fillRect(BATX, BATY, BATX + chargePercent, BATY + 17);
}

void updateEnergyBars(unsigned int *values, unsigned int length) {
  int yAxisScale = 0;
  
  // get largest value for scaling the graph
  for(int i = 0; i < length; i++) {
    if(yAxisScale < values[i]) yAxisScale = values[i];
  }
   
  if(yAxisScale < 20) yAxisScale = 20;     // prevent Y-axis scale from being too small
  yAxisScale = (yAxisScale + 9) / 10 * 10; // round to the next 10
  
  clearEnergyBars();
  SET_BLUE();
  
  for (int i = 0; i < length; i++) {
    GLCD.fillRect(PBX + PBW * i, PBY, PBX + PBSLOT + PBW * i, PBY - (values[i] * MAX_BAR_HEIGHT) / yAxisScale);
  }
  
  // write axis label
  GLCD.setFont(SmallFont);
  GLCD.setBackColor(VGA_WHITE);
  GLCD.setColor(VGA_BLACK);
  GLCD.printNumI(yAxisScale, 30, 255);
}

void clearEnergyBars() {
  GLCD.setColor(VGA_WHITE);
  GLCD.fillRect(PBX, PBY, 300, PBLIMIT);
}

void updateScreenValues(int powerIn, int oldEnergy, int powerOut) {
  GLCD.setFont(BigFont);
  GLCD.setBackColor(VGA_WHITE);
  GLCD.setColor(VGA_GREEN);
  GLCD.print("    ", 260, 140);
  GLCD.print("    ", 260, 173);
  GLCD.print("    ", 260, 203);
  GLCD.printNumI(oldEnergy, 260, 140);
  GLCD.printNumI(powerIn, 260, 173);
  GLCD.printNumI(powerOut, 260, 203);
}

void drawErrorBox() {
  GLCD.setColor(VGA_BLACK);
  GLCD.drawRect(100, 100, 750, 380);
  GLCD.drawRect(101, 101, 749, 379);
  GLCD.setColor(VGA_YELLOW);
  GLCD.fillRect(101, 101, 749, 379);
  GLCD.setColor(VGA_RED);
  GLCD.setFont(BigFont);
  GLCD.setBackColor(VGA_TRANSPARENT);
}

void setupPrintMinorError() {
  GLCD.setColor(VGA_BLACK);
  GLCD.setFont(BigFont);
  GLCD.setBackColor(VGA_TRANSPARENT);
}

