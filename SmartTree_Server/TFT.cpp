#include <SPI.h>
#include <SdFat.h>
//#include <UTFT.h>
//#include <UTFT_SdRaw.h>
#include <rtc_clock.h>

#include <DueFlashStorage.h>
#include <efc.h>
#include <flash_efc.h>

#include "TFT.h"
#include "constants.h"
#include "globals.h"
#include "transmitter.h"

void initRTC() {
  rtc_clock.init();
  rtc_clock.set_time(8, 0, 0);
  rtc_clock.set_date(25, 10, 2015);
}

void updateBattery() {
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
  Serial.print ("Updating Energy bar!!");
  
  Serial1.println(":B");
  if (energyAddress < 8 && firstCycle) { // if there are not enough bars to fill the meter
    for(int i = 0; i < energyAddress; i++) {
      values[i] = dueFlashStorage.read(i);
    }
    transmitEnergyBars(energyAddress, values);
  } else {
    firstCycle = false;
    for(int i = 0; i < 8; i++) {
      values[i] = dueFlashStorage.read((energyAddress - 8 + 50 + i) % 50); // wrap around, ensure positive
    }
    
    values[8] = oldEnergy;    
//    Serial.println(oldEnergy);
    
    transmitEnergyBars(9, values);
  }
}

void updateFlash() {
  dueFlashStorage.write(energyAddress, oldEnergy);
  energyAddress++;
  if(energyAddress > 50) {
    energyAddress = 0;
  }
}

// TODO remove changing of global energy state

int updateScreenValues(int powerIn, int powerOut) {
  double energy = oldEnergy;
  int time = rtc_clock.get_seconds() - oldTime;
  if (time < 0) {
    time += 60;
  }

  
  double energyDifference = (float) (powerIn - powerOut) * ((float)time / 3600.0); // right ?
  
  Serial.print("Energy difference is ");
  Serial.println(energyDifference);

  energy += energyDifference;
  
  if (powerIn< 20)
    transmitPowerData(0, energy, powerOut);
  else if(powerOut<20)
    transmitPowerData(powerIn, energy, 0);
  else
    transmitPowerData(powerIn, energy, powerOut);
 
  Serial.print("Energy  is ");
  Serial.println(energy);
  
  oldTime += time;
  if (oldTime >= 60) {
    oldTime -= 60;
  }
  totalEnergy += energyDifference;
  //GLCD.printNumI(totalEnergy / 1000, 620, 370);
  return energy;
}

int getVirtualDay() {
  return rtc_clock.get_seconds();
}
