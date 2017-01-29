#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <arduino.h>
#include <stdarg.h>

#define SLEEP 'S'
#define WAKE 'W'

// Change to Serial1 after debugging
#define SC_CONN Serial1

inline void transmitMessage(char msg) {
  SC_CONN.print(':');
  SC_CONN.println(msg);
  SC_CONN.println('0');
  SC_CONN.println(';');
}

inline void transmitErrorMessage(int n, ...) {
  va_list argp;
  va_start(argp, n);
  SC_CONN.println(":E");
  SC_CONN.println(n);
  for(int i = 0; i < n; i++) {
    SC_CONN.println((char*)va_arg(argp, char*));
  }
  SC_CONN.println(';');
}

inline void transmitNoticeMessage(int n, ...) {
  va_list argp;
  va_start(argp, n);
  SC_CONN.println(":N");
  SC_CONN.println(n);
  for(int i = 0; i < n; i++) {
    SC_CONN.println((char*)va_arg(argp, char*));
  }
  SC_CONN.println(';');
}


inline void transmitClear() {
  SC_CONN.println(":C\n1");
//  SC_CONN.println(errorNumber);
  SC_CONN.println('1'); //dummy data
  SC_CONN.println(';');
}

inline void transmitErrorNumber(int errorNumber, int param = 0) {
  SC_CONN.println(":e\n2");
  SC_CONN.println(errorNumber);
  SC_CONN.println(param);
  SC_CONN.println(';');
}

inline void transmitNoticeNumber(int noticeNumber, int param = 0) {
  SC_CONN.println(":n\n2");
  SC_CONN.println(noticeNumber);
  SC_CONN.println(param);
  SC_CONN.println(';');
}

inline void transmitChargePercent(int chargePercent) {
  SC_CONN.println(":M\n1");
  SC_CONN.println(chargePercent);
  SC_CONN.println(';');
}

inline void transmitEnergyBars(int num, double *bars) {
  SC_CONN.println(":B");
  SC_CONN.println(num);
  for(int i = 0; i < num; i++) {
//    Serial.println(*bars);
    SC_CONN.println(*bars);
    bars++;
//    SC_CONN.println(bars[num]);
  }
  SC_CONN.println(';');
}

inline void transmitPowerData(int powerIn, int energy, int powerOut, int totalEnergy) {
  SC_CONN.println(":D");
  SC_CONN.println('4');
  SC_CONN.println(powerIn);
  SC_CONN.println(energy);
  SC_CONN.println(powerOut);
  SC_CONN.println(totalEnergy);
  SC_CONN.println(';');
}

inline double updateEnergyValue(double PowerIn, double OldEnergy, double PowerOut, int intval) {
  double energy = OldEnergy;
  double energyDifference = (PowerIn - PowerOut) * intval / 1000;
  energy = energy + energyDifference;
  return energy;
}

inline void internalTransmitLogData(int cellNumber, double PowerIn, double PowerOut, double OldEnergy,
              double voltage, double CurrentIn, double CurrentOut,
              double Temperature, byte error) {
  SC_CONN.println(":L");
  SC_CONN.println('2');
  SC_CONN.println(cellNumber);
  printDate(SC_CONN);
  SC_CONN.print(',');
  printTime(SC_CONN);
  SC_CONN.print(',');

  SC_CONN.print(PowerIn);
  SC_CONN.print(',');

  SC_CONN.print(PowerOut);
  SC_CONN.print(',');

  SC_CONN.print(OldEnergy);
  SC_CONN.print(',');

  SC_CONN.print(voltage);
  SC_CONN.print(',');

  SC_CONN.print(CurrentIn);
  SC_CONN.print(',');

  SC_CONN.print(CurrentOut);
  SC_CONN.print(',');

  SC_CONN.print(Temperature);
  SC_CONN.print(',');

  SC_CONN.println(error);

  SC_CONN.println(';');
}

inline void transmitLogData(int cellNumber, double voltage, double CurrentIn, double CurrentOut, double Temperature, byte error) {
  double PowerIn  = voltage*CurrentIn;
  double PowerOut = voltage*CurrentOut;
//  Serial.print (F("Power is: ")); //(debugging power output)
//  Serial.println (PowerIn-PowerOut);

//  oldEnergy = updateEnergyValue(PowerIn, oldEnergy, PowerOut,500); // 500 due to interrupt (this shouldn't be used -> use tft.cpp instead
  internalTransmitLogData(cellNumber, PowerIn, PowerOut, oldEnergy, voltage, CurrentIn, CurrentOut, Temperature, error);
}

#endif
