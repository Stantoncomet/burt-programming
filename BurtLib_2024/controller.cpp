#include "controller.h"

#include "Arduino.h"
#include <XBOXONE.h>
#include "controlsMap.h"
#include "burtLib.h"
#include "HMI.h"

/**
 * Usb port on shield
*/
USB Usb;
/**
 * Primary drive controller.
*/
XBOXONE Xbox(&Usb);

void setupController() {
    if (Usb.Init() == -1) {
        Serial.print(F("\r\nOSC did not start"));
        while (1); //halt
    }
    Serial.print(F("\r\nXBOX ONE USB Library Started"));
}

// void controllerRoutine() {

//     static int lastMillis = millis();

//     //if (Xbox.XboxOneConnected) analogWrite(A0, 255);
//     if (lastMillis + CONTROLLER_READ_INTERVAL < millis()) {
//         lastMillis = millis();

//         Usb.Task();
//         if (!Xbox.XboxOneConnected) return;
        
//         //Disable rumble because we want things to work properly
//         Xbox.setRumbleOff();

//         /**
//          * Movement controls
//         */
//         verticalMotors();
//         thrustMotors();

//         for (int i = 0; i < HOLDING_REGS_SIZE; i++) {
//             Serial.println(Holding_Regs[i]);
//         }
//         Serial.println("---");
           
        
        
//     }
// }
void controllerRoutine() {
    Usb.Task();

    //if (Xbox.XboxOneConnected) analogWrite(A0, 255);
    if (Xbox.XboxOneConnected) {
        //Disable rumble because we want things to work properly
        Xbox.setRumbleOff();

        verticalMotors();
        thrustMotors();

        for (int i = 0; i < HOLDING_REGS_SIZE; i++) {
            Serial.println(Holding_Regs[i]);
        }
        Serial.println("---");
    }

    delay(1);
}



int readJoystick(char joystick, char axis, bool map_to_speed = true) {
    int xaxis, yaxis;
    int joystick_xvalue, joystick_yvalue; // Value between -32000 and 32000
    int output_value = 0; // Value between -SPEED_LIMIT and + SPEED_LIMIT


    if (joystick == 'L') {
        xaxis = LeftHatX;
        yaxis = LeftHatY;
    }
    if (joystick == 'R') {
        xaxis = RightHatX;
        yaxis = RightHatY;
    }
    joystick_xvalue = constrain(Xbox.getAnalogHat(xaxis), -JOYSTICK_MAX, JOYSTICK_MAX);
    joystick_yvalue = constrain(Xbox.getAnalogHat(yaxis), -JOYSTICK_MAX, JOYSTICK_MAX);

    if (axis == 'X') {
        output_value = joystick_xvalue;
    }
    if (axis == 'Y') {
        output_value = joystick_yvalue;
    }

    if (withinSpread(output_value, JOYSTICK_DEADBAND)) {
        output_value = 0;
        return output_value;
    }

    //Maps joystick val to speed for thruster
    if (map_to_speed) {
        output_value = map(output_value, -JOYSTICK_MAX, JOYSTICK_MAX, -SPEED_LIMIT, SPEED_LIMIT);
    }
 

    return output_value;
}

void verticalMotors() {
    /* a bit wordy but we solve for all 4 combinations of 2 buttons (or inputs)
     *  in practice a dpdt toggle is used spring back to off center. 
        #define PB_UP 2   for reference
        #define PB_DOWN 3   
        /MIN_SPEED (INITSERVO - SPEED_LIMIT)
    */
    #define RAMP_SPEED 20  /// update speed bigger is slower 400 is to slow
    #define RAMP_STEP 10   // step size of speed change smaller is slower
    int up, down; // do this so we do not keep calling digitalRead() many times
    static unsigned last_speed;  // remeber speed last time we were here    
    static unsigned long last_time = millis();  // remeber last time we were here
    
    // Update speed every RAMP_SPEED
    if ((millis() - last_time) > RAMP_SPEED) { 
        last_time = millis();    // update the timer. 
        // look at buttons lets not debounce at this time. 
        up = Xbox.getButtonPress(A);
        down = Xbox.getButtonPress(Y);
        
        if ((!up && !down) || (up && down)) {   // reset motor to stop. (0,0 & 1,1)
            last_speed = INIT_SERVO;  // both false; expected. both true; a problem. 
        }
        
        if (up && !down) {  //go up,  
            if(last_speed < MAX_SPEED)  // yea we can over speed by RAMP_STEP.
                last_speed += RAMP_STEP;          
        }
        
        if (!up && down) {   //go down. 
            if(last_speed > MIN_SPEED)  // yea we can under speed by RAMP_STEP.
                last_speed -= RAMP_STEP;          
        }   
      Holding_Regs[THRUSTER_1] = Holding_Regs[THRUSTER_2] = last_speed; // update this ever RAMP_SPEED microseconds.
   }
}
 
void thrustMotors() {
    int yaxis = readJoystick('L', 'Y');  // return value of -SPEED_LIMIT, +SPEED_LIMIT
    int xaxis = readJoystick('L', 'X');  // return value of -SPEED_LIMIT, +SPEED_LIMIT
    int turn_val = readJoystick('R', 'X');

    Holding_Regs[THRUSTER_3] = Holding_Regs[THRUSTER_4] = Holding_Regs[THRUSTER_5] = Holding_Regs[THRUSTER_6] = INIT_SERVO;


    // Moving stick left
    if (turn_val < 0) {
        Holding_Regs[THRUSTER_5] = turn_val + INIT_SERVO;
        Holding_Regs[THRUSTER_4] = -turn_val + INIT_SERVO;
        return; //leave the function
    }
    // Moving stick right
    if (turn_val > 0) {
        Holding_Regs[THRUSTER_3] = turn_val + INIT_SERVO;
        Holding_Regs[THRUSTER_6] = -turn_val + INIT_SERVO;
        return; //leave the function
    }
    //If not turning, then check movement

    // If joystick is mostly forward 
    if (yaxis > abs(xaxis)) {
        Holding_Regs[THRUSTER_3] = Holding_Regs[THRUSTER_5] = yaxis + INIT_SERVO; //joystick reading plus init signal
        return;
    }
    // backwards
    if (yaxis < -abs(xaxis)) {
        Holding_Regs[THRUSTER_4] = Holding_Regs[THRUSTER_6] = yaxis + INIT_SERVO;
        return;
    }
    // If joystick is mostly port
    if (xaxis > abs(yaxis)) {
        Holding_Regs[THRUSTER_3] = Holding_Regs[THRUSTER_4] = xaxis + INIT_SERVO;
        return;
    }
    // starboard
    if (xaxis < -abs(yaxis)) {
        Holding_Regs[THRUSTER_5] = Holding_Regs[THRUSTER_6] = xaxis + INIT_SERVO;
        return;
    }

    
}