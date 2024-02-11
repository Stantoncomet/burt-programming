#ifndef burtLib_h
#define burtLib_h

#include "Arduino.h"
#include "pinsMap.h"


bool withinTol(bool value, int tolerance);
bool outofTol(bool value, int tolerance);

float mapValue(int value, int fromMin, int fromMax, int toMin, int toMax);


#endif