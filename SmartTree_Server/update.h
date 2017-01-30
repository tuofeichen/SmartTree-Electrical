#ifndef TFT_H
#define TFT_H

#define SD_CHIP_SELECT  53  // SD chip select pin
/*
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

void initLCD();
*/


void initRTC(int hour, int min, int sec, int day, int month, int year);
void updateBattery(double voltage);
void updateEnergyBars();
void updateFlash();
void updateScreen(double voltage, double currentIn, double currentOut);
void updateScreenUponDateChange() ; // replace this function when sleep mode is completed
int  updateScreenValues(int powerIn, int powerOut);

int getVirtualDay();

#endif
