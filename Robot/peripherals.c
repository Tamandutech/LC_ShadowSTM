/*
 * peripherals.c
 *
 *  Created on: Aug 29, 2026
 *      Author: Kelvin Novais
 */

 #include "peripherals.h"

 const volatile uint32_t *const irSensorsRawValues[N_IR_SENSORS] = {
  // Left
  [L_1] = &adc2_buffer[5],
  [L_2] = &adc1_buffer[6],

  // Center
  [C_1]  = &adc1_buffer[7],
  [C_2]  = &adc2_buffer[4],
  [C_3]  = &adc2_buffer[2],
  [C_4]  = &adc2_buffer[1],
  [C_5]  = &adc2_buffer[0],
  [C_6]  = &adc2_buffer[6],
  [C_7]  = &adc2_buffer[7],
  [C_8]  = &adc1_buffer[3],
  [C_9]  = &adc1_buffer[2],
  [C_10] = &adc1_buffer[1],
  [C_11] = &adc1_buffer[0],
  [C_12] = &adc2_buffer[3],

  // Right
  [R_1] = &adc1_buffer[5],
  [R_2] = &adc1_buffer[4]
};
