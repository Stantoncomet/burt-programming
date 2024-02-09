/*****************************************
   ROV_header.H
    written for Arduino UNO
    1/20/2017
    s. pugh
    rev ROV_Build_3_25_2019
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

#ifndef BUILD_2019
      // The SFE_LSM9DS1 library requires both Wire and SPI be
      // included BEFORE including the 9DS1 library.
      #include <Wire.h>
      #include <SPI.h>
      #include <SparkFunLSM9DS1.h>
      
      // The SFE_LSM9DS1 library requires both Wire and SPI be
      // included BEFORE including the 9DS1 library.
      
      LSM9DS1 Compass;
#endif

#define ROV_COMM_BUFF 24 
#define INITSERVO 1500  /* this is the init speed of the ESC controller soped */
#define MOTOR_UPDATE_DELAY  75  // units of milliseconds 2-22-18 was 200 ms
#define SPEED_LIMIT 200 // this limits top speed of motor for test. Actual motor range is 1100 to 1900, 
#define CREEP_ENABLE

#define POSITION_LIMIT  600 // used in the manipulator
#define NEG_POS_LIMIT 600   // this can change if required after fitted to mechanical device.
#define POS_POS_LIMIT 600 // this can change if required after fitted to mechanical device.

// #define PRINT_SPEED 1250 // 1250 ms between update
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

#ifndef BUILD_2019
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

#endif

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

#ifndef BUILD_2019
  int Nav_working;  // global test if nav chip working
#endif

void read_camera_select(void); // read selected camera and update the mux.
void WriteMotorSpeed1(void);
void WriteMotorSpeed2(void);
void WriteMotorSpeed3(void);
void WriteMotorSpeed4(void);
void WriteMotorSpeed5(void);
void Write_to_HMI(int);

