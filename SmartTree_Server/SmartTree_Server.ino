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


DueFlashStorage dueFlashStorage; // Due own flash storage for the energy bar
RTC_clock rtc_clock(XTAL);       // Due internal RTC for counting date

byte mode = 0;                   // state machine (normal, error, sleep, debug) // sleep and debug not implemented. 


// global variables for the sensor value (voltage is the average, currentIn, currentOut are sums)
double voltage = 0;
double currentIn = 0;
double currentOut = 0;
double oldEnergy = 0;


byte energyAddress = 0; // address in the Due flash for storing energy bar value 
int oldTime = 0;    

float totalEnergy = 0;  // total energy generated to date (don't know why is a float here)

byte preventRefresh = 0; // flag to prevent refreshing in the case of an error 

volatile bool logDataFlag = false, updateScreenFlag = false; // volatile variables for trigger interrupt 

volatile byte logDataCurrentCell = 0;

void setup() {
  Serial.begin(115200);   // serial debug interface
   
  Serial1.begin(9600); // RS232 Screen communication interface
  
  initScreen();        // initialize screen
  transmitClear();     // initialize transmitter (reload screen background)
  
  Cell::setup();       // set up ADC pin from Analog Mux
  
  // TODO set priorities for ints
  Timer.getAvailable().attachInterrupt(setLogDataFlag).start      (1000000); // trasnmit log data to the screen
  Timer.getAvailable().attachInterrupt(setUpdateScreenFlag).start (2000000); // transmit display data to the screen
  Timer.getAvailable().attachInterrupt(globalBatteryCheck).start  (100000);  // BMS check

}

void loop() {
  
  if(IN_NORMAL_MODE_STRICT(mode)) {
//    Serial.println("BMS in Normal Mode");

    if (preventRefresh) // if last mode is error, clear preventRefresh flag, refresh screen 
    {
        transmitClear();
        preventRefresh = 0;
    }
    if(updateScreenFlag) { // if timer interrupt is triggered
      updateScreenFlag = false;
//      cells[logDataCurrentCell].logData(Serial); // print out cell data
      
      updateScreen(); // update screen value 
      updateScreenUponDateChange(); // update energy bars
    }

  } else if(IN_ERROR_MODE_STRICT(mode)) {
//    Serial.println("BMS in Error Mode");
    if(!displayError(cells, NUM_CELL) && updateScreenFlag) { // display error once (displayError is the error handling function) 
      updateScreenFlag = false;
      updateScreen();
      updateScreenUponDateChange();
    }
  } else if(IN_SLEEP_MODE_STRICT(mode)) {
    transmitMessage(SLEEP);
  } else if(IN_DEBUG_MODE_STRICT(mode)) {
    // start or continue debug interface
  }

  if(logDataFlag) { // if log data timer interrupt is triggered 
  
  // serial debug
    logDataFlag = false;
    printDate(Serial);                         // print out date
    Serial.println();
    printTime(Serial);                         // print out time
    Serial.println();
    cells[logDataCurrentCell].logData(Serial); // print out cell data

    if(IS_FLAG_SET(mode, ERROR_FLAG)) {        // pritn out error
      cells[logDataCurrentCell].logErrors(Serial);
    }
    
    // actual transmission
    transmitLogData(logDataCurrentCell, cells[logDataCurrentCell].getVoltage(), cells[logDataCurrentCell].getCurrentIn(),
                cells[logDataCurrentCell].getCurrentOut(), cells[logDataCurrentCell].getTemperature(),
                cells[logDataCurrentCell].getErrorFlags());
    // transmit to the screen
  }
  
}

// BMS check interrupt
void globalBatteryCheck() {
  double totalVoltage = 0.0;
  double totalCurrentIn = 0.0;
  double totalCurrentOut = 0.0;
  
  bool allCellsNormal = true;
  for(int i = 0; i < NUM_CELL; i++) {
    cells[i].update(); // read in new sensor for all of them

    if(cells[i].checkErrors()) { // check errors
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
