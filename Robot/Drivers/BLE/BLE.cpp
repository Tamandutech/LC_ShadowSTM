/*
 * BLE.cpp
 *
 *  Created on: Aug 30, 2026
 *      Author: Kelvin Novais
 */

#include "BLE.hpp"

#include "../../peripherals.h"

// A buffer to receive characters (currently we use only the first character)
static uint8_t rx_buffer[32] = {0};

// Here we define the global variable that stores action
volatile Action action = STOP;

void BLEInitialize(void) {
  /*
   * Request UART to recive 1 character.
   *
   * As soon as the character is received, the callback
   * "HAL_UART_RxCpltCallback" is triggered
   */
  HAL_UART_Receive_DMA(&BLE_BUS, (uint8_t *)rx_buffer, 1);
}

extern "C" {

// This callback is called when the UART receives 1 character
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  // Check if the callback was triggered by BLE UART instance, ignore otherwise
  if(huart->Instance == USART1) {
    // Check the received character, and update the action acordingly

    // 1 \0
    switch(rx_buffer[0]) {
    case '2': action = RUN;           break;
    case '3': action = CALIBRATE;     break;
    case '4': action = PRINT_SENSORS; break;
    case '1': action = STOP; break;
    default: break;
    }

    // Request UART to receive another character
    HAL_UART_Receive_DMA(&BLE_BUS, rx_buffer, 1);
  }
}
}
