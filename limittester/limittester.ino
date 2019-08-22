/* Tomy Tran - CPE 439
 * This is the main code to control the arm. Uses Serial functions to receive commands from and 
 * send statuses to Zybo.
 * 
 * Pins: 
 * Arduino  PWMServo
 *    GND       GND
 *    5V        VCC
 *    A4        SDA
 *    A5        SCL
 *    
 * PWMServo   ArmServo
 *    0       Base
 *    1       Shoulder (right)
 *    2       Elbow (left)
 *    3       Gripper
 * 
 * Communication Architecture - 15 bytes
 * | Obj. Loc. | Obj. Loc. | Obj. Dest. | Obj. Dest. |
 * |  X-coord  |  Y-coord  |  X-coord   |  Y-coord   |
 * |  3 bytes  |  3 bytes  |  3 bytes   |  3 bytes   |
 * other 3 bytes are commas
 * E.X. 000,100,100,000
 */

#define ZERO 48                                   // define a zero for byte reading
#define NEGSIGN 45
//#define ZHEIGHT 10                                // define the z plane height as a constant
#include "meArm_Adafruit.h"                       // library for inverse kinematic functions
#include <Adafruit_PWMServoDriver.h>              // library for ServoDriver breakout board
#include <Wire.h>
#include <SoftwareSerial.h>

meArm arm;                                        // instantiation of meArm class that contains all
                                                  // all movement functions; 1 per arm

void ASCIItoInt(byte* bytes, int* ints);          // instantiate function

int zheight = -20;                                 // allocate a zheight
int xloc = 45;
int yloc = 125;
bool gripper = 0;                                 // counter for gripper


byte incomingMsg[2];                              // byte array for the commands received
byte* pmsg = incomingMsg;                         // pointer to the bytearray with command data
byte incomingAction[15];
byte* paction = incomingAction;
int commands[4]; 
int* pcmd = commands;



void setup() {
  Serial.begin(9600);                             // start serial comm on specified baud
  arm.begin();                                    // initialize the arm we created above
}

void loop() {

  if (Serial.available() > 0) {
    Serial.readBytes(pmsg, 2);                    // read 2 bytes and send to incomingMessage
    if (*pmsg == 36) {                            // look for the action symbol $
      if (*(pmsg+1) == 122) {                     // look for z 
        Serial.println("What z-height?");
        while(true) {                             // while loop to wait for next command
          if (Serial.available() > 0) {
            Serial.readBytes(paction, 4);         // use the action pointer but read only three bytes for z height
//            Serial.println(*paction);
//            Serial.println(*(paction+1));
//            Serial.println(*(paction+2));
            if (*paction == NEGSIGN) {
//              *(paction+3) = Serial.read();
              ASCIItoInt((paction+1), pcmd, 1);
              zheight = (*pcmd) * -1;                      // set the zheight
            }
            else {
              ASCIItoInt(paction, pcmd, 1);         // translate the message to an integer
              zheight = *pcmd;                      // set the zheight
            }
            Serial.println(zheight);
            arm.gotoPoint(xloc, yloc, zheight);       // go to the called zheight
            break;
          }
        }
      }
      if (*(pmsg+1) == 114) {
        Serial.println("Resetting position.");
        xloc = 45;
        yloc = 125;
        zheight = -20;
        arm.goDirectlyTo(xloc,yloc,zheight);
      }
      if (*(pmsg+1) == 99) {
        if (gripper == 1) {
          Serial.println("Closing gripper");
          arm.closeGripper();
          gripper = 0;
        }
        else {
          Serial.println("Opening gripper");
          arm.openGripper();
          gripper = 1;
        }
      }
      if (*(pmsg+1) == 112) {
        Serial.println("What position (x,y)?");
        while(true) {
          if (Serial.available() > 0) {
            Serial.readBytes(paction, 7);
            ASCIItoInt(paction, pcmd, 2);
//            Serial.println(*pcmd);
//            Serial.println(*(pcmd+1));
            xloc = *pcmd;
            yloc = *(pcmd+1);
            Serial.println(xloc);
            Serial.println(yloc);
            arm.gotoPoint(xloc, yloc, zheight);
            break;
          }
        }
      }
    }
    else {
      Serial.println("Wrong value:");
    }

//    ASCIItoInt(bytelocation, intlocation);        // change bytes to ints

//    int i = 0;
//    while (i<4) {                                 // while loop for showing ints
//      Serial.print(*(intlocation+i));
//      Serial.print('\n');
//      i++;
//    }

//    if (arm.isReachable() == 1) {
//      arm.openGripper();                            // open the gripper to prepare to pick up
//      arm.gotoPoint(*intlocation, *(intlocation+1), ZHEIGHT);      
//    }

  }
}

void ASCIItoInt(byte* bytes, int* ints, int numValues) {
//This function takes in numbers that are 3 bytes-ASCII-decimal and converts them to integers
  int i = 0;
  int j = 0;
  while (i<numValues) {
    *(ints+i) = ((*(bytes+j) - ZERO)*100) + ((*(bytes+j+1) - ZERO)*10) + ((*(bytes+j+2) - ZERO));
    j = j + 4;
    i++;
  }
  
//  *(ints) = ((*bytes - ZERO)*100) + ((*(bytes+1) - ZERO)*10) + ((*(bytes+2) - ZERO));
//  *(ints+1) = ((*(bytes+4) - ZERO)*100) + ((*(bytes+5) - ZERO)*10) + ((*(bytes+6) - ZERO));
//  *(ints+2) = ((*(bytes+8) - ZERO)*100) + ((*(bytes+9) - ZERO)*10) + ((*(bytes+10) - ZERO));
//  *(ints+3) = ((*(bytes+12) - ZERO)*100) + ((*(bytes+13) - ZERO)*10) + ((*(bytes+14) - ZERO));

  return;
}
