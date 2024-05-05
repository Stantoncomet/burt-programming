#include "motors.h"

#include "burtLib.h"
#include "pinsMapROV.h"
#include "ROV.h"
#include "Arduino.h"


/**
 * Define objects like servos in the .cpp files after using extern
*/
Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;
Servo Servo5;
Servo Servo6;

Servo Servos[] {
    Servo1,
    Servo2,
    Servo3,
    Servo4,
    Servo5,
    Servo6
};


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
    Holding_Regs_ROV[THRUSTER_1] = INIT_SERVO;
    Holding_Regs_ROV[THRUSTER_2] = INIT_SERVO;
    Holding_Regs_ROV[THRUSTER_3] = INIT_SERVO;
    Holding_Regs_ROV[THRUSTER_4] = INIT_SERVO;
    Holding_Regs_ROV[THRUSTER_5] = INIT_SERVO;
    Holding_Regs_ROV[THRUSTER_6] = INIT_SERVO;
}

void limitMotors() {
    int running_motors = 0; // motors currently running
    // if theres a thruster entry that is running, then increase running motors
    for (int i = THRUSTER_1; i < THRUSTER_6; i++) {
        if (Holding_Regs_ROV[i] != INIT_SERVO) {
            // set any thrusters over the max runnign limit to 1500 (stop)
            if (++running_motors > MAX_MOTORS) {
                Holding_Regs_ROV[i] = INIT_SERVO;
            }
        }
    }
}

void writeMotorSpeeds() {
    static unsigned long motor_update = millis();  // last time the motors were updated.
  
    if ((motor_update + MOTOR_UPDATE_DELAY) < millis()) { // 75 ms don't know how often this should be updated
        motor_update = millis();   // reset timer
        

        //Servo1.writeMicroseconds(Holding_Regs_ROV[THRUSTER_1]);
        //Serial.print(Holding_Regs_ROV[THRUSTER_1]);
        //Serial.println(", writing to ESC 1");
        // loop over every motor
        for (int i = THRUSTER_1; i < THRUSTER_6; i++) {
            // check if speed is over the speed limit and just stop it
            if ((Holding_Regs_ROV[i] < (INIT_SERVO - SPEED_LIMIT)) || (Holding_Regs_ROV[i] > (INIT_SERVO + SPEED_LIMIT))) {
                Holding_Regs_ROV[i] = INIT_SERVO;
                Servos[i].writeMicroseconds(Holding_Regs_ROV[i]);
                Serial.println(Holding_Regs_ROV[i]);
            } 
            else {
                Servos[i].writeMicroseconds(Holding_Regs_ROV[i]); // write to ESC
            }
        }
        
    }
}