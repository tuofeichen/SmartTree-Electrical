//#include "log.h"
//#include "constants.h"
//
//#include <SPI.h>
//#include <SdFat.h>
//#include <UTFT.h>
//#include <UTFT_SdRaw.h>
//#include <rtc_clock.h>
//
//#include <DueFlashStorage.h>
//#include <efc.h>
//#include <flash_efc.h>
//
//#include "globals.h"
//
//void initSD() {
//  while (!Serial) { }
//  Serial.println(F("Initialising SD card..."));
//  bool mysd = 0;
//  // see if the card is present and can be initialized:
//  while (!mysd) {
//    if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) {
//      Serial.println(F("Card failed, or not present"));
//    } else {
//      mysd = 1;
//      Serial.println(F("Card initialised."));
//    }
//  }
//}
//
//void writeLogFileHeader(char *fname) { 
//  // Test writing condition
//  if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) {
//    sd.initErrorHalt();
//  }
//
//  // open the file for write at end like the Native SD library
//  if (!file.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
//    sd.errorHalt("opening Data.txt for write failed");
//  }
//  
//  Serial.println(F("Write Header."));
//  file.println(F("Date, PowerIn, PowerOut, Energy, Voltage, CurrentIn, CurrentOut, Temperature, Error"));
//  file.close();
//  Serial.println(F("Write Header Done."));
//}
//
//double updateEnergyValue(double PowerIn, double OldEnergy, double PowerOut, int intval) {
//  double energy = oldEnergy;
//  double energyDifference = (PowerIn - PowerOut) * intval / 1000;
//  energy = energy + energyDifference;
//  return energy;
//}
//
//void logMeasurementsInternal(char *fname, double PowerIn, double PowerOut, double OldEnergy, 
//              double voltage, double CurrentIn, double CurrentOut,
//              double Temperature, byte error) {
//  /*
//  int seconds = rtc_clock.get_seconds();
//  int minutes = rtc_clock.get_minutes();
//  int hours = rtc_clock.get_hours();
//  int days = rtc_clock.get_days();
//  int months = rtc_clock.get_months();
//  uint16_t years = rtc_clock.get_years();
//  */
//  
//  while (!Serial) {}  // wait for Leonardo
//
//  // Initialize SdFat or print a detailed error message and halt
//  // Use half speed like the native library.
//  // change to SPI_FULL_SPEED for more performance.
//  if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) {
//    sd.initErrorHalt();
//  }
//
//  // open the file for write at end like the Native SD library
//  if (!file.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
//    sd.errorHalt("opening Data.txt for write failed");
//  }
//  // if the file opened okay, write to it:
//  Serial.println(F("---------------------------"));
//  Serial.print(F("Writing to Data..."));
//  /*
//  file.print(months);
//  file.print('/');
//  file.print(days);
//  file.print('/');
//  file.print(years);
//  */
//  printDate(file);
//  file.print(F(" Time: "));
//  /*
//  file.print(hours);
//  file.print(':');
//  file.print(minutes);
//  file.print(':');
//  file.print(seconds);
//  */
//  printTime(file);
//  file.print(',');
//  
//  file.print(PowerIn);
//  file.print(',');
//  
//  file.print(PowerOut);
//  file.print(',');
//  
//  file.print(OldEnergy);
//  file.print(',');
//  
//  file.print(voltage); 
//  file.print(',');
//
//  file.print(CurrentIn);
//  file.print(',');
//  
//  file.print(CurrentOut);
//  file.print(',');
//  
//  file.print(Temperature);
//  file.print(',');
//  
//  file.println(error);
//  
//  file.close();
//  Serial.println(F("done."));
//}
//
//void logMeasurements(char *fname, double voltage, double CurrentIn, double CurrentOut, double Temperature, byte error) {
//  double PowerIn  = voltage*CurrentIn;
//  double PowerOut = voltage*CurrentOut;
//  Serial.print(F("Power is: "));
//  Serial.println(PowerIn-PowerOut);
//  oldEnergy = updateEnergyValue(PowerIn, oldEnergy, PowerOut,500);
//  logMeasurementsInternal(fname, PowerIn, PowerOut, oldEnergy, voltage, CurrentIn, CurrentOut, Temperature, error);
//}
