
/*
 * build HMI_BUILD_5_10_2109
 * This version is for the 2019 Mate ROV
 * 
 * 5.1.'18 
 * remapped out the motors. 
 * Rebuilt program to run port / stbd motors  two vertical motors, and one crab motor.
 * This is the operater control program for the rov.
 * Two programs one for the human operater, second resides in the robot.
 * the program is done to emulate  an industral OIT or HMI (name depending on your background)
 * the rov is designed to be a industral controller like a PLC.
 * The operator comptuer is Ardunio Mega
 * 
The ROV computer is Arduino Uno. 

 I  prefer defines rather than const vars. Personal choice
 also make #define all CAPS, makes easier debug
 the more modern C++ guideline suggest using const int instead.
 advanced use of #defines can cause uexepected problems.

 globals start with a cap
 local vars start with lower case

 */

#include <arduino.h>
#include "HMI_header.h"

#define  BUILD_2019   true // We will not erase existing code but rather not use it.


void setup() 
{
 
  pinMode(CAMERA_SELECT, INPUT);    // initialize the pushbutton pin as an input: video 4
  pinMode(DISPLAY_SELECT, INPUT); // nav display  5

  pinMode(LEDPIN , OUTPUT);   // USED FOR TESTING on obard led output 13
   
  pinMode(TxENABLE,OUTPUT);  // connect pin 22 of MAX485
  Serial.begin(9600);       // communication channel for debugging on monitor
  Serial1.begin(9600);      // communication channel for HMI-ROV RS-484 link
  Serial2.begin(9600);      // communication channel for panel display only works at 9600 baud
   
 
}

void loop() 
{
  camera_select();  // pb for select camera , NUM_CAMERAS = 2
  simple_motors();  // 4 and 2 thruster control. see HMI_header.h for config

  Update_ROV_Data();  // communication to ROV via Serial1
  
   
} // end loop               


 /******************************** 
  *  assign test values will be over written
   *correct valuses will come from the nav chip
   *HoldingRegs{} is global array used for data xmission and receive.
  *************************************/
void Nav_test_values(void)
{
  HoldingRegs[COMPASS] = 99;  //realistic but random start values.
  HoldingRegs[TIP] = 300;
  HoldingRegs[TILT]= 234;
  HoldingRegs[TEMPERATURE] = 19;
}

/************************************
The prgram takes a single push button and at each press index to the next video camera.
The push button resides in the controller on the surface. It generates a camera select, sends the 
select down to the ROV where the actual selceiton happens. 

The program in final form is in two parts, and two computers. An operator control on the surface, and a camera select
on the ROV. 

********************************************/

void camera_select(void)  // called from HMI loop()
{
  int valid_button;
  valid_button = debounce2(CAMERA_SELECT);  // return -1 if no data, 0 low, 1 high
  if(valid_button >= 0) 
    button2(valid_button);
}

int debounce2(int button2)  // called from camera_select
{
  /* read the state of the switch into a local variable:
    check to see if you just pressed the button
    (i.e. the input went from LOW to HIGH),  and you've waited
    long enough since the last press to ignore any noise:
    If the switch changed, due to noise or pressing:
    reading   LastButtonState
    0   0      
    0   1
    1   0
    1   1
*/ 
  
  static int LastButtonState = LOW;   // the previous reading from the input pin
  static unsigned long LastDebounceTime = 0;  // the last time the output pin was toggled

  int status;
  int reading = digitalRead(button2);   // read button
  
 
  if (reading != LastButtonState)
  {
        LastDebounceTime = millis();     // reset the debouncing timer & start over
        status = -1;
  }
    
  LastButtonState = reading; 
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
  
  if ((millis() - LastDebounceTime) > DEBOUNCEDELAY) 
  {
    status = reading;  // good value can be high / low
  }
   return (status);   // return 0 if open, 1 if closed, -1 not ready
}



void button2(int state)// locate in HMI
{
  static int flag = 0;    //static is like a global variable
  static int light;       // that is only valid in function.
  int button;
  
  // need to debounce 
  
  if(state >= 0)  // if button press is good. 
  {
    // toggle lights...
    if (state && !flag)
    {
      flag = 1;
      button = false;
    }
    if (!state && flag)
    {
      flag = 0;
      light = ++light % NUM_CAMERAS;  // light will count 0,1,2,3
      // what will really happen is light is a value written in the command 
     
       HoldingRegs[VIDEO_CHANNEL] = light; // this resides in HMI
      //Serial.println(HoldingRegs[VIDEO_CHANNEL]);  // print out to see what is going on.    
    }
  }
} // end button2


 
/*
 * 
 * / SPEED_LIMIT is about 200
 * called from calc_motors()
 * read joystick read analog input 0-1023
 * converts the value to between +/- SPEED_LIMIT
 * any value between +/- DEAD_BAND is set to 0
 * returns value of speed +/- SPEED_LIMIT
 */
 int read_joystck (char analog_input_pin) 
 {
   
    int sensorValue;  // value between 0 - 1023
    int outputValue;  // value between between -SPEED_LIMIT, +SPEED_LIMIT  zero is stopped
    
    sensorValue = analogRead(analog_input_pin); // returns value  between 0 - 1023,  512 is midway
    outputValue = map(sensorValue, 0, 1023, -SPEED_LIMIT, +SPEED_LIMIT);
    //outputValue is a number between -SPEED_LIMIT, +SPEED_LIMIT  zero is stopped
    // typical  -200 to -10,  +10 to 200, values -10 to +10 are forced to zero.
    
    //if((outputValue < DEAD_BAND) && (outputValue > -DEAD_BAND) )  
     // outputValue = 0;   // force the stop command to servos and t-100 motors. add  to 1500 later
     
  if((outputValue > DEAD_BAND)
    outputValue += 25;
  if((outputValue < DEAD_BAND)
    outputValue -= 25;
      
    return outputValue;  //return value between  -SPEED_LIMIT AND + SPEED_LIMIT
}

 
void simple_motors(void)
{
  int motor1_speed;
  int motor2_speed;
  int motor3_speed;
  int motor4_speed;
//  int motor0_speed;
 
      motor1_speed = read_joystck( JOYSTICK_1);  // value between -SPEED_LIMIT  +SPEED_LIMIT
      motor2_speed = read_joystck( JOYSTICK_3);  // value between -SPEED_LIMIT  +SPEED_LIMIT
      motor3_speed = read_joystck( JOYSTICK_4);  // value between -SPEED_LIMIT  +SPEED_LIMIT
      motor4_speed = read_joystck( JOYSTICK_2);  // value between -SPEED_LIMIT  +SPEED_LIMIT
     //  motor4_speed = read_joystck( JOYSTICK_4);  // value between -SPEED_LIMIT  +SPEED_LIMIT
  
      HoldingRegs[THRUSTER_1]= constrain( motor1_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
      HoldingRegs[THRUSTER_2]= constrain( motor2_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
      HoldingRegs[THRUSTER_3]= constrain( motor3_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
      HoldingRegs[THRUSTER_4]= constrain( motor4_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
     // HoldingRegs[THRUSTER_5]= constrain( motor4_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
 
}
char foo(int a, char b, float c)
{
  return(a+b+c);
    
}

