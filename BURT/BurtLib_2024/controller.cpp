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

//returns forward/backward hat value
bool forward() {
    if (Xbox.getAnalogHat(MAP_MOVE) > DEADBAND && outofTol(Xbox.getAnalogHat(MAP_STRAFE), XYTOLERANCE)) return Xbox.getAnalogHat(MAP_MOVE);
    else return false;
}