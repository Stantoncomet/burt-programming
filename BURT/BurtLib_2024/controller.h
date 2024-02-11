#ifndef controller_h
#define controller_h

#include "Arduino.h"

#define DEADBAND       7500    // Range where any hat(joystick) input will just be zero
#define XYTOLERANCE    26000   // Range to accept foward/backward movement vs side-to-side

void setupController();
void controllerRoutine();

bool forward();


#endif