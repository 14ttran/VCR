#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>

class comms {
protected:
  int X;
  int Y;
  bool flag;
public:
  void comm();
  void commWithBasis();
  int getX();
  int getY();
  bool getFlag();
};
#endif
