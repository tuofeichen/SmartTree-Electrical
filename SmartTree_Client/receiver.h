#ifndef RECEIVER_H
#define RECEIVER_H

#include <Arduino.h>
#include <stream.h>

#define REPLY_READY   'R'
#define REPLY_BUSY    'B'
#define REPLY_FAILED  'F'
#define REPLY_SUCCESS 'S'

class Receiver {
public:
  Receiver(Stream& stream, char *types);

  bool receiveData();
  void reply(char message);
  char type();
  int size();
  char *operator[](unsigned int n);

private:
  Stream &s;
  static const int maxRL = 10;
  static const int maxDL = 1000;
  char rType;
  int rCount;
  char rData[maxRL][maxDL];
  char *validMessageTypes;

  void waitForBeginMessage();
  char getMessageType();
  int getMessageLength();
  bool getDataValues();
};

#endif
