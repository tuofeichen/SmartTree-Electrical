#include <arduino.h>
#include "bmucell.h"
#include "constants.h"

void Cell::setup() {
  // set up shared ADC
  pinMode(ADCPIN, INPUT);
  pinMode(ADDR0, OUTPUT);
  pinMode(ADDR1, OUTPUT);
  pinMode(ADDR2, OUTPUT);
  pinMode(ADDR3, OUTPUT);
  analogReadResolution(12);

  pinMode(RSCREEN, OUTPUT);
  digitalWrite(RSCREEN, HIGH);
}

Cell::Cell(unsigned int cellnum) {
  cell_num = cellnum;
  relay_in = relayInPin(cellnum);
  relay_out = relayOutPin(cellnum);
  pinMode(relay_in, OUTPUT);
  pinMode(relay_out, OUTPUT);
  digitalWrite(relay_in, HIGH);
  digitalWrite(relay_out, HIGH);

  temp_count = 0;
  cin_count = 0;
  cout_count = 0;
  v_high_count = 0;
  v_low_count = 0;
  error = 0;
}

void Cell::update() volatile {
  temperature = readTemperature();
  current_out = readCurrentOut();
  current_in = readCurrentIn();
  voltage = readVoltage();
}

void Cell::logData(Stream &s) volatile {
  s.print(F("Cell "));
  s.print(cell_num);
  s.println(':');

  s.print(F("  Temperature:    "));
  s.println(temperature);

  s.print(F("  Output Current: "));
  s.println(current_out);

  s.print(F("  Input Current:  "));
  s.println(current_in);

  s.print(F("  Voltage:        "));
  s.println(voltage);
}

