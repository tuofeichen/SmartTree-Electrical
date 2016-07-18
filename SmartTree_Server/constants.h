#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LOG_FILE_NAME "Jan 2nd Noon.csv"

#define DEBUG_PIN A0

#define ON_BOARD_TEMP A6

#define LED1 A8
#define LED2 A9

// ADC controls
#define ADDR0 A2
#define ADDR1 A3
#define ADDR2 A4
#define ADDR3 A5
#define ADCPIN A1

// Relay outputs
#define RI1 16
#define RO1 17
#define RI2 14
#define RO2 15
#define RI3 10
#define RO3 9
#define RI4 12
#define RO4 11
#define RSCREEN 3

#define SD_CHIP_SELECT  53

#define DEBUG_FLAG 0b100
#define SLEEP_FLAG 0b010
#define ERROR_FLAG 0b001

#define IS_FLAG_SET(FIELD, FLAG) (((FIELD) & (FLAG)) != 0)
#define IS_FLAG_CLEAR(FIELD, FLAG) (((FIELD) & (FLAG)) == 0)
#define SET_FLAG(FIELD, FLAG) FIELD |= (FLAG);
#define CLEAR_FLAG(FIELD, FLAG) FIELD &= ~(FLAG);

#define IN_NORMAL_MODE_STRICT(FIELD) (((FIELD) & 0b111) == 0b000)
#define IN_ERROR_MODE_STRICT(FIELD) (((FIELD) & 0b111) == 0b001)
#define IN_SLEEP_MODE_STRICT(FIELD) (((FIELD) & 0b110) == 0b010)
#define IN_DEBUG_MODE_STRICT(FIELD) (((FIELD) & 0b100) == 0b100)
#endif
