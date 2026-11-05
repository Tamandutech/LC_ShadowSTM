/*
 * Motors.cpp
 */

#include "Motors.hpp"

#include "../../peripherals.h"

/*
 * Level of the direction pin that makes each wheel go FORWARD.
 * The template example shows GPIO_PIN_SET as forward for the right motor.
 * The left one is not confirmed: if a wheel spins backwards, swap
 * GPIO_PIN_SET <-> GPIO_PIN_RESET on its line.
 */
#define MOTOR_LEFT_FORWARD_LEVEL  GPIO_PIN_SET
#define MOTOR_RIGHT_FORWARD_LEVEL GPIO_PIN_SET

static int32_t clampSpeed(int32_t speed, int32_t minimum, int32_t maximum) {
  if(speed > maximum) {
    return maximum;
  }
  if(speed < minimum) {
    return minimum;
  }
  return speed;
}

static GPIO_PinState oppositeLevel(GPIO_PinState level) {
  return (level == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;
}

static void drive(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState forwardLevel,
                  TIM_HandleTypeDef *timer, uint32_t channel, int32_t speed) {
  const int32_t limited = clampSpeed(speed, -MOTOR_PWM_MAX, MOTOR_PWM_MAX);
  const bool    forward = (limited >= 0);

  HAL_GPIO_WritePin(port, pin, forward ? forwardLevel : oppositeLevel(forwardLevel));
  __HAL_TIM_SET_COMPARE(timer, channel, forward ? limited : -limited);
}

namespace Motors {

void set(int32_t left, int32_t right) {
  drive(MOTOR_LEFT_DIRECTION_PORT, MOTOR_LEFT_DIRECTION_PIN,
        MOTOR_LEFT_FORWARD_LEVEL, &MOTOR_LEFT_TIMER, MOTOR_LEFT_CHANNEL, left);
  drive(MOTOR_RIGHT_DIRECTION_PORT, MOTOR_RIGHT_DIRECTION_PIN,
        MOTOR_RIGHT_FORWARD_LEVEL, &MOTOR_RIGHT_TIMER, MOTOR_RIGHT_CHANNEL, right);
}

void stop(void) {
  __HAL_TIM_SET_COMPARE(&MOTOR_LEFT_TIMER, MOTOR_LEFT_CHANNEL, 0);
  __HAL_TIM_SET_COMPARE(&MOTOR_RIGHT_TIMER, MOTOR_RIGHT_CHANNEL, 0);
}

void setVacuum(int32_t pwm) {
  __HAL_TIM_SET_COMPARE(&VACUUM_TIMER, VACUUM_CHANNEL,
                        clampSpeed(pwm, 0, MOTOR_PWM_MAX));
}

}  // namespace Motors
