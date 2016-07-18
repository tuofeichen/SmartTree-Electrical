#ifndef TFT_H
#define TFT_H

#define SD_CHIP_SELECT  53  // SD chip select pin

// Color Setting
#define SET_RED() GLCD.setColor(255,0,0)
#define SET_ORANGE() GLCD.setColor(255,153,51)
#define SET_BLUE() GLCD.setColor(51,204,255)
#define SET_GREEN() GLCD.setColor(51,153,51)
#define SET_YELLOW() GLCD.setColor(255,255,0)
#define SET_BLACK() GLCD.setColor(0,0,0)
#define SET_WHITE() GLCD.setColor(255,255,255)

// Icon Location
#define BATX 175
#define BATY 20

#define PBX 65
#define PBY 380
#define PBW 28
#define PBLIMIT 265
#define PBSLOT 18
#define MAX_BAR_HEIGHT (PBY - PBLIMIT)

void initSD();
void initLCD();
void updateBattery(int chargePercent);
void updateEnergyBars(unsigned int *bars, unsigned int length);
void updateScreenValues(int powerIn, int oldEnergy, int powerOut);
void clearEnergyBars();
void drawErrorBox();
void setupPrintMinorError();
#endif
