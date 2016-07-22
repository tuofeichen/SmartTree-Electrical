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
//  Serial.println("Received begin message");
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
    int bytes = 0;
    char temp[2];
    if (i==0){
      bytes = s.readBytesUntil('\n', temp, 2);
//      Serial.println("skip header endline");
    }
    
    bytes = s.readBytesUntil('\n',rData[i], maxDL - 1);
//    Serial.print("Read Data of bytes: " );
//    Serial.println (bytes) ; 
    
    for (int j = 0 ; j < bytes; j++)
      Serial.print(rData[i][j]);
    Serial.println(" "); //finished line
    
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
  bool endFlag= 0;
  
  
  if(strchr(validMessageTypes, rType) == NULL) {
    return false;
  }
  
  Serial.print(" Received valid type message ");
  Serial.print(rType);
  
  
  rCount = getMessageLength();
  Serial.print (" message length is ") ;
  Serial.println(rCount);
  
  if(rCount < 0 || rCount > maxRL) {
    
    return false;
  }
  
  endFlag = getDataValues();
  
  return endFlag;
}

void Receiver::reply(char message) {
  s.print(message);
}

