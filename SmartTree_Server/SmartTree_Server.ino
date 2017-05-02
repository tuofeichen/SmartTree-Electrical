// latest version
#include <SPI.h>

// rtc include 
#include <rtc_clock.h>

// due flash storage 
#include <DueFlashStorage.h>
#include <efc.h>
#include <flash_efc.h>
#include <math.h>

// external library for interrupt 
#include <DueTimer.h>

// library we wrote 
#include "constants.h"
#include "bmucell.h"
#include "util.h"
#include "update.h"
#include "error.h"
#include "transmitter.h"
//#include "debug.h"


#define NUM_CELL 4

volatile Cell cells[] = {
  Cell(0), Cell(1), Cell(2), Cell(3)
};



DueFlashStorage dueFlashStorage; // Due own flash storage for the energy bar
RTC_clock rtc_clock(XTAL);       // Due internal RTC for counting date



// global variables for the sensor value (voltage is the average, currentIn, currentOut are sums)
double g_voltage = 0;
double g_currentIn = 0;
double g_currentOut = 0;


 
byte preventRefresh = 0; // flag to prevent refreshing in the case of an error 
byte mode = 0;                   // state machine (normal, error, sleep, debug) // sleep and debug not implemented. 


volatile bool logDataFlag = false;
volatile bool updateScreenFlag = false; // volatile variables for trigger interrupt 
volatile byte logDataCurrentCell = 0;

void setup() {
  Serial.begin(115200);   // serial debug interface
  Serial1.begin(9600); // RS232 Screen communication interface
  
  // initScreen();        // initialize screen
  initRTC(16,40,0,1,5,2017); // hh/mm/ss dd/mm/yr
  transmitClearMessage();     // initialize transmitter (reload screen background)
  
  // init BMS; 
  Cell::setup();       
  
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
        transmitClearMessage();
        preventRefresh = 0;
    }

    if(updateScreenFlag) { // if timer interrupt is triggered
      updateScreenFlag = false;    
      updateScreen(g_voltage,g_currentIn,g_currentOut);               // update screen value 
      updateScreenUponDateChange(); // update energy bars
    }

  } else if(IN_ERROR_MODE_STRICT(mode)) {

    // Serial.println("BMS in Error Mode");
    if(!displayError(cells, NUM_CELL) && updateScreenFlag) { // display error once (displayError is the error handling function) 
      updateScreenFlag = false;
      updateScreen(g_voltage,g_currentIn,g_currentOut);  
      updateScreenUponDateChange();
    }
    else if(updateScreenFlag)
    {
      updateScreenFlag = false;
      updateScreen(g_voltage,g_currentIn,g_currentOut);  
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
    // printDate(Serial);                         // print out date
    // Serial.println();
    // printTime(Serial);                         // print out time
    // Serial.println();
    cells[logDataCurrentCell].logData(Serial); // print out cell data

    if(IS_FLAG_SET(mode, ERROR_FLAG)) {        // print out error
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
  
  bool allCellsNormal = true;
  g_voltage    = 0;
  g_currentIn  = 0;
  g_currentOut = 0;

  for(int i = 0; i < NUM_CELL; i++) {
    cells[i].update(); // read in new sensor for all of them

    if(cells[i].checkErrors()) { // check errors
      allCellsNormal = false; 
      SET_FLAG(mode, ERROR_FLAG);
    }
    
    g_voltage    += cells[i].getVoltage();
    g_currentIn  += cells[i].getCurrentIn();
    g_currentOut += cells[i].getCurrentOut();
  }
  
  if(allCellsNormal) {
    CLEAR_FLAG(mode, ERROR_FLAG);
  }

  g_voltage = (g_voltage / NUM_CELL);
}


void setUpdateScreenFlag() {
  updateScreenFlag = true;
}

void setLogDataFlag() {
  if(logDataFlag) 
    return;
  logDataFlag = true;
  logDataCurrentCell++;
  if(logDataCurrentCell == NUM_CELL) {
    logDataCurrentCell = 0;
  }
  
}
