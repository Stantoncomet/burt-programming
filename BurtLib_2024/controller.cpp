#include "controller.h"

#include "Arduino.h"
#include <XBOXONE.h>
#include "controlsMap.h"
#include "burtLib.h"

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

void controllerRoutine() {
    Usb.Task();

    if (Xbox.XboxOneConnected) {
        //Disable rumble because we want things to work properly
        Xbox.setRumbleOff();

        /**
         * Movement controls
        */
        if (Xbox.getButtonPress(MAP_RISE)) {
            //go up
            Serial.println("upping");
        }
        if (Xbox.getButtonPress(MAP_FALL)) {
            //go down
            Serial.println("downing");
        }

        if (getInline()) {
            Serial.print("choo choo  ");
            Serial.println(getInline());
        }
        if (getStrafe()) {
            Serial.print("cha cha  ");
            Serial.println(getStrafe());
        }


        delay(1); //REPLACE WITH NOT THIS
        
    }
}

int getInline() {
    int jiVal = capValue(Xbox.getAnalogHat(MAP_MOVE), JOYSTICK_MAX);
    int jsVal = capValue(Xbox.getAnalogHat(MAP_STRAFE), JOYSTICK_MAX);
    // Check if move joystick x pos is out of the deadband, and the y pos within
    if ((!withinSpread(jiVal, DEADBAND)) && abs(jiVal) > abs(jsVal)) {
        return jiVal;
    } else {
        return 0;
    } 
}

int getStrafe() {
    int jiVal = capValue(Xbox.getAnalogHat(MAP_MOVE), JOYSTICK_MAX);
    int jsVal = capValue(Xbox.getAnalogHat(MAP_STRAFE), JOYSTICK_MAX);
    //Check or whatever
    if ((!withinSpread(jsVal, DEADBAND)) && abs(jsVal) > abs(jiVal)) {
        return jsVal;
    } else {
        return 0;
    } 
}

//dumb dont use please
int driveStick() {
    /*int poses[2] = { Xbox.getAnalogHat(MAP_MOVE), Xbox.getAnalogHat(MAP_STRAFE) };
    if (!withinSpread(Xbox.getAnalogHat(MAP_MOVE), DEADBAND)) {
        poses[0] = 0;
    }
    if (!withinSpread(Xbox.getAnalogHat(MAP_STRAFE), DEADBAND)) {
        poses[1] = 0;
    }
    Serial.print(poses[0]);
    Serial.print(",  ");
    Serial.println(poses[1]);*/
    int value = max(abs(Xbox.getAnalogHat(MAP_MOVE)), abs(Xbox.getAnalogHat(MAP_STRAFE)));
    if (value < DEADBAND) {
        value = 0;
    }
    return value;
}