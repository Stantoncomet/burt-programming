/*****************************************
 * HMI_header.H
 * build BUILD_2_3_22
 * build HMI_BUILD_5_10_2109
 *  written for Arduino Mega
 *  4_23_18 
 *  rev for port stbd motors, two vertical axis motors. 
 *  rev for crab motor.
 *  motor assignments are wired in the rov, Rov code stays the same.
 *  the twist pot is reassigned used for side side crab.
 *  
 *  3/20/2017
 *  s. pugh 
 *  rev for 3 motors using a/d joy stick input
 *  Rev 3. remove test mode to allow fwd, reverse switch. 
 *  REV 2. Added reverse motor control;
 * 
 * Test basic program interface to BlueRobotics T100 motor, using their ESC motor control module.
 * This uses the arduino PWM server library. the code thinks it is talking to a hobby servo positioner.
 * the rate of pulse is not all that critical. the pulse lenght determines the direction and speed. 
 * 1500 us at start is init control and for stop motor +- 25 us. deadband
 * 
 * Numbers greater than 1500 us is forward direction
 * Values less than 1500 us is reverse direction
 * range is 1100 to 1900 us pulse width. 
 * full thrust can be 5 lbs, at about 135 watts , 16 volts. 12.5 amps.. 
 * Avoid running long periods out of water. 
 * The UNO can support 6 servos
 *  * 
 * *****************************************/

#include <Servo.h>  // loads servo class for pwm control of RC servos 

// #define FOUR_MOTORS   defined for a motor at each corner.
// #define CRAB_ENABLE    true // rotational analog knob for left - right motor.
// #define CRAB_JOYSTICK  true // rotational analog knob for left - right motor.  ould also be push button 
#define DEAD_BAND 10   // what is correct value? was 5 used for T100 motors

#define INITSERVO 1500  /* this is the init speed of the ESC controlleror stopped motor */
#define MOTOR_UPDATE_DELAY  50  // units of milliseconds
// speed_limit can be a
#define SPEED_LIMIT 200 // this limits top speed of motor for test. Actual motor range is 1100 to 1900, 

#define MIN_SPEED (INITSERVO - SPEED_LIMIT)
#define MAX_SPEED (INITSERVO + SPEED_LIMIT)

#define POSITION_LIMIT   600
#define NEG_POS_LIMIT 600   // this can change if required after fitted to mechanical device.
#define POS_POS_LIMIT 600 // this can change if required after fitted to mechanical device.

#define NUM_CAMERAS 2    // 2 cameras makes 
#define DISPLAY_DELAY 1000  // update the vfd once per second
// #define PB_MOTORS           // use this if the motors are controlled by push buttons rather than analog
#define PB_UP 2   
#define PB_DOWN 3   
#define CAMERA_SELECT   4       // the pin number of the video select pb for testing
#define DISPLAY_SELECT   5       // the number of the vfd display select pb for testing
#define PB_LEFT 6               // aadded for crab control not used but keep reserved.
#define PB_RIGHT 7              // added for crab control  not used but keep reserved.
#define TxENABLE 22       // the number of the Tx ENABLE
#define LEDPIN  13 // on board led used for testing
 // analog inputs
#define JOYSTICK_1  A1  // Select A/D input  pin     
#define JOYSTICK_2  A2  // Select A/D input  pin     
#define JOYSTICK_3  A0  // Select A/D input  pin     
// #define JOYSTICK_3  A3 // Select A/D input  pin    // MANULIPATOR1
#define JOYSTICK_4  A4  // Select A/D input  pin    // MANULIPATOR1
#define KICK_UP_SPEED 50
#define BIG_STEP 25       // add 25 us to moter speed to jump over ESC deadband of 25 us




#define DEBOUNCEDELAY 50    // debounce time in ms.
#define VIDEO_SWITCH_DELAY 1000

void vertical_motor(void);
void Thrust_motors(void);
void vertical_motor(void);
void camera_select(void);
void Write_to_ROV(int index);
void read_camera_select(void);
//d button2(int);
void button3(int);
int debounce2(int);
void Update_ROV_Data(void);
void Two_motor_control(void);
int Nav_display(void);  // called from loop()
void Read_ROV_Data(int index);  // not used but sorta works


/*
 * 
 * hmi_header.h
 * This is a perfect place for an enum
 * these are the addresses for the data array that is sent to the ROV
 * could use something like const int PORT_THRUSTER = 2
 * HoldingRegs[] holds the data sent to the ROV
 * '
 * The labels have been updated using the port - starboard thruster motor config
 */

enum  // this HAS to match the same in the ROV
{     
    // just add or remove registers and your good to go...
    // MODBUS expects unsigned int uint16_t
    // The first register starts at address 0
    ADC_VAL,     // 0 system voltage need reference
    PWM_VAL,      // not used
    THRUSTER_1,  // 2
    THRUSTER_2,  //
    THRUSTER_3,  // 
    THRUSTER_4,  // 
    THRUSTER_5,  // 
    COMPASS,  // 7  // keep compass to temp group together. 
    TIP,    //works
    TILT,     //works     
    TEMPERATURE,    //works
    VIDEO_CHANNEL,  // 11
    MANIPULATOR1,
    MANIPULATOR2, 
    HOLDING_REGS_SIZE // leave this one as last or array size
    
};


unsigned int HoldingRegs[HOLDING_REGS_SIZE]; // Global array of data
