#ifndef FLIGHT_LOOP_CONTROL_H
#define FLIGHT_LOOP_CONTROL_H

#include "imu.h"
#include "attitude_controller.h"
#include "rate_controller.h"
#include "motor_mixer.h"
#include "rc_control.h"

#define MUTEX_ESP_ENABLE 1
#define FLIGHT_CONTROL_LOOP_ENABLE_TEST 0


#if MUTEX_ESP_ENABLE != 0
#include "FreeRTOS.h"   // Needed for SemaphoreHandle_t
#include "semphr.h"
#endif // MUTEX_ESP_ENABLE != 0

#ifdef __cplusplus
extern "C" {
#endif

#define FLIGHT_CONTROLLER_PID_FREQ_HZ 1000

#define ACCELEROMETER_FILTER_CUTOFF_FREQ_HZ 20
#define ACCELEROMETER_SAMPLE_RATE_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ

#define GYRO_FILTER_CUTOFF_FREQ_HZ 80
#define GYRO_SAMPLE_RATE_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ

#define CONTROLLER_PID_KP 0.135f		// 0.6
#define CONTROLLER_PID_KI 0.135f		// 3.5
#define CONTROLLER_PID_KD 0.0036f		// 0.03
#define CONTROLLER_MAX_INTEGRAL_LIMIT (CONTROLLER_PID_MAX_OUTPUT / 2.0f)
#define CONTROLLER_PID_KFF 0.0f		// 80

#define CONTROLLER_YAW_PID_KP 0.180f		// 2.0
#define CONTROLLER_YAW_PID_KI 0.018f		// 12.0
#define CONTROLLER_YAW_PID_KD 0.0f		// 0.0
#define CONTROLLER_YAW_MAX_INTEGRAL_LIMIT (CONTROLLER_PID_MAX_OUTPUT / 2.0f)
#define CONTROLLER_YAW_PID_KFF 0.0f		// 90
#define MOT_YAW_HEADROOM 0.2f // 20% of maximum motor throttle

#define D_TERM_PID_FILTER_CUTOFF_FREQ_HZ (GYRO_FILTER_CUTOFF_FREQ_HZ / 2)
#define FF_TERM_PID_FILTER_CUTOFF_FREQ_HZ 40

#define RATE_GAIN 4.5f	// LEVEL_P or ATC_ANG_PIT_P
#define MAX_ANGLE radians(45.0f)
#define MAX_RATE radians(360.0f)

#define RC_INPUT_SAMPLE_RATE_HZ 1000
#define RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ 40
#define RC_INPUT_DEADBAND 0.0f

#define THROTTLE_IDLE 0.055f
#define MAX_THROTTLE 1.0f

struct flight_control_loop_s;

typedef coord3D(*fcl_get_gyro_data_fn)(struct flight_control_loop_s* fcl, void* context);
typedef coord3D(*fcl_get_accel_data_fn)(struct flight_control_loop_s* fcl, void* context);
typedef coord3D(*fcl_get_target_attitude_fn)(struct flight_control_loop_s* fcl, void* context);
typedef float (*fcl_get_target_throttle_fn)(struct flight_control_loop_s* fcl, void* context);
typedef void (*fcl_write_throttle_to_motors_fn)(struct flight_control_loop_s* fcl, float motor_throttle[NUM_MOTORS], void* context);


typedef struct flight_control_loop_s{
	imu_t imu;
	rate_controller_t rate_controller;
	attitude_controller_t attitude_controller;
	rc_attitude_control_t rc_attitude_control;
	float motor_throttle[NUM_MOTORS];
	// flags
	int are_esc_armed : 1;

	fcl_get_gyro_data_fn get_gyro_data;
	fcl_get_accel_data_fn get_accel_data;
	fcl_get_target_attitude_fn get_target_attitude;
	fcl_get_target_throttle_fn get_target_throttle;
	fcl_write_throttle_to_motors_fn write_throttle;

	void* gyro_hook_context;
	void* accel_hook_context;
	void* target_attitude_hook_context;
	void* target_throttle_hook_context;
	void* write_throttle_hook_context;

#if MUTEX_ESP_ENABLE != 0
	SemaphoreHandle_t imu_mutex;
	SemaphoreHandle_t rate_controller_mutex;
	SemaphoreHandle_t attitude_controller_mutex;
	SemaphoreHandle_t rc_attitude_control_mutex;
	SemaphoreHandle_t motor_throttle_mutex;
	SemaphoreHandle_t flags_mutex;
#endif // MUTEX_ESP_ENABLE != 0


} flight_control_loop_t;

void flight_control_loop_arm_esc(flight_control_loop_t* fcl);
void flight_control_loop_disarm_esc(flight_control_loop_t* fcl);
int flight_control_loop_are_esc_armed(flight_control_loop_t* fcl);
void flight_control_loop_get_motor_throttle(flight_control_loop_t* fcl, float motors_throttle[NUM_MOTORS]);

void rc_control_tick(flight_control_loop_t* fcl);
void imu_tick(flight_control_loop_t* fcl);

void flight_control_loop_init(flight_control_loop_t* fcl);
void flight_control_loop_tick(flight_control_loop_t* fcl);

#if FLIGHT_CONTROL_LOOP_ENABLE_TEST != 0
void flight_control_loop_test();
#endif

void flight_control_loop_set_gyro_hook(flight_control_loop_t* fcl, fcl_get_gyro_data_fn hook, void* context);
void flight_control_loop_set_accel_hook(flight_control_loop_t* fcl, fcl_get_accel_data_fn hook, void* context);
void flight_control_loop_set_target_attitude_hook(flight_control_loop_t* fcl, fcl_get_target_attitude_fn hook, void* context);
void flight_control_loop_set_target_throttle_hook(flight_control_loop_t* fcl, fcl_get_target_throttle_fn hook, void* context);
void flight_control_loop_set_write_throttle_hook(flight_control_loop_t* fcl, fcl_write_throttle_to_motors_fn hook, void* context);

void flight_control_loop_update_rc_control(flight_control_loop_t* fcl, coord3D target_attitude, float target_throttle);
void flight_control_loop_update_imu(flight_control_loop_t* fcl, coord3D gyro_data, coord3D accel_data);
void flight_control_loop_get_motors_throttle(flight_control_loop_t* fcl, float motors_throttle[NUM_MOTORS]);

#ifdef __cplusplus
}
#endif

#endif // !FLIGHT_LOOP_CONTROL_H
