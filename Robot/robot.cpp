/*
 * robot.cpp
 *
 *  Created on: Aug 27, 2026
 *      Author: Kelvin Novais
 */

#include "robot.h"
#include "peripherals.h"

#include "Drivers/BLE/BLE.hpp"
#include "Drivers/LineSensor/LineSensor.hpp"
#include "Drivers/Motors/Motors.hpp"

#include <cstdio>
#include <cstring>

#define MESSAGE_MAX_SIZE 256

// Control loop period (the old HAL_Delay(100) was far too slow for a PID)
#define CONTROL_PERIOD_US 2000

// Telemetry period over BLE
#define PRINT_PERIOD_MS 100

/*
 * PID. The error is the line position (-750..+750).
 * Start with only KP, then add KD, and only use KI if really needed.
 * KD is per control cycle (CONTROL_PERIOD_US).
 */
#define BASE_SPEED          250   // PWM 0..999. Start low!
#define KP                  0.15f
#define KI                  0.0f
#define KD                  1.5f
#define INTEGRAL_LIMIT      2000.0f

// Set to -1 if the robot turns AWAY from the line
#define STEERING_SIGN       1

// Vacuum PWM while running (0 = off)
#define VACUUM_RUN_PWM      0

/*
 * Bluetooth name. It is stored in the external Bluetooth module, so the
 * STM32 sends AT commands to it at boot (works with HM-10/JDY-23-like
 * modules; HC-05 needs its special AT mode and will ignore this).
 * After the name shows up correctly on the phone, set BLE_SET_NAME to 0 so
 * the module's memory is not rewritten at every boot.
 */
#define BLE_SET_NAME        1
#define BLE_NAME_COMMAND    "AT+NAMEShadow"
#define BLE_RESET_COMMAND   "AT+RESET"

static char message[MESSAGE_MAX_SIZE] = {0};

static uint32_t controlPeriodTicks = 0;
static uint32_t lastControlTick    = 0;
static uint32_t lastPrintMs        = 0;

static Action previousAction = STOP;

static float integral      = 0.0f;
static float previousError = 0.0f;

// Sends `message` by BLE, only if the previous transmission has finished
static void sendMessage(void) {
  if(BLE_BUS.gState == HAL_UART_STATE_READY) {
    HAL_UART_Transmit_DMA(&BLE_BUS, (uint8_t *)message, strlen(message));
  }
}

static bool printDue(void) {
  const uint32_t now = HAL_GetTick();
  if(now - lastPrintMs >= PRINT_PERIOD_MS) {
    lastPrintMs = now;
    return true;
  }
  return false;
}

static void resetPid(void) {
  integral      = 0.0f;
  previousError = 0.0f;
}

// Called once each time the action changes
static void onActionChange(Action newAction) {
  resetPid();
  Motors::stop();
  Motors::setVacuum(0);

  if(newAction == CALIBRATE) {
    LineSensor::startCalibration();
  }

  if(previousAction == CALIBRATE) {
    snprintf(message, MESSAGE_MAX_SIZE, "Calibration: %lu of %d sensors ok\n",
             (unsigned long)LineSensor::calibratedCount(), (int)N_IR_SENSORS);
    sendMessage();
  }

  if(newAction == RUN) {
    Motors::setVacuum(VACUUM_RUN_PWM);
  }
}

static void stop(void) {
  Motors::stop();
  Motors::setVacuum(0);
}

#if BLE_SET_NAME
// Sends the AT commands that rename the Bluetooth module (must not be
// connected to a phone). Answers from the module are ignored by BLE.cpp,
// since any unknown character just means STOP.
static void setBleName(void) {
  HAL_Delay(200);
  HAL_UART_Transmit(&BLE_BUS, (uint8_t *)BLE_NAME_COMMAND,
                    strlen(BLE_NAME_COMMAND), 100);
  HAL_Delay(300);
  HAL_UART_Transmit(&BLE_BUS, (uint8_t *)BLE_RESET_COMMAND,
                    strlen(BLE_RESET_COMMAND), 100);
  HAL_Delay(800);  // the module restarts
  action = STOP;
}
#endif

