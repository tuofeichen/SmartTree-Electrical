#include <SPI.h>
//#include <SdFat.h>
//#include <UTFT.h>
//#include <UTFT_SdRaw.h>
#include <rtc_clock.h>

#include <DueFlashStorage.h>
#include <efc.h>
#include <flash_efc.h>

#include "update.h"
#include "constants.h"
#include "globals.h"
#include "transmitter.h"

// has to loop through the 

byte energyDueFlashAddress = 0; // address in the Due flash for storing energy bar value 
int  currTime = 0;        // global time record
double  dailyEnergy = 0;
double  totalEnergy = 0;  // total energy generated to date (don't know why is a float here)

void initRTC(int hour, int min, int sec, int day, int month, int year) {
  rtc_clock.init();
  rtc_clock.set_time(hour, min, sec);
  rtc_clock.set_date(day, month, year);
}

void updateBattery(double voltage) {
  // charge percent calculation is performed according to the datasheet
  int chargePercent = (int)(19.0476 * voltage - 657.1422 + 0.5);
  if(chargePercent > 100) chargePercent = 100;
  if(chargePercent < 0) chargePercent = 0;
  transmitChargePercent(chargePercent); // send battery meter signal
}

void updateEnergyBars() {
  // first cycle means the flash memory is current incomplete; i.e. the first week
  static bool firstCycle = true;
  double values[9];
  Serial1.println(":B");
  if (energyDueFlashAddress < 8 && firstCycle) { // if there are not enough bars to fill the meter
    for(int i = 0; i < energyDueFlashAddress; i++) {
      values[i] = dueFlashStorage.read(i);
    }
    transmitEnergyBars(energyDueFlashAddress, values);
  } else {
    firstCycle = false;
    for(int i = 0; i < 8; i++) {
      values[i] = dueFlashStorage.read((energyDueFlashAddress - 8 + 50 + i) % 50); // wrap around, ensure positive
    }
    
    values[8] = dailyEnergy;    
//    Serial.println(dailyEnergy);
    
    transmitEnergyBars(9, values);
  }
}

void updateFlash() {
  
  dueFlashStorage.write(energyDueFlashAddress, dailyEnergy * 3600);
  energyDueFlashAddress++;
  if(energyDueFlashAddress > 50) {
    energyDueFlashAddress = 0;
  }
}

// TODO remove changing of global energy state

int updateScreenValues(int powerIn, int powerOut) {
  
  int sampleIntvl = rtc_clock.get_seconds() - currTime; // this should be the right time
  
  if (sampleIntvl < 0) {
    sampleIntvl += 60;
  }

  // Serial.println("Updating screen values!!");
  double energyDifference = (float) (powerIn - powerOut) * ((float)sampleIntvl / 3600.0);  // in Wh
  dailyEnergy += energyDifference;       //update daily energy difference
  totalEnergy += energyDifference;  //update total enegy difference
   
  if (powerIn< 20)
    transmitPowerData(0, dailyEnergy, powerOut, totalEnergy);
  else if(powerOut<20)
    transmitPowerData(powerIn, dailyEnergy, 0, totalEnergy);
  else
    transmitPowerData(powerIn, dailyEnergy, powerOut,totalEnergy);
  
  currTime = rtc_clock.get_seconds();
  
}

int getVirtualDay() {
  
  return rtc_clock.get_minutes();
}

void updateScreen(double voltage, double currentIn, double currentOut) {
  updateBattery(voltage);
  updateScreenValues(voltage * currentIn, voltage * currentOut);
  
}

void updateScreenUponDateChange() { // replace this function when sleep mode is completed
  static int oldDay = 0;
  int NewDay = getVirtualDay(); // a day may be shortened for testing purposes
  if (NewDay != oldDay) { 
    Serial.println(F("Updating Graph"));
    updateEnergyBars();
    updateFlash();
    dailyEnergy = 0; 
  }
   oldDay = NewDay;
}

