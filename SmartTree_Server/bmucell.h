#ifndef BMU_H
#define BMU_H

#include <Arduino.h>

enum sensorType {
  TEMPERATURE = 0,
  CURRENT_OUT = 1,
  CURRENT_IN = 2,
  VOLTAGE = 3
};

#define TEMPERATURE_ERROR      0b00000001
#define HIGH_VOLTAGE_ERROR     0b00000010
#define LOW_VOLTAGE_ERROR      0b00000100
#define HIGH_CURRENT_IN_ERROR  0b00001000
#define HIGH_CURRENT_OUT_ERROR 0b00010000
#define LOW_CURRENT_IN_WARNING 0b00100000

class Cell {
public:
  static void setup();
  Cell(unsigned int cellnum); 
  
  // update measurements
  void update() volatile;
  
  // update error values, used internally by update()
  // return true represents error condition
  bool checkErrors() volatile; 
  
  // send error descriptors over a stream, such as Serial
  void logData(Stream& s) volatile;
  void logErrors(Stream& s, bool verbose = false) volatile; 
  
  double getTemperature() volatile { return temperature; }
  double getVoltage() volatile { return voltage; }
  double getCurrentIn() volatile { return current_in; }
  double getCurrentOut() volatile { return current_out; }
  byte getErrorFlags() volatile { return error; }
  
private:
  static unsigned int readADC(unsigned int val);
  double readADCVoltage(sensorType type) volatile;
  
  double readTemperature() volatile;
  double readVoltage() volatile;
  double readCurrentIn() volatile;
  double readCurrentOut() volatile;
   
  static int relayInPin(unsigned int cell);
  static int relayOutPin(unsigned int cell);
private:
  // NOTE: in later versions of arduino, "static const double" must be replaced with "static constexpr double"
  static const double Vref = 3.3;
  static const double Voe = 2.5;
  unsigned int cell_num;
  
  // Error trigger and recovery values
  static const unsigned int TriggerCount = 10;
  static const unsigned int CycleCount = 100;
  
  // Error condition values
  static const double vHighLimit = 40;
  static const double vLowLimit = 30;
  static const double cLimit = 7;
  static const double cLowLimit = 1.0;
  static const int tempLimit = 50;
  
  // Recovery condition values
  static const double vHighRecover = 39;
  static const double vLowRecover = 36.5;
  static const double cRecover = 2.5;
  static const int tempRecover = 45;
  
  // measurements
  volatile double temperature, current_in, current_out, voltage;
  
  // error counter values
  volatile unsigned int temp_count, cin_count, cout_count, v_high_count, v_low_count;
  
  // error flags
  volatile byte error;
  
  // relay pins
  unsigned int relay_in, relay_out;
  
  //IOManager *io;
};

#endif