bool Cell::checkErrors() volatile {
  //Serial.println(error, BIN);
  // low input current warning
  if(current_in < cLowLimit) {
    SET_FLAG(error, LOW_CURRENT_IN_WARNING);
  } else {
    CLEAR_FLAG(error, LOW_CURRENT_IN_WARNING);
  }

  // if error condition then increment error counter, otherwise decrement until 0
  // if any error occurs more than TriggerCount times then record and handle the error

  if (IS_FLAG_CLEAR(error, TEMPERATURE_ERROR)) { // currently no temperature error, test for temperature error condition
    if (temperature > tempLimit) temp_count++;
    else if (temp_count > 0) temp_count--;
    // this value is remembered because static local var (or its just a class variable now? )

    if (temp_count >= TriggerCount) { // if the error condition happens enough
      temp_count = 0;
      digitalWrite(relay_in, LOW); //Over Temperature
      digitalWrite(relay_out, LOW); // isolate the entire battery pack
      SET_FLAG(error, TEMPERATURE_ERROR);
    }
   } else { // error has occurred previously, try to recover from temperature error
    if (temperature < tempRecover) temp_count++;
    else if (temp_count > 0) temp_count--;

    if (temp_count >= TriggerCount) {
      temp_count = 0;
      CLEAR_FLAG(error, TEMPERATURE_ERROR);
    }
  }

  if (IS_FLAG_CLEAR(error, HIGH_VOLTAGE_ERROR)) {
    if (voltage > vHighLimit) v_high_count++;
    else if (v_high_count > 0) v_high_count--;

    if (v_high_count >= TriggerCount) {
      v_high_count = 0;
      digitalWrite(relay_in, LOW); //Over Voltage, stop charging
      SET_FLAG(error, HIGH_VOLTAGE_ERROR);
    }
  } else {
    if (voltage < vHighRecover) v_high_count++;
    else if (v_high_count > 0) v_high_count--;

    if (v_high_count >= TriggerCount) {
      v_high_count = 0;
      CLEAR_FLAG(error, HIGH_VOLTAGE_ERROR);
    }
  }

  if(IS_FLAG_CLEAR(error, LOW_VOLTAGE_ERROR)) {
    if (voltage < vLowLimit) v_low_count++;
    else if (v_low_count > 0) v_low_count--;


    if (v_low_count >= TriggerCount) {
      v_low_count = 0;
      digitalWrite(relay_out, LOW); //Under Voltage, stop discharging
      SET_FLAG(error, LOW_VOLTAGE_ERROR);
    }
  } else {
    if (voltage > vLowRecover) v_low_count++;
    else if (v_low_count > 0) v_low_count--;

    if (v_low_count >= TriggerCount)
    {
      v_low_count = 0;
      CLEAR_FLAG(error, LOW_VOLTAGE_ERROR);
    }
  }

  // if there is overvoltage or overtemp, then the relay must stay off
  if((error & (HIGH_VOLTAGE_ERROR | TEMPERATURE_ERROR)) == 0) {
    // check if there is currently no overcurrent error
    if(IS_FLAG_CLEAR(error, HIGH_CURRENT_IN_ERROR)) {
      if (current_in > cLimit) cin_count++;
      else if (cin_count > 0) cin_count--;

      if (cin_count >= TriggerCount) {
         cin_count = 0;
        digitalWrite(relay_in, LOW); //Over Current in, stop charging
        SET_FLAG(error, HIGH_CURRENT_IN_ERROR);
      }
    } else { // try to recover from current error
      cin_count++;
      // after first CycleCount duration has passed, close the relay to test
      if(cin_count == CycleCount) {
        // only if other errors allow for the relay to close
        if((error | ~(TEMPERATURE_ERROR | HIGH_VOLTAGE_ERROR))
                 == ~(TEMPERATURE_ERROR | HIGH_VOLTAGE_ERROR)) {
          digitalWrite(relay_in, HIGH);
        } else {
          cin_count = 0;
        }
      }
      // if test fails before second CycleCount duration passes then reset cycle
      if(cin_count > CycleCount) {
        if(current_in > cRecover) {
          //
          digitalWrite(relay_in, LOW);
          cin_count = 0;
        }
      }
      // if test succeeds for the second CycleCount duration then remove error
      if(cin_count > 2 * CycleCount) {
        CLEAR_FLAG(error, HIGH_CURRENT_IN_ERROR);
        cin_count = 0;
      }
    }
  }

  // if there is undervoltage or overtemp, then the relay must stay off
  if((error & (LOW_VOLTAGE_ERROR | TEMPERATURE_ERROR)) == 0) {
    // check if there is currently no overcurrent error
    if(IS_FLAG_CLEAR(error, HIGH_CURRENT_OUT_ERROR)) {
      if (current_out > cLimit) cout_count++;
      else if (cout_count > 0) cout_count--;

      if (cout_count >= TriggerCount) {
        cout_count = 0;
        digitalWrite(relay_out, LOW); //Over Current out, stop discharging
        SET_FLAG(error, HIGH_CURRENT_OUT_ERROR);
      }
    } else { // overcurrent error flag is set, try to recover
      cout_count++;
      // after first CycleCount duration has passed, close the relay to test
      if(cout_count == CycleCount) {
        // only if other errors allow for the relay to close
        if((error | ~(TEMPERATURE_ERROR | LOW_VOLTAGE_ERROR))
                 == ~(TEMPERATURE_ERROR | LOW_VOLTAGE_ERROR)) {
          digitalWrite(relay_out, HIGH);
        } else {
          cout_count = 0;
        }
      }
      // if test fails before second CycleCount duration passes then reset cycle
      if(cout_count > CycleCount) {
        if(current_out > cRecover) {
          digitalWrite(relay_out, LOW);
          cout_count = 0;
        }
      }
      // if test succeeds for the second CycleCount duration then remove error
      if(cout_count > 2 * CycleCount) {
        CLEAR_FLAG(error, HIGH_CURRENT_OUT_ERROR);
        cout_count = 0;
      }
    }
  }

  // turn on the circuit based on errors
  // note: need to mask out under current in

  if (((error & (0b00011111)) | ~(TEMPERATURE_ERROR | LOW_VOLTAGE_ERROR | HIGH_CURRENT_OUT_ERROR))
            == ~(TEMPERATURE_ERROR | LOW_VOLTAGE_ERROR | HIGH_CURRENT_OUT_ERROR)) {
    digitalWrite(relay_out, HIGH);
  }

  if (((error & (0b00011111)) | ~(TEMPERATURE_ERROR | HIGH_VOLTAGE_ERROR | HIGH_CURRENT_IN_ERROR))
            == ~(TEMPERATURE_ERROR | HIGH_VOLTAGE_ERROR | HIGH_CURRENT_IN_ERROR)) {
    digitalWrite(relay_in, HIGH);
  }

  return (error != 0 && error != LOW_CURRENT_IN_WARNING);
}