static void run(void) {
  LineSensor::update();

  const float error = (float)LineSensor::position();

  integral += error;
  if(integral > INTEGRAL_LIMIT) {
    integral = INTEGRAL_LIMIT;
  } else if(integral < -INTEGRAL_LIMIT) {
    integral = -INTEGRAL_LIMIT;
  }

  const float derivative = error - previousError;
  previousError          = error;

  const float correction =
      STEERING_SIGN * (KP * error + KI * integral + KD * derivative);

  // Line on the right (positive error) -> left wheel faster, turns right
  const int32_t left  = (int32_t)(BASE_SPEED + correction);
  const int32_t right = (int32_t)(BASE_SPEED - correction);
  Motors::set(left, right);

  if(printDue()) {
    snprintf(message, MESSAGE_MAX_SIZE, "pos=%ld L=%ld R=%ld\n",
             (long)LineSensor::position(), (long)left, (long)right);
    sendMessage();
  }
}

static void printSensors(void) {
  LineSensor::update();

  if(!printDue()) {
    return;
  }

  int len = snprintf(message, MESSAGE_MAX_SIZE, "pos=%ld raw:",
                     (long)LineSensor::position());
  for(uint32_t k = 0; k < N_IR_SENSORS && len < MESSAGE_MAX_SIZE - 12; k++) {
    len += snprintf(message + len, MESSAGE_MAX_SIZE - len, " %lu",
                    (unsigned long)LineSensor::raw(k));
  }
  snprintf(message + len, MESSAGE_MAX_SIZE - len, "\n");
  sendMessage();
}

void setup(void) {
  // Initialize base timer
  HAL_TIM_Base_Start(&BASE_TIMER);
  HAL_Delay(50);

  // Initialize ADC (needed for IRSensors and Battery)
  HAL_ADCEx_Calibration_Start(&ADC_1, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&ADC_2, ADC_SINGLE_ENDED);
  HAL_Delay(50);

  // Initialize DMA (needed for IRSensors and Battery)
  HAL_ADC_Start_DMA(&ADC_1, (uint32_t *)adc1_buffer, ADC_BUFFER_SIZE);
  HAL_ADC_Start_DMA(&ADC_2, (uint32_t *)adc2_buffer, ADC_BUFFER_SIZE);
  HAL_Delay(50);

  // Initialize motors PWM timers
  HAL_TIM_PWM_Start(&MOTOR_LEFT_TIMER, MOTOR_LEFT_CHANNEL);
  HAL_TIM_PWM_Start(&MOTOR_RIGHT_TIMER, MOTOR_RIGHT_CHANNEL);
  HAL_TIM_PWM_Start(&VACUUM_TIMER, VACUUM_CHANNEL);
  Motors::stop();
  Motors::setVacuum(0);
  HAL_Delay(50);

  // Initialize BLE (make it listen to 1 character)
  BLEInitialize();

#if BLE_SET_NAME
  setBleName();
#endif

  // Send a Hello World
  snprintf(message, MESSAGE_MAX_SIZE, "Hello World\n");
  HAL_UART_Transmit_DMA(&BLE_BUS, (uint8_t *)message, strlen(message));

  // End here your setup:
  LineSensor::initialize();

  /*
   * BASE_TIMER (TIM2) is free running. The generated code uses prescaler 16-1,
   * but the timer clock here is not 16 MHz, so it is computed instead of
   * assumed: ticks per microsecond = timer clock / (prescaler + 1) / 1e6.
   * (APB1 prescaler is 1, so the timer clock equals PCLK1.)
   */
  const uint32_t timerHz = HAL_RCC_GetPCLK1Freq() / (BASE_TIMER.Init.Prescaler + 1);
  controlPeriodTicks     = (timerHz / 1000000U) * CONTROL_PERIOD_US;
  lastControlTick        = __HAL_TIM_GET_COUNTER(&BASE_TIMER);
}

void loop(void) {
  // Wait for the next control cycle (wrap-safe, TIM2 is 32 bit)
  const uint32_t now = __HAL_TIM_GET_COUNTER(&BASE_TIMER);
  if(now - lastControlTick < controlPeriodTicks) {
    return;
  }
  lastControlTick = now;

  const Action current = action;
  if(current != previousAction) {
    onActionChange(current);
    previousAction = current;
  }

  switch(current) {
  case STOP:
    stop();
    break;

  case RUN:
    run();
    break;

  case CALIBRATE:
    LineSensor::calibrate();
    break;

  case PRINT_SENSORS:
    stop();
    printSensors();
    break;

  default:
    stop();
    break;
  }
}
