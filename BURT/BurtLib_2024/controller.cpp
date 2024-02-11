#include "Arduino.h"
#include "controller.h"
#include "controlsMap.h"


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
        const MOVEVALUE = forward();
        Serial.print("choo choo  ");
        
        Serial.println(Xbox.getAnalogHat(MAP_STRAFE));
        }

        delay(1); //REPLACE WITH NOT THIS
    }
}

//returns forward/backward hat value
bool forward() {
    if (Xbox.getAnalogHat(MAP_MOVE) > DEADBAND && outofTol(Xbox.getAnalogHat(MAP_STRAFE), XYTOLERANCE)) return Xbox.getAnalogHat(MAP_MOVE);
    else return false;
}