void Cell::logErrors(Stream& s, bool verbose) volatile {
  s.print("cell ");
  s.print(cell_num);
  unsigned char msn = error >> 4;
  unsigned char lsn = error & 0x0F;
  msn += (msn > 9) ? 'a' - 10 : '0';
  lsn += (lsn > 9) ? 'a' - 10 : '0';
  s.print(" error code: ");
  s.print((char)msn);
  s.println((char)lsn);

  if(verbose) {
    if(IS_FLAG_SET(error, TEMPERATURE_ERROR)) s.println(F("Temperature error"));
    if(IS_FLAG_SET(error, HIGH_VOLTAGE_ERROR)) s.println(F("Overvoltage error"));
    if(IS_FLAG_SET(error, LOW_VOLTAGE_ERROR)) s.println(F("Undervoltage error"));
    if(IS_FLAG_SET(error, HIGH_CURRENT_IN_ERROR)) s.println(F("Overcurrent input error"));
    if(IS_FLAG_SET(error, HIGH_CURRENT_OUT_ERROR)) s.println(F("Overcurrent output error"));
    if(IS_FLAG_SET(error, LOW_CURRENT_IN_WARNING)) s.println(F("Undercurrent input warning"));
  }
}

int Cell::relayInPin(unsigned int cell) {
  switch (cell) {
    case 0:
      return RI1;
    case 1:
      return RI2;
    case 2:
      return RI3;
    case 3:
      return RI4;
    default:
      return -1;
  }
}

int Cell::relayOutPin(unsigned int cell) {
  switch (cell) {
    case 0:
      return RO1;
    case 1:
      return RO2;
    case 2:
      return RO3;
    case 3:
      return RO4;
    default:
      return -1;
  }
}

double Cell::readTemperature() volatile {
  double voltage = readADCVoltage(TEMPERATURE);
  return (13.582 - sqrt(184.470724 + 0.01732 * (2230.8 - voltage * 1000))) / (-0.00866) + 30;
}

double Cell::readVoltage() volatile {
  double temp = readADCVoltage(VOLTAGE);
  return temp / (63.69 * 0.001);
}

double Cell::readCurrentIn() volatile {
  double voltage = readADCVoltage(CURRENT_IN);
  return abs((voltage - Voe) *  5/0.625);
}

double Cell::readCurrentOut() volatile {
  double voltage = readADCVoltage(CURRENT_OUT);
  return abs((voltage - Voe) * 5/0.625);
}

double Cell::readADCVoltage(sensorType type) volatile {
  return Vref * readADC(cell_num * 4 + type) / 4096;
}


unsigned int Cell::readADC(unsigned int val) {
  digitalWrite(ADDR0, (val & 0b0001) != 0 ? HIGH : LOW);
  digitalWrite(ADDR1, (val & 0b0010) != 0 ? HIGH : LOW);
  digitalWrite(ADDR2, (val & 0b0100) != 0 ? HIGH : LOW);
  digitalWrite(ADDR3, (val & 0b1000) != 0 ? HIGH : LOW);

  return analogRead(ADCPIN);
}
