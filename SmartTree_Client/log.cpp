#include "log.h"
#include "globals.h"

void SDInit() {
  isCardPresent = false;
  long beginTime = millis();
  // if card is not accessible within 1 second then do not log
  while(millis() - beginTime < 1000) {
    if(sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) {
      isCardPresent = true;
      break;
    }
  }
}

void logToFile(char *fname, char *data) {
  if(!isCardPresent || !file.open(fname, O_RDWR | O_CREAT | O_AT_END)) return;
  file.print(data);
  file.close();
}
