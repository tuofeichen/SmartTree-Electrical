#ifndef GLOBALS_H
#define GLOBALS_H

#include <SPI.h>
#include <SdFat.h>
#include <UTFT.h>
#include <UTFT_SdRaw.h>

#include "receiver.h"
#include "tft.h"
#include <stream.h>
#include <string.h>
#include <stdlib.h>

extern SdFat sd;
extern SdFile file;
extern UTFT GLCD;
extern UTFT_SdRaw files;


extern bool isCardPresent;
#endif
