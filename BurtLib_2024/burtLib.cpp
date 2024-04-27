#include "burtLib.h"

#include "Arduino.h"


bool withinSpread(int value, int spread) {
  if (value < spread && value > -spread) return true;
  else return false;
}

bool outofSpread(int value, int spread) {
  if (value > spread || value < -spread) return true;
  else return false;
}

