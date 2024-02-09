ROV-Build_4_23_2018.c

/*****************************************
 * ROV-Build_4_23_2018
 * Modify port stbd motors, two up down motors.
 * leave  code for 4 motors at axis.
ROV-Build_5_17
load on ROV running Arduino UNO.
main program for the ROV

Comm from ROV to HMI is not working. ROV can receive and execjute.


* *****************************************/


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

 
// PrintCompass();  // a test routine
}




/*************  compass.c   ****************************************************
LSM9DS1_Basic_I2C.ino
SFE_LSM9DS1 Library Simple Example Code - I2C Interface
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 30, 2015
https://github.com/sparkfun/LSM9DS1_Breakout

The LSM9DS1 is a versatile 9DOF sensor. It has a built-in
accelerometer, gyroscope, and magnetometer. Very cool! Plus it
functions over either SPI or I2C.

This Arduino sketch is a demo of the simple side of the
SFE_LSM9DS1 library. It'll demo the following:
* How to create a LSM9DS1 object, using a constructor (global
  variables section).
* How to use the begin() function of the LSM9DS1 class.
* How to read the gyroscope, accelerometer, and magnetometer
  using the readGryo(), readAccel(), readMag() functions and 
  the gx, gy, gz, ax, ay, az, mx, my, and mz variables.
* How to calculate actual acceleration, rotation speed, 
  magnetic field strength using the calcAccel(), calcGyro() 
  and calcMag() functions.
* How to use the data from the LSM9DS1 to calculate 
  orientation and heading.

Hardware setup: This library supports communicating with the
LSM9DS1 over either I2C or SPI. This example demonstrates how
to use I2C. The pin-out is as follows:
  LSM9DS1 --------- Arduino
   SCL ---------- SCL (A5 on older 'Duinos')
   SDA ---------- SDA (A4 on older 'Duinos')
   VDD ------------- 3.3V
   GND ------------- GND
(CSG, CSXM, SDOG, and SDOXM should all be pulled high. 
Jumpers on the breakout board will do this for you.)

The LSM9DS1 has a maxCompassm voltage of 3.6V. Make sure you power it
off the 3.3V rail! I2C pins are open-drain, so you'll be 
(mostly) safe connecting the LSM9DS1's SCL and SDA pins 
directly to the Arduino.

Development environment specifics:
  IDE: Arduino 1.6.3
  Hardware Platform: SparkFun Redboard
  LSM9DS1 Breakout Version: 1.0

This code is beerware. If you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, 
please buy us a round!

Distributed as-is; no warranty is given.
******************************************************]**
HoldingReg[COMPASS] =  
HoldingReg[TIP] = 
HoldingReg[TILT] = 
*********/


/*******************************
 * printAttitude  resides in ROV
 * updates the HoldingReg[COMPASS], HoldingReg[TIP], HoldingReg[TILT] 
 * lots of fp math takes about one ms to calcuate the mess.
 * remove pitch and roll if pressed for time.
 */

#define LSM9DS1_TEMP_BIAS 27.5
 
void printAttitude(float ax, float ay, float az, float mx, float my, float mz)
{
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));
  
  float heading;
  if (my == 0)
    heading = (mx < 0) ? PI : 0;
  else
    heading = atan2(mx, my);
    
  heading -= DECLINATION * PI / 180;
  
  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);
  else if (heading < 0) heading += 2 * PI;
  
  // Convert everything from radians to degrees:
  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;

  // remeber to subtract 200  at the HMI 
  HoldingRegs[TIP] = (unsigned) pitch + 200;
  HoldingRegs[TILT] = (unsigned) roll + 200;
  HoldingRegs[COMPASS] = (unsigned) heading;
  
}

/*******************************
 * Init_compass  resides in ROV
 * sets the HoldingReg[COMPASS], HoldingReg[TIP], HoldingReg[TILT] = 0
 * if unable to initilize then set Nav_working to false.
 * 
 */
void Init_compass(void)
{
  // Before initializing the IMU, there are a few settings
  // we may need to adjust. Use the settings struct to set
  // the device's communication mode and addresses:
  //lastPrint = millis(); // Update lastPrint time
      
  Compass.settings.device.commInterface = IMU_MODE_I2C;
  Compass.settings.device.mAddress = LSM9DS1_M;
  Compass.settings.device.agAddress = LSM9DS1_AG;
  
  // The above lines will only take effect AFTER calling
  // Compass.begin(), which verifies communication with the IMU
  // and turns it on.


  
  delay(500);   // we can use delay here because nothing else is going on.
  if (!Compass.begin())
  {
    Nav_working = false;    // Could not initilize compass module
    // an idea todo make diagnostic light if nav is working.
 
    HoldingRegs[COMPASS] =
    HoldingRegs[TILT] = 
    HoldingRegs[TIP] = 0;
    
    delay(2000);
       
  }  // !Compass.begin()
  else
    Nav_working = true;

    
}

