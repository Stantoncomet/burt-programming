/*****************************************
 * ROV_Build_3_25_2019
 * 
 * Modify port stbd motors, two up down motors.
 * leave  code for 4 motors at axis.
ROV-Build_5_17
load on ROV running Arduino UNO.
main program for the ROV

Comm from ROV to HMI is not working. ROV can receive and execjute.


* *****************************************/


#define  BUILD_2019   true // We will not erase existing code but rather not use it.

#include "ROV_Header.h"

#include <Servo.h>  // loads servo class for pwm control of RC servos 


// #define MANIPULATOR  // enable if Manipulator is installed.
#define  LED 13     // used for diagnostics.

void setup() 
{
  Serial.begin(9600);   // this is comm link to hmi controller

  // modified from 4 motors to 2 thrusters and 2 up/down + creep
  // no real program changes, just naming.
  Servo1.attach(SERVOPIN1); // Motor 1 port thurster
  Servo2.attach(SERVOPIN2); // Motor 2 up 1 thuster
  Servo3.attach(SERVOPIN3); // Motor 3 stbd thruster
  Servo4.attach(SERVOPIN4); // Motor 4 up 2 thruster
  Servo5.attach(SERVOPIN5); // Motor 5 creep
  #ifdef MANIPULATOR
   Servo6.attach(SERVOPIN6); // this is the  manilptator
  #endif MANIPULATOR 
  Servo1.writeMicroseconds(INITSERVO); // send "stop" and initilize signal to ESC.
  Servo2.writeMicroseconds(INITSERVO); // send "stop" and initilize signal to ESC.
  Servo3.writeMicroseconds(INITSERVO); // send "stop" and initilize signal to ESC.
  Servo4.writeMicroseconds(INITSERVO); // send "stop" and initilize signal to ESC.
  Servo5.writeMicroseconds(INITSERVO); // send "stop" and initilize signal to ESC.
  
  #ifdef MANIPULATOR
    Servo6.writeMicroseconds(INITSERVO); // send "stop" and initilize signal to ESC.
  #endif MANIPULATOR
  
  delay(2000); //  delay to allow the ESC to recognize the stopped signal
  //Init_compass();   
  
  //pinMode (LED, OUTPUT);
  pinMode(TxENABLE, OUTPUT);
  pinMode(VIDEO_MUX_A0, OUTPUT);
  pinMode(VIDEO_MUX_A1, OUTPUT);
  
  digitalWrite(TxENABLE, LOW);
  digitalWrite(VIDEO_MUX_A0, LOW);   // low for camera #1, high for camera #2
  digitalWrite(VIDEO_MUX_A1, LOW);  // if only 2 cameras this should always be low.
  
  HoldingRegs[THRUSTER1] = INITSERVO;
  HoldingRegs[THRUSTER2] = INITSERVO;
  HoldingRegs[THRUSTER3] = INITSERVO;
  HoldingRegs[THRUSTER4] = INITSERVO;
  HoldingRegs[THRUSTER5] = INITSERVO;
  #ifdef MANIPULATOR
     HoldingRegs[MANIPULATOR1] = INITSERVO;
  #endif MANIPULATOR
  
}
/*
 * Basic round robin operating system.
 * each task is taken in turn.
 * first task is read the command array.
 */
 
void loop()
{

  /*
  if (Nav_working)
    //Update_compass();     // read compass and write to HMI registers. collides with Serial..??
   delay(20); 
   */
  read_camera_select(); // read selected camera and update the mux.
  ROV_communications();  // resides in ROV_ascii
  WriteMotorSpeed1();    // HoldingRegs[THRUSTER1] is motor 
  WriteMotorSpeed2();    // HoldingRegs[THRUSTER2] is motor speed
  WriteMotorSpeed3();    // HoldingRegs[THRUSTER3] is motor speed 
  WriteMotorSpeed4();    // HoldingRegs[THRUSTER4] is motor speed
  WriteMotorSpeed5();    // HoldingRegs[THRUSTER5] is motor speed
  #ifdef MANIPULATOR
    WriteMotorSpeed6();    // HoldingRegs[MANIPULATOR1] is motor position
  #endif MANIPULATOR

 
}


/*
 * rov_video_2 comments updated 2-11-2018
 
 * the DG409 analog mux chip is used
 * a possible of four inputs are allowed, selected by the inputs of A0,A1
 * we only use 2 cameras so A0 is either 0,1. while A1 is always 0 
 * but we check that the contents of HoldingRegs[VIDEO_CHANNEL] array
 * is between 0 - 3. In our machine NUM_CAMERAS = 2
 */
