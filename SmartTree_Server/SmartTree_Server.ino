// latest version

#include <SPI.h>
#include <SdFat.h>
#include <UTFT.h>
#include <UTFT_SdRaw.h>
#include <rtc_clock.h>

#include <DueFlashStorage.h>
#include <efc.h>
#include <flash_efc.h>

#include <math.h>
#include <DueTimer.h>
#include "screen.h"
#include "constants.h"
#include "bmucell.h"
#include "util.h"
#include "TFT.h"
#include "log.h"
#include "error.h"
#include "transmitter.h"
//#include "debug.h"

#define NUM_CELL 1

volatile Cell cells[] = {
  Cell(0), Cell(1), Cell(2), Cell(3)
};

//SdFat sd;
//SdFile file;
//UTFT GLCD(CTE70, 25, 26, 27, 28);
//UTFT_SdRaw files(&GLCD);

DueFlashStorage dueFlashStorage; // consider switch to SD
RTC_clock rtc_clock(RC);

byte mode = 0;

// TODO change to doubles - change in globals.h too
double voltage = 0;
double currentIn = 0;
double currentOut = 0;
double oldEnergy = 0;

byte energyAddress = 0;
int oldTime = 0;
float totalEnergy = 0;

byte preventRefresh = 0;

volatile bool logDataFlag = false, updateScreenFlag = false;
volatile byte logDataCurrentCell = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  initScreen();
  Cell::setup();

//  writeLogFileHeader(LOG_FILE_NAME);

  // TODO fine tune timing
  // TODO the interval for whichever interrupt updates the energy should be made a constant

  // TODO set priorities for ints
  Timer.getAvailable().attachInterrupt(setLogDataFlag).start      (1000000);
  Timer.getAvailable().attachInterrupt(setUpdateScreenFlag).start (2000000);
  Timer.getAvailable().attachInterrupt(globalBatteryCheck).start  (100000);
}

void loop() {
  if(IN_NORMAL_MODE_STRICT(mode)) {
//    Serial.println("BMS in Normal Mode");
    preventRefresh = 0;
    if(updateScreenFlag) {
      updateScreenFlag = false;
      cells[logDataCurrentCell].logData(Serial); // print out cell data
      updateScreen();
      updateScreenUponDateChange();
    }

  } else if(IN_ERROR_MODE_STRICT(mode)) {
//    Serial.println("BMS in Error Mode");
    if(!displayError(cells, NUM_CELL) && updateScreenFlag) { // if big screen is not
      updateScreenFlag = false;
      updateScreen();
      updateScreenUponDateChange();
    }
  } else if(IN_SLEEP_MODE_STRICT(mode)) {
    transmitMessage(SLEEP);
  } else if(IN_DEBUG_MODE_STRICT(mode)) {
    // start or continue debug interface
  }

  if(logDataFlag) {
    logDataFlag = false;
    printDate(Serial);                         // print out date
    Serial.println();
    printTime(Serial);                         // print out time
    Serial.println();
//    cells[logDataCurrentCell].logData(Serial); // print out cell data

    if(IS_FLAG_SET(mode, ERROR_FLAG)) {        // pritn out error
      cells[logDataCurrentCell].logErrors(Serial);
    }
    
    transmitLogData(logDataCurrentCell, cells[logDataCurrentCell].getVoltage(), cells[logDataCurrentCell].getCurrentIn(),
                cells[logDataCurrentCell].getCurrentOut(), cells[logDataCurrentCell].getTemperature(),
                cells[logDataCurrentCell].getErrorFlags());
    // transmit to the screen
  }
  
}

void globalBatteryCheck() {
  double totalVoltage = 0.0;
  double totalCurrentIn = 0.0;
  double totalCurrentOut = 0.0;
  bool allCellsNormal = true;
  for(int i = 0; i < NUM_CELL; i++) {
    cells[i].update();

    if(cells[i].checkErrors()) {
      allCellsNormal = false;
      SET_FLAG(mode, ERROR_FLAG);
    }
    totalVoltage += cells[i].getVoltage();
    totalCurrentIn += cells[i].getCurrentIn();
    totalCurrentOut += cells[i].getCurrentOut();
  }
  if(allCellsNormal) {
    CLEAR_FLAG(mode, ERROR_FLAG);
  }

  // TODO change globals to doubles
  voltage = (totalVoltage / NUM_CELL);
  currentIn =   totalCurrentIn;
  currentOut =  totalCurrentOut;
}


void setUpdateScreenFlag() {
  updateScreenFlag = true;
}

void setLogDataFlag() {
  if(logDataFlag) return;
  logDataFlag = true;
  logDataCurrentCell++;
  if(logDataCurrentCell == NUM_CELL) {
    logDataCurrentCell = 0;
  }
}
