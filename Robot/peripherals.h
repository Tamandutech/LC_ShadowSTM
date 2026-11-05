/*
 * peripherals.h
 *
 *  Created on: Aug 27, 2026
 *      Author: Kelvin Novais
 */

#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include "stm32g4xx_hal.h"

#include "i2c.h"
#include "usart.h"
#include "tim.h"
#include "adc.h"

// ENCODERS
// (I) Left
#define ENCODER_LEFT_TIMER    htim4
#define ENCODER_LEFT_CHANNEL  TIM_CHANNEL_ALL
// (II) Right
#define ENCODER_RIGHT_TIMER   htim3
#define ENCODER_RIGHT_CHANNEL TIM_CHANNEL_ALL

// IMU
#define IMU_BUS hi2c1

// LEDS
#define LEDS_TIMER   htim1
#define LEDS_CHANNEL TIM_CHANNEL_1

// MOTORS
// (I) Left
#define MOTOR_LEFT_DIRECTION_PORT   motor2dir_GPIO_Port
#define MOTOR_LEFT_DIRECTION_PIN    motor2dir_Pin
#define MOTOR_LEFT_TIMER            htim8
#define MOTOR_LEFT_CHANNEL          TIM_CHANNEL_1

// (II) Right
#define MOTOR_RIGHT_DIRECTION_PORT  motor1dir_GPIO_Port
#define MOTOR_RIGHT_DIRECTION_PIN   motor1dir_Pin
#define MOTOR_RIGHT_TIMER           htim8
#define MOTOR_RIGHT_CHANNEL         TIM_CHANNEL_3

// VACUUM
#define VACUUM_TIMER   htim5
#define VACUUM_CHANNEL TIM_CHANNEL_2

// BLE
#define BLE_BUS huart1

// BASE TIMER
#define BASE_TIMER htim2

// ADC BUFFERS
#define ADC_1 hadc1
#define ADC_2 hadc2

extern volatile uint32_t adc1_buffer[ADC_BUFFER_SIZE];
extern volatile uint32_t adc2_buffer[ADC_BUFFER_SIZE];

// IR SENSORS
enum IRSensors {
  // Left
  L_1 = 0,
  L_2,

  // Center
  C_1,
  C_2,
  C_3,
  C_4,
  C_5,
  C_6,
  C_7,
  C_8,
  C_9,
  C_10,
  C_11,
  C_12,

  // Right
  R_1,
  R_2,

  N_IR_SENSORS
};

extern const volatile uint32_t *const irSensorsRawValues[N_IR_SENSORS];

#endif /* PERIPHERALS_H_ */
