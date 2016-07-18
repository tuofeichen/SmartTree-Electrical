#include <string.h>
#include <Arduino.h>
#include "receiver.h"

char *rEmptyStr = "";

Receiver::Receiver(Stream& stream, char *types) : s(stream) {
  validMessageTypes = types;
}

char Receiver::type() {
  return rType;
}

int Receiver::size() {
  return rCount;
}

char *Receiver::operator[](unsigned int n) {
  if(n >= rCount) {
    return rEmptyStr;
  } else {
    return rData[n];
  }
}

void Receiver::waitForBeginMessage() {
  while(s.read() != ':') { }
}

char Receiver::getMessageType() {
  while(!s.available()) { }
  return s.read();
}

int Receiver::getMessageLength() {
  return s.parseInt();
}

bool Receiver::getDataValues() {
  while(!s.available()) { }
  if(s.peek() == '\n') {
    s.read();
  }
  for(int i = 0; i < rCount; i++) {
    int bytes = s.readBytesUntil('\n', rData[i], maxDL - 1);
    if(bytes == 0) return false;
    rData[i][bytes] = 0; // null terminate
    if(rData[i][0] == ';' && bytes == 1) return false;
  }
  while(!s.available()) { }
  return (s.read() == ';');
}

bool Receiver::receiveData() {
  waitForBeginMessage();
  rType = getMessageType();
  if(strchr(validMessageTypes, rType) == NULL) {
    return false;
  }
  rCount = getMessageLength();
  if(rCount < 0 || rCount > maxRL) {
    return false;
  }
  return getDataValues();
}

void Receiver::reply(char message) {
  s.print(message);
}

