/*
 * Motors.hpp
 *
 *  Drives the two traction motors (PWM + direction pin) and the vacuum.
 */

#ifndef DRIVERS_MOTORS_MOTORS_HPP_
#define DRIVERS_MOTORS_MOTORS_HPP_

#include <cstdint>

// Maximum PWM value (TIM8 and TIM5 have ARR = 999)
#define MOTOR_PWM_MAX 999

namespace Motors {

// Speed in [-MOTOR_PWM_MAX, MOTOR_PWM_MAX]. Negative means reverse.
// Values outside the range are clamped.
void set(int32_t left, int32_t right);

// Sets both motors to 0
void stop(void);

// Vacuum PWM in [0, MOTOR_PWM_MAX]
void setVacuum(int32_t pwm);

}  // namespace Motors

#endif /* DRIVERS_MOTORS_MOTORS_HPP_ */
