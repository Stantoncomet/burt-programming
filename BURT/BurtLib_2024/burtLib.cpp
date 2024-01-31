#include "Arduino.h"
#include "burtLib.h"

//check if value falls within -range to +range
bool withinTol(bool value, int tolerance) {
  if (value < tolerance || value > -tolerance) return true;
  else return false;
}

//check if value does not fall within -range to +range
bool outofTol(bool value, int tolerance) {
  if (value > tolerance || value < -tolerance) return true;
  else return false;
}

//map value in one range to equavelant value in another range
float mapValue(int value, int fromMin, int fromMax, int toMin, int toMax) {
    float mappedValue = (value * (toMax - toMin)) / (fromMax - fromMin) + toMin;
    return mappedValue;
}
