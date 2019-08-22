#include "comms.h"
#include <SoftwareSerial.h>

void comms::comm() {
  X = 0;
  Y = 0;
  flag = false;
};

void comms::commWithBasis(){
  while (Serial.available() == 0);

  String input = Serial.readString();

  // Sorry to butcher this!! We need to reduce the size of the compiled sketch
//  int ind1 = input.indexOf(',');  
//  X = input.substring(0, ind1).toInt();   
//  Y = input.substring(ind1+1, input.indexOf(',', ind1+1)).toInt();  

  int ind1 = input.indexOf(',');  
  X = input.substring(0, ind1).toFloat();   
  int ind2 = input.indexOf(',', ind1+1 );   
  Y = input.substring(ind1+1, ind2).toFloat();  
  int ind3 = input.indexOf(',', ind2+1 );  
  String input3 = input.substring(ind2+1, ind3);
  if (input3 == "t" ||  input3 == "T"){
    flag = true;
  } else{
    flag = false;
  }
}


int comms::getX(){
  return X;
}
int comms::getY(){
  return Y;
}

bool comms::getFlag(){
  return flag;
}