/*******************************************
 * Update_compass  resides in ROV
 * 
 * updates the compass, tip and tilt
 * not called if  Nav_working = false;  Could not initilize compass module
 * if Nav_working == false then compass, etc returns zero and all done
 * 
 */
void Update_compass() // resides in ROV
 {
 // Update the sensor values whenever new data is available

/*
  if ( Compass.gyroAvailable() )
  {
    // To read from the gyroscope,  first call the
    // readGyro() function. When it exits, it'll update the
    // gx, gy, and gz variables with the most current data.
    Compass.readGyro();
  }
*/
    
  if(Compass.tempAvailable())
  {
    Compass.readTemp();
    HoldingRegs[TEMPERATURE] = (unsigned) conv_temp(Compass.temperature);
    
  }
    
  if ( Compass.accelAvailable() )
  {
    // To read from the accelerometer, first call the
    // readAccel() function. When it exits, it'll update the
    // ax, ay, and az variables with the most current data.
    Compass.readAccel();
  }
  
  if ( Compass.magAvailable() )
  {
    // To read from the magnetometer, first call the
    // readMag() function. When it exits, it'll update the
    // mx, my, and mz variables with the most current data.
    Compass.readMag();
  }
  
  if ((lastPrint + PRINT_SPEED) < millis())
  {
    #ifndef ROV_DEV
      //printGyro();  // Print "G: gx, gy, gz"
     // printAccel(); // Print "A: ax, ay, az"
      //printMag();   // Print "M: mx, my, mz"
    #endif
  // Print the heading and orientation for fun!
    // Call print attitude. The LSM9DS1's mag x and y
    // axes are opposite to the accelerometer, so my, mx are
    // substituted for each other.

    // printAttitude updates Holding Registers
    printAttitude(Compass.ax, Compass.ay, Compass.az, 
                 -Compass.my, -Compass.mx, Compass.mz);
       
    lastPrint = millis(); // Update lastPrint time
  }
     Compass.readTemp();

 }

// this took a bit of effort 
static int conv_temp(int16_t raw_temp)
{
  //return raw_temp / LSM9DS1_TEMP_SCALE + LSM9DS1_TEMP_BIAS;
  return ((int) raw_temp/256. + LSM9DS1_TEMP_BIAS); 
} 
 
void PrintCompass(void)
{
delay(500);
 Serial.print(HoldingRegs[COMPASS]);
 Serial.print(HoldingRegs[TIP]);
 Serial.print(HoldingRegs[TILT]);
 Serial.print(HoldingRegs[TEMPERATURE]);
  
}
//Receiver Code


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
    // this section not tested yet
    if(str[0] == 82)  // "R" read value from register
    {
      // first find the index data format is Windex:value
      i = 0; j = 0;
      
      while(isdigit(str[++i]))
      {
         tempchar[j++] = str[i];
      }
      tempchar[j] = '\0';    // gota finish string
      index = atoi(tempchar);  // now have valid HoldingReg index   
      
      if(index  < HOLDING_REGS_SIZE)
          Write_to_HMI(index);  // This writes back to the HMI contents in HoldingRegs[index]
      //else  do nothing
      // return an error,
    }  //if(str[0] == 82) 
  
  Serial.print(index); Serial.print("  "); Serial.println(HoldingRegs[index]);    
    }  // if Serial.available.
   
} // ROV_communications()




