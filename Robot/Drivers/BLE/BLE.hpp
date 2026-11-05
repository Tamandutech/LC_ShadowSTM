/*
 * BLE.hpp
 *
 *  Created on: Aug 30, 2026
 *      Author: Kelvin Novais
 */

#ifndef DRIVERS_BLE_BLE_HPP_
#define DRIVERS_BLE_BLE_HPP_


// Enumerate here the actions you desire for your robot
enum Action {
  STOP,          // BLE '1' (also any unknown character)
  RUN,           // BLE '2'
  CALIBRATE,     // BLE '3': move the robot by hand over line and background
  PRINT_SENSORS  // BLE '4': prints the raw sensor values, motors stopped

  // ...
};

// This makes the action available for every file that includes this header
extern volatile Action action;


void BLEInitialize(void);


#endif /* DRIVERS_BLE_BLE_HPP_ */
