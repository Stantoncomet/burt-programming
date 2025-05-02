#ifndef controller_h
#define controller_h


/**
 * Max joystick value
*/
#define JOYSTICK_MAX       32000

/**
 * Range where any hat(joystick) input will just be zero
*/
#define JOYSTICK_DEADBAND  7500

/**
 * Range to accept foward/backward movement vs side-to-side
*/
#define XYTOLERANCE        20000

/**
 * 
*/
#define CONTROLLER_READ_INTERVAL 10

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
//int getInline();

/**
 * Checks if joystick is pushed left or right, out of deadband, and within tolerances.
 * @return X of movement joystick if success, else `0`.
*/
//int getStrafe();

/**
 * Get joystick value. Limits values, applies deadband, and (by default) maps to a speed value
 * @param joystick Joystick, 'L' or 'R'
 * @param axis Axis, 'X' or 'Y'
 * @param map_to_speed Default to true. If false, returns the actual value of joystick (between -32000 and 32000)
 * @return Joystick value
*/
int readJoystick(char joystick, char axis, bool map_to_speed = true);

/**
 * Includeds up, down and bucking
*/
void verticalMotors();

/**
 * Includes thrusting and turning
*/
void thrustMotors();

void motorDisabler();

#endif