void Write_to_HMI(int index)
{
  // HoldingRegs for index, value contents of that register.
  // should look like W5:1234 
  // write index 5 value of 1234
  char str[5];
  char OutStr[20];
  
  for(int i = 0 ; i<20 ; i++)
    OutStr[i] = '\0';
  
  
  strcpy(OutStr,"W");
  itoa(index, str, 10); //Turn value into a character array 
  strcat(OutStr,str); // now Windex
  strcat(OutStr,":"); // now Windex:
  
  itoa(HoldingRegs[index], str, 10); //Turn value into a character array
  strcat(OutStr,str);  // now Windex:valuee
  
  digitalWrite(TxENABLE, HIGH);
  Serial.print(OutStr); // actual output to HMI ****
  Serial.flush();
  delay(10); // give time to write all. 
  digitalWrite(TxENABLE, LOW);
  // done
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
  static unsigned long Motor_update;  // last time the motors were updated.
  
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

/*

"ROV_Header.h"

*/

/*****************************************
   ROV_header.H
    written for Arduino UNO
    1/20/2017
    s. pugh
    rev for 3 motors using a/d joy stick input
    Rev 3. remove test mode to allow fwd, reverse switch.
    REV 2. Added reverse motor control;

   Test basic program interface to BlueRobotics T100 motor, using their ESC motor control module.
   This uses the arduino PWM server library. the code thinks it is talking to a hobby servo positioner.
   the rate of pulse is not all that critical. the pulse lenght determines the direction and speed.
   1500 us at start is init control and for stop motor +- 25 us. deadband

   Numbers greater than 1500 us is forward direction
   Values less than 1500 us is reverse direction
   range is 1100 to 1900 us pulse width.
   full thrust can be 5 lbs, at about 135 watts , 16 volts. 12.5 amps..
   Avoid running long periods out of water.
   The UNO can support 6 servos
 *  *
 * *****************************************/


#include <Servo.h>  // loads servo class for pwm control of RC servos 


// The SFE_LSM9DS1 library requires both Wire and SPI be
// included BEFORE including the 9DS1 library.
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

// The SFE_LSM9DS1 library requires both Wire and SPI be
// included BEFORE including the 9DS1 library.

LSM9DS1 Compass;

#define ROV_COMM_BUFF 24 
#define INITSERVO 1500  /* this is the init speed of the ESC controller soped */
#define MOTOR_UPDATE_DELAY  100  // units of milliseconds 2-22-18 was 200 ms
#define SPEED_LIMIT 200 // this limits top speed of motor for test. Actual motor range is 1100 to 1900, 
#define CREEP_ENABLE

#define POSITION_LIMIT  600 // used in the manipulator
#define NEG_POS_LIMIT 600   // this can change if required after fitted to mechanical device.
#define POS_POS_LIMIT 600 // this can change if required after fitted to mechanical device.

#define PRINT_SPEED 1250 // 1250 ms between update
static unsigned long lastPrint = 0; // Keep track of print time
#define NUM_CAMERAS 2  // used by vidoe routine, can be 0-4

Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;
Servo Servo5;
#ifdef MANIPULATOR
  Servo Servo6;
#endif

//////////////////////////
// LSM9DS1 Library Init //
//////////////////////////
// Use the LSM9DS1 class to create an object. [Compass] can be
// named anything, we'll refer to that throught the sketch.


///////////////////////
// Example I2C Setup //
///////////////////////
// SDO_XM and SDO_G are both pulled high, so our addresses are:
#define LSM9DS1_M 0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

////////////////////////////
// Sketch Output Settings //
////////////////////////////

// Earth's magnetic field varies by location. Add or subtract
// a declination to get a more accurate heading. Calculate
// your's here:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION +16.3 // Declination (degrees) in Port Townsend, wa



// this is for all the Arduino UNO digital pin numbers. on the digital side of arduino.

enum
{
  Rx,         // Uno Output Pin 0 to Comm port to MAX485 pin 1
  Tx,         // Comm port to MAX485 pin 4
  TxENABLE,   // Rx enable low, tx enable high
  AVAILABLE1,       // AVAILABLE
  AVAILABLE2,
  VIDEO_MUX_A0,     // LSB DG409 5 CHANNEL MUX
  VIDEO_MUX_A1,      // MSB DG409 6 CHANNEL MUX
  SERVOPIN1,  // servo for motor 1. select PWM pin 7
  SERVOPIN2,  // servo for motor 2. select pin 8
  SERVOPIN3,  // servo for motor 3. select pin 9
  SERVOPIN4,  // servo for motor 4. select pin 10
  SERVOPIN5,  // servo for motor 5. select pin 11
  SERVOPIN6,  // servo for motor 6. select pin 12
  SYS_LED    // ON BOARD LED on pin 13
};

/*
* these are the command registers used for communicaiton.
* these are the register numbers in 
* HoldingRegs[HOLDING_REGS_SIZE]
* use enum default values 0,1,2,3,etc
* 
* We could just as well said:
* #define ADC_VAL  0  //this becomes HoldingRegs[0]
* #define PWM_VAL 1   //this becomes HoldingRegs[0]
* or even:
* const int THRUSTER1 = 2;    //this becomes HoldingRegs[0]
* const int THRUSTER2 = 3;    //this becomes HoldingRegs[0]
 */

enum 
{
  // just add or remove registers and your good to go...
  // MODBUS expects unsigned int uint16_t
  // The first register starts at address 0
  // we could also #define ADC_VAL 0
  // or CONST unsigned int ADC_VAL = 0;
  ADC_VAL,     // 0 system voltage need reference
  PWM_VAL,      // not used
  THRUSTER1,  // 2
  THRUSTER2,
  THRUSTER3,
  THRUSTER4,
  THRUSTER5,
  COMPASS,  // 7  //works
  TIP,    //works
  TILT,     //works
  TEMPERATURE,    //works
  VIDEO_CHANNEL,  // 11
  MANIPULATOR1,
  MANIPULATOR2,
  HOLDING_REGS_SIZE // leave this one as last or array so, tada! no math required!

};

unsigned int HoldingRegs[HOLDING_REGS_SIZE]; // Global array of data

int Nav_working;  // global test if nav chip working

void read_camera_select(void); // read selected camera and update the mux.

void WriteMotorSpeed1(void);
void WriteMotorSpeed2(void);
void WriteMotorSpeed3(void);
void WriteMotorSpeed4(void);
void WriteMotorSpeed5(void);
void Write_to_HMI(int);

