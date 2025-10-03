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

#define ACCELEROMETER_FILTER_CUTOFF_FREQ_HZ 100
#define ACCELEROMETER_SAMPLE_RATE_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ

#define GYRO_FILTER_CUTOFF_FREQ_HZ 100
#define GYRO_SAMPLE_RATE_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ

#define CONTROLLER_PID_KP radians(300.0f)		// 50
#define CONTROLLER_PID_KI radians(100.0f)		// 70
#define CONTROLLER_PID_KD radians(0.0f)		// 20
#define CONTROLLER_MAX_INTEGRAL_LIMIT (CONTROLLER_PID_MAX_OUTPUT * 0.3f)
#define CONTROLLER_PID_KFF radians(0.0f)	// 80

#define CONTROLLER_YAW_PID_KP radians(300.0f)	// 45
#define CONTROLLER_YAW_PID_KI radians(100.0f)		// 70
#define CONTROLLER_YAW_PID_KD radians(0.0f)		// 0
#define CONTROLLER_YAW_MAX_INTEGRAL_LIMIT (CONTROLLER_MAX_INTEGRAL_LIMIT)
#define CONTROLLER_YAW_PID_KFF radians(0.0f)	// 90

#define D_TERM_PID_FILTER_CUTOFF_FREQ_HZ 20
#define FF_TERM_PID_FILTER_CUTOFF_FREQ_HZ 40

#define RATE_GAIN radians(50.0f)	// LEVEL_P
#define MAX_ANGLE radians(50.0f)
#define MAX_RATE radians(600.0f)

#define RC_INPUT_SAMPLE_RATE_HZ 100
#define RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ 40
#define RC_INPUT_DEADBAND 0.06f

#define THROTTLE_IDLE 0.055f
#define MAX_THROTTLE 0.5f

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