void read_camera_select()  // resides in ROV reads VIDEO_CHANNEL 
{
  static int last_selection;  // remeber last selection to avoid flicker
  // test for bad data
  if((HoldingRegs[VIDEO_CHANNEL] < 0) && (HoldingRegs[VIDEO_CHANNEL] > NUM_CAMERAS))
    HoldingRegs[VIDEO_CHANNEL] = 0 ;    // force some limits, in case of errors.
  
  // dont write to switch_video if nothing new to say.
  if (HoldingRegs[VIDEO_CHANNEL]!= last_selection)
  {
    last_selection = HoldingRegs[VIDEO_CHANNEL];
    switch_video(HoldingRegs[VIDEO_CHANNEL]) ;
   // Serial.println(last_selection);
  }
}

/********************
 * although in this itteration there are only 2 cameras
 * we need to deal with the unused pin VIDEO_MUX_A1
 * camera is HoldingRegs[VIDEO_CHANNEL]
 */
void switch_video(int camera)
{
       if (camera == 0)
        {
            digitalWrite(VIDEO_MUX_A0,false);
            digitalWrite(VIDEO_MUX_A1,false);
        }else  
        if (camera == 1)
        {
            digitalWrite(VIDEO_MUX_A0,true);
            digitalWrite(VIDEO_MUX_A1,false);
        }else  
        if (camera == 2)
        {
            digitalWrite(VIDEO_MUX_A0,false);
            digitalWrite(VIDEO_MUX_A1,true);
        }else  
        if (camera == 3)
        {
            digitalWrite(VIDEO_MUX_A0,true);
            digitalWrite(VIDEO_MUX_A1,true);
        }

}

/*
 * Write_to_Motors
 * Module for ROV_
 */

/*
 * * Write_motor_speed()
 * called from loop, read from HoldingRegs[] the set motor
 * speed in microseconds and write to moto.
 * Maybe the ESC controllers do not need to be updated that often.
 * Expects a valid number in HoldingRegs[THRUSTERx]
 * that is a time in micro seconds requried by the servo routine.
 * (HoldingRegs[THRUSTERx] stopped should be 1500 micro seconds +/- SPEED_LIMIT
 * We really do not care how HoldingRegs is filled. 
 * but we should do some basic error checkking
 * 

 * controlling the motors this way is not most efficent but does
 * allow the MOTOR_UPDATE_DELAY to be used for independent motors.
 */
 
void WriteMotorSpeed1(void)
{
  static unsigned long Motor_update = millis();  // last time the motors were updated.
  
  if((Motor_update + MOTOR_UPDATE_DELAY) < millis() ) // 100 ms don't know how often this should be updated
  {
    Motor_update = millis();   // reset timer
       
    if( (HoldingRegs[THRUSTER1] < (INITSERVO - SPEED_LIMIT)) || (HoldingRegs[THRUSTER1] > (INITSERVO + SPEED_LIMIT)))
    {
       HoldingRegs[THRUSTER1] = INITSERVO; // if invalid speed just stop the thing, 
       Servo1.writeMicroseconds(HoldingRegs[THRUSTER1]);       
    }
    else
    {
       Servo1.writeMicroseconds(HoldingRegs[THRUSTER1]); // write to ESC
    }
  }
}    

void WriteMotorSpeed2(void)
{
  static unsigned long Motor_update;  // last time the motors were updated.
  
  if((Motor_update + MOTOR_UPDATE_DELAY) < millis() )
  {
    Motor_update = millis();   // reset timer
    if( (HoldingRegs[THRUSTER2] < (INITSERVO - SPEED_LIMIT)) || (HoldingRegs[THRUSTER2] > (INITSERVO + SPEED_LIMIT)))
    {
       HoldingRegs[THRUSTER2] = INITSERVO; // if invalid speed just stop the thing, 
       Servo2.writeMicroseconds(HoldingRegs[THRUSTER2]);
    }
    else
     Servo2.writeMicroseconds(HoldingRegs[THRUSTER2]); // write to ESC
  }
}    

void WriteMotorSpeed3(void)
{
  static unsigned Motor_update;  // last time the motors were updated.
  
  if((Motor_update + MOTOR_UPDATE_DELAY) < millis() )
  {
    Motor_update = millis();   // reset timer
    if( (HoldingRegs[THRUSTER3] < (INITSERVO - SPEED_LIMIT)) || (HoldingRegs[THRUSTER3] > (INITSERVO + SPEED_LIMIT)))
    {
       HoldingRegs[THRUSTER3] = INITSERVO; // if invalid speed just stop the thing, 
       Servo3.writeMicroseconds(HoldingRegs[THRUSTER3]);
    }
    else
     Servo3.writeMicroseconds(HoldingRegs[THRUSTER3]); // write to ESC
  }
}    


