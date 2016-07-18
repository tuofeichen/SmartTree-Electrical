#include "debug.h"
#include "constants.h"

#include "globals.h"

bool pollDebugPin() {
  return digitalRead(DEBUG_PIN) == LOW;
}

void enterDebugMode(Print &out) {
  SET_FLAG(mode, DEBUG_MODE);
  out.println("DEBUG");
}

void setDateFromStream(Stream &in, Print& out) {
  out.print("set date (MM/DD/YYYY): ");
  int month, day, year;
  int val = in.peek();
  while(val <= ' ') {
    in.read();
    val = in.peek();
  }
  do {
    month = in.parseInt();
  } while(month == 0);
  if(in.peek() != '/') {
    out.println("Syntax error");
    return;
  } 
  day = in.parseInt();
  if(in.peek() != '/') {
    out.println("Syntax error");
    return;
  }
  year = in.parseInt();
  
  if(year < 100) {
    year += 2000;
  }
  
  rtc_clock.set_date(day, month, year);
}

void setTimeFromStream(Stream &in, Print& out) {
  out.print("set time (HH:MM:SS): ");
  int second, minute, hour;
  int val = in.peek();
  while(val < '0' || val > '9') {
    if(val > ' ') {
      out.println("Syntax error");
      return;
    }
    in.read();
    val = in.peek();
  }
  
  hour = in.parseInt();
  if(in.peek() != ':') {
    out.println("Syntax error");
    return;
  }
  minute = in.parseInt();
  if(in.peek() != ':') {
    second = 0;
  } else {
    second = in.parseInt();
  }
  rtc_clock.set_time(hour, minute, second);
}

void debug(Stream &in, Print& out) {
  int command = in.read();
  switch(command) {
  
  case 'D':
    setDateFromStream(in, out);
  case 'd':
    printDate(out);
    break;
  case 'T':
    setTimeFromStream(in, out);
  case 't':
    printTime(out);
    break;
  case 'e':
    out.println("Exit");
    CLEAR_FLAG(mode, DEBUG_MODE);
    break;
  default:
    if(command > ' ') {
      out.println("Invalid command"); 
    }
  }
}
