#ifndef burtLib_h
#define burtLib_h


/**
 * Check if a value falls within a range, centered on 0.
 * @param value Value to check.
 * @param spread Half of the range, does the value fall between -`spread` and +`spread`?
 * @return If the value fell in the range.
*/
bool withinSpread(int value, int spread);

/**
 * Check if a value does not fall within a range, centered on 0.
 * @param value Value to check.
 * @param spread Half of the range, does the value fall outside of -`spread` and +`spread`?
 * @return If the value did not fall in the range.
*/
bool outofSpread(int value, int spread);

/**
 * Map a value in one range to equavelant value in another range. Aka, porportionally scale a value.
 * @param value Value to scale.
 * @param fromMin Minimum value in the value's orignal range.
 * @param fromMax Maximum value in the value's orignal range.
 * @param toMin Minimum value in the value's new range.
 * @param toMax Maximum value in the value's new range.
 * @return A new value, porportionally equivalent to the old.
*/
float mapValue(int value, int fromMin, int fromMax, int toMin, int toMax);


#endif