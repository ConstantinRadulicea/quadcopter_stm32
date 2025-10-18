#ifndef RTOS_FLIGHT_CONTROLLER_H
#define RTOS_FLIGHT_CONTROLLER_H

#include "flight_control_loop.h"

#ifdef __cplusplus
extern "C" {
#endif

/* User Configuration */
// Timer Clock
#define TIMER_CLOCK				84000000 // 84 MHz

#define MOTOR_1_TIM             (&htim2)
#define MOTOR_1_TIM_CHANNEL     TIM_CHANNEL_1	// FL PA15

#define MOTOR_2_TIM             (&htim2)
#define MOTOR_2_TIM_CHANNEL     TIM_CHANNEL_2	// FR PB3

#define MOTOR_3_TIM             (&htim2)
#define MOTOR_3_TIM_CHANNEL     TIM_CHANNEL_3	// RL PA2

#define MOTOR_4_TIM             (&htim2)
#define MOTOR_4_TIM_CHANNEL     TIM_CHANNEL_4	// RR PA3

#define ESC_PWM_HZ 400
#define ESC_PWM_MIN_US 1000		// at 14.0v, 1140 us, the motor start to spin
#define ESC_PWM_MAX_US 2000

#define MAIN_LOOP_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ
#define RC_CONTROLLER_HZ RC_INPUT_SAMPLE_RATE_HZ
#define TELEMETRY_TASK_HZ 20
#define WRITE_MOTOR_TASK_HZ ESC_PWM_HZ

extern flight_control_loop_t fcl;

void app_main_start(void *argument);

#ifdef __cplusplus
}
#endif

#endif
