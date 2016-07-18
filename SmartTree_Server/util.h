#ifndef UTIL_H
#define UTIL_H

#include <arduino.h>
#include <rtc_clock.h>
#include <stream.h>
#include "globals.h"

extern RTC_clock rtc_clock;

inline void printDate(Print &out) {
  out.print(rtc_clock.get_months());
  out.print('/');
  out.print(rtc_clock.get_days());
  out.print('/');
  out.print(rtc_clock.get_years());
}

inline void printTime(Print &out) {
  out.print(rtc_clock.get_hours());
  out.print(':');
  out.print(rtc_clock.get_minutes());
  out.print(':');
  out.print(rtc_clock.get_seconds());
}

#endif
