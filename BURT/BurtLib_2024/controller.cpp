#include "controller.h"

#include "Arduino.h"
#include <XBOXONE.h>
#include "controlsMap.h"
#include "burtLib.h"

//Xbox Controller
USB Usb;
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
        //MOVEMENT
        if (Xbox.getButtonPress(MAP_RISE)) {
        //go up
        Serial.println("upping");
        }
        if (Xbox.getButtonPress(MAP_FALL)) {
        //go down
        Serial.println("downing");
        }

        if (forward()) {
        const int MOVE_VALUE = forward();
        Serial.print("choo choo  ");
        
        Serial.println(MOVE_VALUE);
        Serial.println(Xbox.getAnalogHat(MAP_STRAFE));

        }

        delay(1); //REPLACE WITH NOT THIS
        
    }
}

//returns forward/backward hat value
int forward() {
    if (Xbox.getAnalogHat(MAP_MOVE) > DEADBAND && withinSpread(Xbox.getAnalogHat(MAP_STRAFE), XYTOLERANCE)) {
        return Xbox.getAnalogHat(MAP_MOVE);
    } else {
        return 0;
    } 
}