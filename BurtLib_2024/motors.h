#ifndef motors_h
#define motors_h

#include <Servo.h>

#define MOTOR_UPDATE_DELAY 75

Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;
Servo Servo5;
Servo Servo6;

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