#ifndef burtLib_h
#define burtLib_h


/**
 * Limits the top speed of motors. Actual range is between 1100 and 1900
*/
#define SPEED_LIMIT       200

/**
 * Stop signal for thruster servos
*/
#define INIT_SERVO        1500

/**
 * Maximum frequency the thrusters can spin
*/
#define MAX_SPEED (INIT_SERVO + SPEED_LIMIT)

/**
 * Maximum frequency the thrusters can spin in revserse
*/
#define MIN_SPEED (INIT_SERVO - SPEED_LIMIT)

/**
 * Check if a value falls within a range, centered on 0.
 * @param value Value to check.
 * @param spread Half of the range, does the value fall between -`spread` and +`spread`?
 * @return If the value fell in the range.
*/
bool withinSpread(int value, int spread);

/**
 * Check if a value does not fall within a range, centered on 0.
 * Also try `!withinSpread()`.
 * @param value Value to check.
 * @param spread Half of the range, does the value fall outside of -`spread` and +`spread`?
 * @return If the value did not fall in the range.
*/
bool outofSpread(int value, int spread);

#endif