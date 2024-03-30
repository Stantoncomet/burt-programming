#include "burtLib.h"

#include "Arduino.h"
#include "pinsMap.h"


bool withinSpread(int value, int spread) {
  if (value < spread && value > -spread) return true;
  else return false;
}

bool outofSpread(int value, int spread) {
  if (value > spread || value < -spread) return true;
  else return false;
}

float mapValue(int value, int fromMin, int fromMax, int toMin, int toMax) {
    float mappedValue = (value * (toMax - toMin)) / (fromMax - fromMin) + toMin;
    return mappedValue;
}
