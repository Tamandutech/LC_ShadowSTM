/*
 * LineSensor.cpp
 */

#include "LineSensor.hpp"

#include "../../peripherals.h"

/*
 * Set to 1 if the raw value is HIGHER over the line than over the background,
 * or 0 if it is LOWER. Use the PRINT_SENSORS command (BLE '4') to check.
 */
#define LINE_IS_HIGH 0

// Calibration used until a calibration is done (12 bit ADC: 0..4095)
#define LINE_DEFAULT_MIN 300
#define LINE_DEFAULT_MAX 3500

// A sensor whose (max - min) is below this value is ignored
#define LINE_MIN_SPAN 400

// Normalized values (0..1000) below this are treated as noise
#define LINE_NOISE_FLOOR 100

// The sum of all normalized values must be at least this to say "line found"
#define LINE_MIN_SUM 300

#define ADC_MAX_VALUE 4095

/*
 * Physical order of the sensors, from left to right.
 * ASSUMPTION: the order of the IRSensors enum is already the physical one.
 * If it is not, just reorder this table.
 */
static const IRSensors sensorOrder[N_IR_SENSORS] = {
  L_1, L_2,
  C_1, C_2, C_3, C_4, C_5, C_6, C_7, C_8, C_9, C_10, C_11, C_12,
  R_1, R_2
};

static int32_t calMin[N_IR_SENSORS];
static int32_t calMax[N_IR_SENSORS];

static int32_t linePosition = 0;

static int32_t readRaw(uint32_t k) {
  return (int32_t)(*irSensorsRawValues[sensorOrder[k]]);
}

// Position of the k-th sensor, in position units
static int32_t sensorPosition(uint32_t k) {
  return (2 * (int32_t)k - (int32_t)(N_IR_SENSORS - 1)) * LINE_SENSOR_STEP / 2;
}

// 0 (background) .. 1000 (line)
static int32_t normalize(uint32_t k) {
  const int32_t span = calMax[k] - calMin[k];
  if(span < LINE_MIN_SPAN) {
    return 0;
  }

  int32_t value = (readRaw(k) - calMin[k]) * 1000 / span;
  if(value < 0) {
    value = 0;
  } else if(value > 1000) {
    value = 1000;
  }

#if LINE_IS_HIGH
  return value;
#else
  return 1000 - value;
#endif
}

namespace LineSensor {

void initialize(void) {
  for(uint32_t k = 0; k < N_IR_SENSORS; k++) {
    calMin[k] = LINE_DEFAULT_MIN;
    calMax[k] = LINE_DEFAULT_MAX;
  }
  linePosition = 0;
}

void startCalibration(void) {
  for(uint32_t k = 0; k < N_IR_SENSORS; k++) {
    calMin[k] = ADC_MAX_VALUE;
    calMax[k] = 0;
  }
}

void calibrate(void) {
  for(uint32_t k = 0; k < N_IR_SENSORS; k++) {
    const int32_t value = readRaw(k);
    if(value < calMin[k]) {
      calMin[k] = value;
    }
    if(value > calMax[k]) {
      calMax[k] = value;
    }
  }
}

bool update(void) {
  int32_t sum      = 0;
  int32_t weighted = 0;

  for(uint32_t k = 0; k < N_IR_SENSORS; k++) {
    int32_t value = normalize(k);
    if(value < LINE_NOISE_FLOOR) {
      value = 0;
    }
    sum      += value;
    weighted += value * sensorPosition(k);
  }

  if(sum >= LINE_MIN_SUM) {
    linePosition = weighted / sum;
    return true;
  }

  // Line lost: keep pointing to the side where it was last seen
  linePosition = (linePosition >= 0) ? LINE_POSITION_MAX : -LINE_POSITION_MAX;
  return false;
}

int32_t position(void) {
  return linePosition;
}

uint32_t raw(uint32_t k) {
  return (uint32_t)readRaw(k);
}

uint32_t calibratedCount(void) {
  uint32_t count = 0;
  for(uint32_t k = 0; k < N_IR_SENSORS; k++) {
    if(calMax[k] - calMin[k] >= LINE_MIN_SPAN) {
      count++;
    }
  }
  return count;
}

}  // namespace LineSensor
