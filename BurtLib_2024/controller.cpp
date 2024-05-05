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


//this string determines if the motors can be on or not

 bool Motors_Enabled = true;



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
//             Serial.println(Holding_Regs_HMI[i]);
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

        motorDisabler();

        if (Motors_Enabled) {
            verticalMotors();
            thrustMotors();
        }

        



        //debug
        /*burtLib
        for (int i = 0; i < HOLDING_REGS_SIZE; i++) {
            Serial.print(Holding_Regs_HMI[i]);
            Serial.print(", ");
        }
        Serial.println();
        */
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
    // Cap joystick values at -32000 and 32000
    joystick_xvalue = constrain(Xbox.getAnalogHat(xaxis), -JOYSTICK_MAX, JOYSTICK_MAX);
    joystick_yvalue = constrain(Xbox.getAnalogHat(yaxis), -JOYSTICK_MAX, JOYSTICK_MAX);

    if (axis == 'X') {
        output_value = joystick_xvalue;
    }
    if (axis == 'Y') {
        output_value = joystick_yvalue;
    }

    // Apply deadband of [7500]
    if (withinSpread(output_value, JOYSTICK_DEADBAND)) {
        output_value = 0;
        return output_value;
    }

    // Maps joystick val to speed for thruster
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
    
    int buck_val = readJoystick('R', 'Y'); // return value of -SPEED_LIMIT, +SPEED_LIMIT
    int turn_val = readJoystick('R', 'X'); // used for if checks

    // If bucking down
    if (buck_val < -abs(turn_val)) {
        Holding_Regs_HMI[THRUSTER_1] = buck_val + INIT_SERVO;
        Holding_Regs_HMI[THRUSTER_2] = -buck_val + INIT_SERVO;
        return; // Exit function so we don't get overridden by up/down buttons
    }
    // Bucking up
    if (buck_val > abs(turn_val)) {
        Holding_Regs_HMI[THRUSTER_1] = -buck_val + INIT_SERVO;
        Holding_Regs_HMI[THRUSTER_2] = buck_val + INIT_SERVO;
        return;
    }

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
      Holding_Regs_HMI[THRUSTER_1] = Holding_Regs_HMI[THRUSTER_2] = last_speed; // update this ever RAMP_SPEED microseconds.
   }
}
 
void thrustMotors() {
    int yaxis = readJoystick('L', 'Y');  // return value of -SPEED_LIMIT, +SPEED_LIMIT
    int xaxis = readJoystick('L', 'X');  // return value of -SPEED_LIMIT, +SPEED_LIMIT
    int turn_val = readJoystick('R', 'X'); //
    int buck_val = readJoystick('R', 'Y'); // used for if checks

    Holding_Regs_HMI[THRUSTER_3] = Holding_Regs_HMI[THRUSTER_4] = Holding_Regs_HMI[THRUSTER_5] = Holding_Regs_HMI[THRUSTER_6] = INIT_SERVO;


    // Moving stick left
    if (turn_val < -abs(buck_val)) {
        Holding_Regs_HMI[THRUSTER_5] = turn_val + INIT_SERVO;
        Holding_Regs_HMI[THRUSTER_4] = turn_val + INIT_SERVO;
        return; //leave the function
    }
    // Moving stick right
    if (turn_val > abs(buck_val)) {
        Holding_Regs_HMI[THRUSTER_3] = turn_val + INIT_SERVO;
        Holding_Regs_HMI[THRUSTER_6] = turn_val + INIT_SERVO;
        return; //leave the function
    }
    //If not turning, then check movement

    // If joystick is mostly forward 
    if (yaxis > abs(xaxis)) {
        Holding_Regs_HMI[THRUSTER_3] =
        Holding_Regs_HMI[THRUSTER_5] = yaxis + INIT_SERVO; //joystick reading plus init signal
        return;
    }
    // backwards
    if (yaxis < -abs(xaxis)) {
        Holding_Regs_HMI[THRUSTER_4] = 
        Holding_Regs_HMI[THRUSTER_6] = yaxis + INIT_SERVO;
        return;
    }
    // If joystick is mostly port
    if (xaxis > abs(yaxis)) {
        Holding_Regs_HMI[THRUSTER_3] = 
        Holding_Regs_HMI[THRUSTER_4] = -xaxis + INIT_SERVO;
        return;
    }
    // starboard
    if (xaxis < -abs(yaxis)) {
        Holding_Regs_HMI[THRUSTER_5] = 
        Holding_Regs_HMI[THRUSTER_6] = -xaxis + INIT_SERVO;
        return;
    }

    
}


void motorDisabler() {
    //variables containing the state of each of the thrusters inputs

    int Rise_State = 0;
    int Fall_State = 0;
    int Strafe_State = 0;
    int Move_State = 0;
    int Turn_State = 0;
    int Buck_State = 0;

    //this shows how many motor control inputs are recieved 

    int Input_Level = Rise_State + Fall_State + Strafe_State + Move_State + Turn_State + Buck_State;

    if (Xbox.getButtonPress(Y)) {
     Rise_State = 2;              //if an input is recieved for rising, store that in its "state," is 2 because there are 2 motors running to rise
     //Serial.println("Y");         //print willl be removed
    } else {                                
     Rise_State = 0;
    }

    if (Xbox.getButtonPress(A)) {
      Fall_State = 2;
      //Serial.println("A");
    } else {
      Fall_State = 0;
    }

     if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500) {
      Strafe_State = 2;
      //Serial.println(Xbox.getAnalogHat(LeftHatX));
    } else {
      Strafe_State = 0;
    }

    if (Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500) {
      Move_State = 2;
      //Serial.println(Xbox.getAnalogHat(LeftHatY));
    } else {
      Move_State = 0;
    }

    if (Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500) {
      Turn_State = 2;
      //Serial.println(Xbox.getAnalogHat(RightHatX));
    } else {
      Turn_State = 0;
    }

    if (Xbox.getAnalogHat(RightHatY) > 7500 || Xbox.getAnalogHat(RightHatY) < -7500) {
      Buck_State = 2;
      //Serial.println(Xbox.getAnalogHat(RightHatY));
    } else {
      Buck_State = 0;
    }

    if (Input_Level > 2) {
      Motors_Enabled = false;   //if too many inputs are registered for motor control, this will disable all motors. used so only 2 motors can run at a time.
    } else {
      Motors_Enabled = true;
    }

  Input_Level = Rise_State + Fall_State + Strafe_State + Move_State + Turn_State + Buck_State;

  //Serial.println(Motors_Enabled); //instead of printing, in real code it will be if Motor_Enabler = "Disabled" motor = off

}