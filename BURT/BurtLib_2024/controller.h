#ifndef controller_h
#define controller_h


/**
 * Range where any hat(joystick) input will just be zero
*/
#define DEADBAND       7500

/**
 * Range to accept foward/backward movement vs side-to-side
*/
#define XYTOLERANCE    20000

void setupController();
void controllerRoutine();

int forward();


#endif