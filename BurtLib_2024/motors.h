#ifndef motors_h
#define motors_h

#include <Servo.h>

/**
 * Maximum number of motors that can run at a time
*/
#define MAX_MOTORS 2

/**
 * How ofter do ESC write to motors? Between 50 and 100
*/
#define MOTOR_UPDATE_DELAY 75

/**
 * Make servo objects global
*/

extern Servo Servo1;
extern Servo Servo2;
extern Servo Servo3;
extern Servo Servo4;
extern Servo Servo5;
extern Servo Servo6;
extern Servo arm;

/**
 * Checks holding regs for any thruster values not equal to 1500.
 * Any thrusters not 1500 over count of MAX_MOTORS will be set to 1500.
*/
void limitMotors();

/**
 * Set pins, initialize motors, and update holding regs
 * 
 * Includes a 2 SECOND DELAY
*/
void setupMotors();

/**
 * Set motors to spin based off data from HMI
*/
void writeMotorSpeeds();

#endif