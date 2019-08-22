/* Tomy Tran - CPE 439
    This is the main code that schedules two tasks readFromCam and controlRobot
*/

/*--------------------------------------------------*/
/*--------------------- Includes -------------------*/
/*--------------------------------------------------*/

#include <Arduino_FreeRTOS.h>

#include "meArm_Adafruit.h"
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "comms.h"

/*--------------------------------------------------*/
/*----------------- Defines ------------------------*/
/*--------------------------------------------------*/

#define TASK1_DELAY ( 50 )
#define TASK2_DELAY ( 250 )

#define ZHEIGHT -20         // define the z plane height as a constant
#define DROPX 120           // x-location to drop die
#define DROPY 120           // y-location to drop die

/*--------------------------------------------------*/
/*----------------- Task Definition ----------------*/
/*--------------------------------------------------*/

//void readFromCam( void *pvParameters );
//void controlRobot( void *pvParameters );

/*--------------------------------------------------*/
/*---------------------- Setup ---------------------*/
/*--------------------------------------------------*/

comms comm;               // instantiate comms
bool actionFlag;          // setup a flag to tell the arm if it's picking up or dropping off
bool robotTaskComplete = true;   // flag for robot arm task completion, change to false for debugging

int X;  // setup coordinates
int Y;

meArm arm;            // instantiate the arm

// constants for debugging and checking that tasks are running
//const char *ardTextforTask1 = "Task 1 is running\r\n";
//const char *ardTextforTask2 = "Task 2 is running\r\n";


void setup() {

  Serial.begin(9600);

  while (!Serial) {
    ; //waiting for serial to connect with blocking code
  }
  
  Serial.println(F("In Setup function"));
  
  // Task creates with the RTOS standard
  xTaskCreate(
    readFromCam
    ,  "ReadFromCam"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    controlRobot
    ,  "ControlRobot"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  xTaskCreate(MyIdleTask, "IdleTask", 100, NULL, 0, NULL);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.

  // Start scheduler
//  vTaskStartScheduler();

/* TT - commented this out because guides I've read say the scheduler starts automatically */

}

void loop() {
  // Empty. Things are done in Tasks.

}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
static void readFromCam(void *pvParameters) {
//  char *ardTaskText;  // pointer for the text to output
//  ardTaskText = (char*) pvParameters; // cast the parameters to a character typedef
  (void) pvParameters;

  // SETUP
//  QueueHandle_t xcoordQueue, xflagQueue;  // creating a coordinate queue and flag queue
//  xcoordQueue = xQueueCreate (
//                  200,  // size of queue, double the size of the flag queue
//                  2 );  // byte size of objects in queue
//  xflagQueue = xQueueCreate (
//                 100,
//                 1 );
//
//  if (xcoordQueue == NULL) {
//    /* Queue was not created and must not be used. */
//  }

  //  static bool flag;

  //STATE MACHINE (looping code) (1-state here besides waiting)
  for (;;) {
    if ( robotTaskComplete == true ) {
      comm.commWithBasis(); // run the serial reading function and get all values out
      X = comm.getX();
      Y = comm.getY();
      actionFlag = comm.getFlag();
      /*
       * when vision sees the dice (actionFlag = true) and it spits out a non-zero number
       * then the robot will be allowed to go and this task will run but do nothing
       */
      if ( actionFlag == true & X != 0 & Y != 0 ) {
        robotTaskComplete = false;
      }
    
      // NOT USING QUEUES FOR THIS REV, USING SHARED VARIABLES
      //    xQueueSend(  // place the x value into the coordinate queue
      //      xcoordQueue,
      //      ( int *)&X,
      //      ( TickType_t ) 1 );  // wait 1 tick
      //    xQueueSend(  // place the y value into the coordinate queue
      //      xcoordQueue,
      //      ( int *)&Y,
      //      ( TickType_t ) 1 );
      //    xQueueSend(  // place the flag value into the flag queue
      //      xflagQueue,
      //      ( bool *)&flag,
      //      ( TickType_t ) 1 );
    }
    Serial.println(F("Task1"));  // print the status
    /* THIS TAKES ~ 50ms so add a delay */
    vTaskDelay( TASK1_DELAY / portTICK_PERIOD_MS );
  }
}

static void controlRobot(void *pvParameters) {
//  char *ardTaskText;  // pointer for the text to output
//  ardTaskText = (char*) pvParameters; // cast the parameters to a character typedef

  (void) pvParameters;
  
  // SETUP

  arm.begin();          // initialize the arm
  arm.openGripper();    // begin with open gripper

  // STATE MACHINE (1-state besides waiting)
  for (;;) {

    /*
       Note to Tomy:

       I'm not sure I understand why we need the action flag? Wouldn't the pickup actions always
       execute before the dropoff actions if the task delay isn't called until both are completed?

       Also yea, we probably need a third state or task to determine when to begin to read
       from the camera again. Maybe just a delay(3000) to give us time to move the die?
    */

    /*
       Note to Lea:
       Now we'll use action flag as a vision trigger. Vision will send an order that the dice has
       appeared and the flag will be set allowing the arm to move to the read point.

       Then we'll use the robot complete flag to manage not reading while the arm already knows
       there is a dice in play
       
    */
    
    // do pickup actions (gripper should be open because we need to reduce the program size)
    if ( actionFlag == true & robotTaskComplete == false ) {
      arm.gotoPoint(X, Y, (ZHEIGHT+40));     // go to above point defined in readFromCam()
      arm.gotoPoint(X, Y, ZHEIGHT);     //then approach
      delay(2000);                      // delay for travel time
      arm.closeGripper();               // close gripper to pick up die
      delay(1000);                      // wait for gripper to close
    
      // do drop off actions
    
      arm.gotoPoint(DROPX, DROPY, (ZHEIGHT+60));   // go to drop-off location
      delay(2000);                            // delay for travel time
      arm.openGripper();                      // open gripper to pick up
//      actionFlag = false;                      // set action to false for future pickup actions
      robotTaskComplete = true;               // set task completion to true
      //OPTIONAL: return to home location     // return the arm to home location
    }

    // block task when complete
//    if (robotTaskComplete = true) {
//      robotTaskComplete = false;                        // set task completion to false for future robot tasks
//      vTaskDelay( TASK2_DELAY / portTICK_PERIOD_MS );   // block task
//    }

    Serial.println(F("Task2"));  // print the status
    vTaskDelay( TASK2_DELAY / portTICK_PERIOD_MS );   // block task
  }

}

static void MyIdleTask(void* pvParameters)
{
  for(;;)
  {
    Serial.println(F("Idle state"));
    delay(50);
  }
}
