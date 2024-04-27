#include "motors.h"

#include "burtLib.h"
#include "pinsMapROV.h"
#include "ROV.h"
#include "Arduino.h"

void setupMotors() {
    // Attach servos
    Servo1.attach(SERVO_PIN_1);
    Servo2.attach(SERVO_PIN_2);
    Servo3.attach(SERVO_PIN_3);
    Servo4.attach(SERVO_PIN_4);
    Servo5.attach(SERVO_PIN_5);
    Servo6.attach(SERVO_PIN_6);
    // Send "stop" and initilize signal to ESCs
    Servo1.writeMicroseconds(INIT_SERVO);
    Servo2.writeMicroseconds(INIT_SERVO);
    Servo3.writeMicroseconds(INIT_SERVO);
    Servo4.writeMicroseconds(INIT_SERVO);
    Servo5.writeMicroseconds(INIT_SERVO);
    Servo6.writeMicroseconds(INIT_SERVO);

    delay(2000); //  delay to allow the ESC to recognize the stopped signal

    // set data array motors to 1500
    Holding_Regs[THRUSTER_1] = INIT_SERVO;
    Holding_Regs[THRUSTER_2] = INIT_SERVO;
    Holding_Regs[THRUSTER_3] = INIT_SERVO;
    Holding_Regs[THRUSTER_4] = INIT_SERVO;
    Holding_Regs[THRUSTER_5] = INIT_SERVO;
    Holding_Regs[THRUSTER_6] = INIT_SERVO;
}

void writeMotorSpeeds() {
    static unsigned long motor_update = millis();  // last time the motors were updated.
  
    if ((motor_update + MOTOR_UPDATE_DELAY) < millis()) { // 100 ms don't know how often this should be updated
        motor_update = millis();   // reset timer
        
        // check if speed is over the speed limit and just stop it
        if ((Holding_Regs[THRUSTER_1] < (INIT_SERVO - SPEED_LIMIT)) || (Holding_Regs[THRUSTER_1] > (INIT_SERVO + SPEED_LIMIT))) {
            Holding_Regs[THRUSTER_1] = INIT_SERVO;
            Servo1.writeMicroseconds(Holding_Regs[THRUSTER_1]);       
        } 
        else {
            Servo1.writeMicroseconds(Holding_Regs[THRUSTER_1]); // write to ESC
        }
    }
}