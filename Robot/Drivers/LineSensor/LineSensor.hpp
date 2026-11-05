/*
 * LineSensor.hpp
 *
 *  Reads the 16 IR sensors, normalizes them (0 = background, 1000 = line)
 *  and computes the line position as a weighted average.
 */

#ifndef DRIVERS_LINESENSOR_LINESENSOR_HPP_
#define DRIVERS_LINESENSOR_LINESENSOR_HPP_

#include <cstdint>

// Distance between two neighbor sensors, in position units
#define LINE_SENSOR_STEP 100

// The position goes from -LINE_POSITION_MAX (far left) to +LINE_POSITION_MAX
// (far right), 0 meaning the line is centered. (16 sensors -> 750)
#define LINE_POSITION_MAX (15 * LINE_SENSOR_STEP / 2)

namespace LineSensor {

// Loads the default calibration (see LineSensor.cpp)
void initialize(void);

// Clears the min/max values, so a new calibration can start
void startCalibration(void);

// Call periodically while the robot is moved by hand over the line and the
// background. Stores the min and max raw value seen by each sensor.
void calibrate(void);

// Reads the sensors and updates position(). Returns false when no sensor sees
// the line (the position then saturates to the side where the line was last
// seen).
bool update(void);

// Line position computed by the last update()
int32_t position(void);

// Raw ADC value of the k-th sensor (k = 0 is the leftmost)
uint32_t raw(uint32_t k);

// How many sensors have a usable calibration (min and max far enough apart)
uint32_t calibratedCount(void);

}  // namespace LineSensor

#endif /* DRIVERS_LINESENSOR_LINESENSOR_HPP_ */
