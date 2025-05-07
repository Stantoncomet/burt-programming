/**
 * This code is shared between both HMI and ROV.
 * Keep variables that are used in both places here.
 */

#ifndef burtLib_h
#define burtLib_h


/**
 * Limits the top speed of motors. Actual range is between 1100 and 1900.
 * This doesn't account for drag offset. (Actual speed limit would be SPEED_LIMIT+DRAG_OFFSET_LIMIT)
*/
#define SPEED_LIMIT       200

/**
 * Drag offset percent limiter thing? idk, its so the pot output isn't super sensative and
 * turning a little bit wont full-power the thrusters.
 * Ex. `(SPEED_LIMIT * 0.5)` means new max speed with offset is 150%
*/
#define DRAG_OFFSET_LIMIT (SPEED_LIMIT * 0.5)

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