void WriteMotorSpeed4(void)
{
  static unsigned long Motor_update;  // last time the motors were updated.
  
  if((Motor_update + MOTOR_UPDATE_DELAY) < millis() )
  {
    Motor_update = millis();   // reset timer
    if( (HoldingRegs[THRUSTER4] < (INITSERVO - SPEED_LIMIT)) || (HoldingRegs[THRUSTER4] > (INITSERVO + SPEED_LIMIT)))
    {
       HoldingRegs[THRUSTER4] = INITSERVO; // if invalid speed just stop the thing, 
       Servo4.writeMicroseconds(HoldingRegs[THRUSTER4]);
    }
    else
     Servo4.writeMicroseconds(HoldingRegs[THRUSTER4]); // write to ESC
  }
}    

void WriteMotorSpeed5(void)  // this would be the z axis motor
{
  static unsigned long Motor_update;  // last time the motors were updated.
  
  if((Motor_update + MOTOR_UPDATE_DELAY) < millis() )
  {
    Motor_update = millis();   // reset timer
    if( (HoldingRegs[THRUSTER5] < (INITSERVO - SPEED_LIMIT)) || (HoldingRegs[THRUSTER5] > (INITSERVO + SPEED_LIMIT)))
    {
       HoldingRegs[THRUSTER5] = INITSERVO; // if invalid speed just stop the thing, 
       Servo5.writeMicroseconds(HoldingRegs[THRUSTER5]);
    }
    else
     Servo5.writeMicroseconds(HoldingRegs[THRUSTER5]); // write to ESC
  }
}


#ifdef MANIPULATOR
void WriteMotorSpeed6(void)  // this would be the manipulator uses same format as ESC motors
{
  static unsigned long Motor_update ;  // last time the motors were updated.
  
  if((Motor_update + MOTOR_UPDATE_DELAY) < millis() )
  {
    Motor_update = millis();   // reset timer
    if( (HoldingRegs[MANIPULATOR1] < (INITSERVO - NEG_POS_LIMIT)) || (HoldingRegs[MANIPULATOR1] > (INITSERVO + POS_POS_LIMIT)))
    {
       HoldingRegs[MANIPULATOR1] = INITSERVO; // if invalid speed just stop the thing, 
       Servo6.writeMicroseconds(HoldingRegs[MANIPULATOR1]);
    }
    else
     Servo6.writeMicroseconds(HoldingRegs[MANIPULATOR1]); // write to ESC
  }
}  
#endif 


void ROV_communications(void)
{
 
  int i, j;
  int val, index;
  char  str[ROV_COMM_BUFF];
  char tempchar[9];
  
  
  digitalWrite(TxENABLE, LOW);  // read enable, Tx disable
  
  if  (Serial.available() > 0 ) 
  { 
     delay(20);    // data takes 7.2 ms at 9600 baud, (from scope) 
      // wait for all data to be loaded. or use an endstring char like a ";"
      for (i = 0; i < ROV_COMM_BUFF ; i++)    //first clear out the buffer
          str[i] = '\0';
     
      i = 0; j = 0;
      //val = Serial.available();   // used for testing
      while(Serial.available() > 0 ) // read data into char array
      {
        str[i++] =  Serial.read();    // heart of the comm. read all the data and put in str buffer.
      }  // while  (Serial.available)
      
      str[i] = '\0';  // make sure the last byte is null.
        
      if(str[0] == 87)  // have a write value command  'W' to HoldingRegs[index] 
      {
        // first find the index data format is Windex:value
        i = 0; j = 0;
   
       while(isdigit(str[++i]))  // start parsing out the command go until a non digit is seen ":"
       {
          tempchar[j++] = str[i];
       }
       tempchar[j] = '\0';    // gota finish string with a null
       // todo put test that index is between 0 and  HOLDING_REGS_SIZE
       index = atoi(tempchar);  // now have valid HoldingReg index value
        
       // find the value. skip past the ":"
       j = 0;
       while(isdigit(str[++i]))   // continue walking throug str
          tempchar[j++] = str[i];
          
       tempchar[j] = '\0';
       val = atoi(tempchar);
       // todo put test that value is good
       HoldingRegs[index]  =  val;  // this is the value to directly control outputs. 
      // Serial.print(index); Serial.print("  "); Serial.println(HoldingRegs[index]);    
     }  // write to register if(str[0] == 87)

//  *******************************************************************
    
  
  Serial.print(index); Serial.print("  "); Serial.println(HoldingRegs[index]);    
    }  // if Serial.available.
   
} // ROV_communications()





