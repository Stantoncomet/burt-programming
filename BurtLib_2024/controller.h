#ifndef controller_h
#define controller_h

/**
 * Max joystick value
*/
#define JOYSTICK_MAX   32000

/**
 * Range where any hat(joystick) input will just be zero
*/
#define DEADBAND       7500

/**
 * Range to accept foward/backward movement vs side-to-side
*/
#define XYTOLERANCE    20000


/**
 * Initialize xbox controller.
*/
void setupController();

/**
 * Checks for inputs on controller and reacts accordingly.
 * Only runs if controller is connected.
*/
void controllerRoutine();

/**
 * Checks if joystick is pushed forward or backward, out of deadband, and within tolerances.
 * @return Y of movement joystick if success, else `0`.
*/
int getInline();

/**
 * Checks if joystick is pushed left or right, out of deadband, and within tolerances.
 * @return X of movement joystick if success, else `0`.
*/
int getStrafe();

//dumb function too lazy to remove but not lazy enough to write this desc
int driveStick();

#endif