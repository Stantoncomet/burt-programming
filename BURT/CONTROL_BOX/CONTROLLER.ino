#include "controlsMap.h"
#define TOLERANCE      7500
#define XYTOLERANCE      26000


void setupController() {
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nXBOX ONE USB Library Started"));
}


bool withinTol(bool value, int tolerance) {
  if (value < tolerance || value > -tolerance) return true;
  else return false;
}
bool outofTol(bool value, int tolerance) {
  if (value > tolerance || value < -tolerance) return true;
  else return false;
}

bool forward() {
  if (Xbox.getAnalogHat(MAP_MOVE) > TOLERANCE && withinTol(Xbox.getAnalogHat(MAP_STRAFE)), XYTOLERANCE) return true;
  else return false;
}