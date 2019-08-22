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
 * Arduino    Zybo
 *    0 (RX)    L14 (TX)
 *    1 (TX)    N15 (RX)
 * SET BAUD FOR BOTH TO 9600
 * 
 * Communication Architecture - 15 bytes
 * | Obj. Loc. | Obj. Loc. | Obj. Dest. | Obj. Dest. |
 * |  X-coord  |  Y-coord  |  X-coord   |  Y-coord   |
 * |  3 bytes  |  3 bytes  |  3 bytes   |  3 bytes   |
 * other 3 bytes are commas
 * E.X. 000,100,100,000
 */

#define ZERO 48                                   // define a zero for byte reading
#define ZHEIGHT -20                               // define the z plane height as a constant
#include "meArm_Adafruit.h"                       // library for inverse kinematic functions
#include <Adafruit_PWMServoDriver.h>              // library for ServoDriver breakout board
#include <Wire.h>
#include <SoftwareSerial.h>

meArm arm;                                        // instantiation of meArm class that contains all
                                                  // all movement functions; 1 per arm

void ASCIItoInt(byte* bytes, int* ints);          // instantiate function

byte incomingBytes[15];                           // byte array for the commands received
byte* bytelocation = incomingBytes;               // pointer to the bytearray with command data
int commands[4]; 
int* intlocation = commands;

void setup() {
  Serial.begin(9600);                             // start serial comm on specified baud
  arm.begin();                                    // initialize the arm we created above
}

void loop() {

  if (Serial.available() > 0) {
    Serial.readBytes(bytelocation, 15);           // read 15 bytes and send to pointer
    Serial.println((char*)bytelocation);
    ASCIItoInt(bytelocation, intlocation);        // change bytes to ints

//    int i = 0;
//    while (i<4) {                                 // while loop for showing ints
//      Serial.print(*(intlocation+i));
//      Serial.print('\n');
//      i++;
//    }

    arm.openGripper();                            // open the gripper to prepare to pick up
    Serial.println(*intlocation + ", " + *(intlocation+1));
    arm.gotoPoint(*intlocation, *(intlocation+1), ZHEIGHT);
    delay(2000);
    arm.closeGripper();
    arm.gotoPoint(*(intlocation+2), *(intlocation+3), ZHEIGHT);
  }
}

void ASCIItoInt(byte* bytes, int* ints) {
//This function takes in numbers that are 3 bytes-ASCII-decimal and converts them to integers
  *(ints) = ((*bytes - ZERO)*100) + ((*(bytes+1) - ZERO)*10) + ((*(bytes+2) - ZERO));
  *(ints+1) = ((*(bytes+4) - ZERO)*100) + ((*(bytes+5) - ZERO)*10) + ((*(bytes+6) - ZERO));
  *(ints+2) = ((*(bytes+8) - ZERO)*100) + ((*(bytes+9) - ZERO)*10) + ((*(bytes+10) - ZERO));
  *(ints+3) = ((*(bytes+12) - ZERO)*100) + ((*(bytes+13) - ZERO)*10) + ((*(bytes+14) - ZERO));

  return;
}
