#ifndef ERROR_H
#define ERROR_H

#include "bmucell.h"

bool displayError(volatile Cell cells[], int numel); // return true if big box error
void setupPrintMinorError();
#endif
