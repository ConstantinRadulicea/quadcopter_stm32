#include "flight_control_loop.h"
#include "geometry3D.h"
#include "geometry2D.h"
#include "string.h"
#include <stdio.h>

#if FLIGHT_CONTROL_LOOP_ENABLE_TEST != 0
#include <time.h>
#endif


// mpu9250
//static float accelerometer_bias[3] = { 26.993853f, -24.311260f, -45.125365f };
//static float accelerometer_A_1[3][3] = {
//		{0.996045f, -0.000592f, 0.001492f},
//		{-0.000592f, 0.999093f, 0.003024f},
//		{0.001492f, 0.003024f, 0.989097f}
//};
//static float gyro_bias[3] = { -0.067540f, 0.034873f, -0.004956f };

//mpu6500
static float accelerometer_bias[3] = { 28.275280f, -16.046399f, 45.211378f };
static float accelerometer_A_1[3][3] = {
		{0.998455f, 0.000180f, -0.002872f},
		{0.000180f, 0.997138f, 0.000472f},
		{-0.002872f, 0.000472f, 0.983752f}
};
static float gyro_bias[3] = { -0.010603f, -0.026431f, 0.020599f };


static quaternion ground_default_position_q = { .w = 1.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f };


void flight_control_loop_arm_esc(flight_control_loop_t* fcl) {
	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreTake(fcl->flags_mutex, portMAX_DELAY);
	#endif
	fcl->are_esc_armed = 1;
	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreGive(fcl->flags_mutex);
	#endif
}

void flight_control_loop_disarm_esc(flight_control_loop_t* fcl) {
	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreTake(fcl->flags_mutex, portMAX_DELAY);
	#endif
	fcl->are_esc_armed = 0;
	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreGive(fcl->flags_mutex);
	#endif
}

int flight_control_loop_are_esc_armed(flight_control_loop_t* fcl) {
	int flag;
	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreTake(fcl->flags_mutex, portMAX_DELAY);
	#endif
	flag = fcl->are_esc_armed;
	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreGive(fcl->flags_mutex);
	#endif
	return flag;
}

void flight_control_loop_set_gyro_hook(flight_control_loop_t* fcl, fcl_get_gyro_data_fn hook, void* context) {
	fcl->get_gyro_data = hook;
	fcl->gyro_hook_context = context;
}

void flight_control_loop_set_accel_hook(flight_control_loop_t* fcl, fcl_get_accel_data_fn hook, void* context) {
	fcl->get_accel_data = hook;
	fcl->accel_hook_context = context;
}

void flight_control_loop_set_target_attitude_hook(flight_control_loop_t* fcl, fcl_get_target_attitude_fn hook, void* context) {
	fcl->get_target_attitude = hook;
	fcl->target_attitude_hook_context = context;
}

void flight_control_loop_set_target_throttle_hook(flight_control_loop_t* fcl, fcl_get_target_throttle_fn hook, void* context) {
	fcl->get_target_throttle = hook;
	fcl->target_throttle_hook_context = context;
}

void flight_control_loop_set_write_throttle_hook(flight_control_loop_t* fcl, fcl_write_throttle_to_motors_fn hook, void* context) {
	fcl->write_throttle = hook;
	fcl->write_throttle_hook_context = context;
}


void flight_control_loop_init(flight_control_loop_t* fcl) {
	memset(fcl, 0, sizeof(flight_control_loop_t));

	// Initialize IMU and attitude controller here (not shown)
	imu_init(
		&fcl->imu,
		ACCELEROMETER_FILTER_CUTOFF_FREQ_HZ,
		GYRO_FILTER_CUTOFF_FREQ_HZ,
		ACCELEROMETER_SAMPLE_RATE_HZ,
		GYRO_SAMPLE_RATE_HZ,
		FLIGHT_CONTROLLER_PID_FREQ_HZ
	);

	imu_set_accel_bias(&fcl->imu, (coord3D) { accelerometer_bias[0], accelerometer_bias[1], accelerometer_bias[2] }, accelerometer_A_1);
	imu_set_gyro_bias(&fcl->imu, (coord3D) { gyro_bias[0], gyro_bias[1], gyro_bias[2] });
	imu_set_leveled_attitude(&fcl->imu, ground_default_position_q);

	attitude_controller_init(
		&fcl->attitude_controller,
		RATE_GAIN,
		MAX_ANGLE,
		MAX_RATE
	);

	rate_controller_init(
		&fcl->rate_controller,
		FLIGHT_CONTROLLER_PID_FREQ_HZ,
		D_TERM_PID_FILTER_CUTOFF_FREQ_HZ,
		FF_TERM_PID_FILTER_CUTOFF_FREQ_HZ
	);

	rate_controller_init_roll_pid(
		&fcl->rate_controller,
		CONTROLLER_PID_KP,
		CONTROLLER_PID_KI,
		CONTROLLER_PID_KD,
		CONTROLLER_MAX_INTEGRAL_LIMIT,
		CONTROLLER_PID_KFF
	);

	rate_controller_init_pitch_pid(
		&fcl->rate_controller,
		CONTROLLER_PID_KP,
		CONTROLLER_PID_KI,
		CONTROLLER_PID_KD,
		CONTROLLER_MAX_INTEGRAL_LIMIT,
		CONTROLLER_PID_KFF
	);

	rate_controller_init_yaw_pid(
		&fcl->rate_controller,
		CONTROLLER_YAW_PID_KP,
		CONTROLLER_YAW_PID_KI,
		CONTROLLER_YAW_PID_KD,
		CONTROLLER_YAW_MAX_INTEGRAL_LIMIT,
		CONTROLLER_YAW_PID_KFF
	);

	rc_attitude_control_init_roll(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		MAX_ANGLE,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);

	rc_attitude_control_init_pitch(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		MAX_ANGLE,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);

	rc_attitude_control_init_yaw(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		MAX_ANGLE,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);

	rc_attitude_control_init_throttle(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		MAX_THROTTLE,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);

#if MUTEX_ESP_ENABLE != 0
	fcl->imu_mutex = xSemaphoreCreateMutex();
	fcl->rate_controller_mutex = xSemaphoreCreateMutex();
	fcl->attitude_controller_mutex = xSemaphoreCreateMutex();
	fcl->rc_attitude_control_mutex = xSemaphoreCreateMutex();
	fcl->motor_throttle_mutex = xSemaphoreCreateMutex();
	fcl->flags_mutex = xSemaphoreCreateMutex();
#endif
}


void flight_control_loop_tick(flight_control_loop_t* fcl) {
	// RC input
	coord3D target_attitude = { 0 };
	float target_throttle = 0.0f;

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->rc_attitude_control_mutex, portMAX_DELAY);
#endif
	rc_attitude_control_get_processed(
		&fcl->rc_attitude_control,
		&(target_attitude.x),
		&(target_attitude.y),
		&(target_attitude.z),
		&target_throttle
	);

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->rc_attitude_control_mutex);
#endif

	// Get estimated attitude and body frame accel/gyro
	coord3D body_frame_accel, body_frame_gyro;
	quaternion body_frame_estimated_q;

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->imu_mutex, portMAX_DELAY);
#endif
	imu_get_estimated_data(&fcl->imu, &body_frame_estimated_q, &body_frame_accel, &body_frame_gyro);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->imu_mutex);
#endif


#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->attitude_controller_mutex, portMAX_DELAY);
#endif
	// Update attitude controller
	attitude_controller_angle_mode_update(
		&fcl->attitude_controller,
		body_frame_estimated_q,
		target_attitude.x, // target roll
		target_attitude.y,  // target pitch
		target_attitude.z   // target yaw rate
	);

	float target_roll_rate = 0.0f;
	float target_pitch_rate = 0.0f;
	float target_yaw_rate = 0.0f;
	attitude_controller_get_calculated_rate(
		&fcl->attitude_controller,
		&target_roll_rate,
		&target_pitch_rate,
		&target_yaw_rate
	);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->attitude_controller_mutex);
#endif

	float pid_roll_output = 0.0f;
	float pid_pitch_output = 0.0f;
	float pid_yaw_output = 0.0f;
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->rate_controller_mutex, portMAX_DELAY);
#endif
	if (target_throttle > THROTTLE_IDLE)
	{
		rate_controller_update(
			&fcl->rate_controller,
			body_frame_gyro.x,
			body_frame_gyro.y,
			body_frame_gyro.z,
			target_roll_rate,
			target_pitch_rate,
			target_yaw_rate
		);
	}
	else {
		rate_controller_reset(&fcl->rate_controller);
	}

	// Get PID outputs
	rate_controller_get_pid_outputs(
		&fcl->rate_controller,
		&pid_roll_output,
		&pid_pitch_output,
		&pid_yaw_output
	);

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->rate_controller_mutex);
#endif

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->motor_throttle_mutex, portMAX_DELAY);
#endif
	// Mix PID outputs to motor commands
	if (flight_control_loop_are_esc_armed(fcl) != 0)
	{
		motor_mixer_quad_x(
			target_throttle,
			pid_roll_output,
			pid_pitch_output,
			pid_yaw_output,
			THROTTLE_IDLE,
			fcl->motor_throttle
		);
	}
	else {
		for (int i = 0; i < NUM_MOTORS; i++) {
			fcl->motor_throttle[i] = 0.0f;
		}
	}
	if (fcl->write_throttle) {
		fcl->write_throttle(fcl, fcl->motor_throttle, fcl->write_throttle_hook_context);
	}
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->motor_throttle_mutex);
#endif

}

void flight_control_loop_update_rc_control(flight_control_loop_t* fcl, coord3D target_attitude, float target_throttle) {

	target_attitude.x = CLAMP(target_attitude.x, -1.0f, 1.0f);
	target_attitude.y = CLAMP(target_attitude.y, -1.0f, 1.0f);
	target_attitude.z = CLAMP(target_attitude.z, -1.0f, 1.0f);
	target_throttle = CLAMP(target_throttle, 0.0f, 1.0f);

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->rc_attitude_control_mutex, portMAX_DELAY);
#endif
	rc_attitude_control_update(
		&fcl->rc_attitude_control,
		target_attitude.x,
		target_attitude.y,
		target_attitude.z,
		target_throttle
	);

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->rc_attitude_control_mutex);
#endif
}

void rc_control_tick(flight_control_loop_t* fcl) {
	// RC input
	coord3D target_attitude = { 0 };
	float target_throttle = 0.0f;

	if (fcl->get_target_attitude) {
		target_attitude = fcl->get_target_attitude(fcl, fcl->target_attitude_hook_context);
	}

	if (fcl->get_target_throttle) {
		target_throttle = fcl->get_target_throttle(fcl, fcl->target_throttle_hook_context);
	}

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->rc_attitude_control_mutex, portMAX_DELAY);
#endif
	rc_attitude_control_update(
		&fcl->rc_attitude_control,
		target_attitude.x,
		target_attitude.y,
		target_attitude.z,
		target_throttle
	);

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->rc_attitude_control_mutex);
#endif
}

void flight_control_loop_update_imu(flight_control_loop_t* fcl, coord3D gyro_data, coord3D accel_data) {
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->imu_mutex, portMAX_DELAY);
#endif
	imu_update(&fcl->imu, accel_data, gyro_data);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->imu_mutex);
#endif
}

void imu_tick(flight_control_loop_t* fcl) {
	// Read IMU data
	coord3D gyro_data = { 0 };
	coord3D accel_data = { 0 };
	if (fcl->get_gyro_data) {
		gyro_data = fcl->get_gyro_data(fcl, fcl->gyro_hook_context);
	}
	if (fcl->get_accel_data) {
		accel_data = fcl->get_accel_data(fcl, fcl->accel_hook_context);
	}
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->imu_mutex, portMAX_DELAY);
#endif
	imu_update(&fcl->imu, accel_data, gyro_data);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->imu_mutex);
#endif
}

void flight_control_loop_get_motors_throttle(flight_control_loop_t* fcl, float motors_throttle[NUM_MOTORS]) {
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl->motor_throttle_mutex, portMAX_DELAY);
#endif
	for (size_t i = 0; i < NUM_MOTORS; i++)
	{
		motors_throttle[i] = fcl->motor_throttle[i];
	}
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl->motor_throttle_mutex);
#endif
}

#if FLIGHT_CONTROL_LOOP_ENABLE_TEST != 0
/*================================================= TEST FUNCTIONS =============================================*/
#include <stdlib.h>   // rand, srand
#include <time.h>     // time (for seeding once)

static inline float frand_range(float lo, float hi) {
	return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
}

static void random_seed_once(void) {
	static int seeded = 0;
	if (!seeded) {
		srand((unsigned)time(NULL));   // or use HAL_GetTick() on embedded
		seeded = 1;
	}
}

coord3D get_gyro_data() {
	// Placeholder function to simulate gyro data retrieval
	return (coord3D) { 0.0f, 0.0f, 0.0f };
	return (coord3D) {
		frand_range(-1.0f, 1.0f),
			frand_range(-1.0f, 1.0f),
			frand_range(-1.0f, 1.0f)
	};
}
coord3D get_accel_data() {
	// Placeholder function to simulate acc data retrieval
	return (coord3D) { 1.0f, 0.0f, 0.0f };
	return (coord3D) {
		frand_range(-10.0f, 10.0f),
			frand_range(-10.0f, 10.0f),
			frand_range(-10.0f, 10.0f)
	};
}

coord3D get_target_attitude() {
	// Placeholder function to simulate target attitude retrieval
	return (coord3D) { 0.0f, 0.0f, 0.0f };
	return (coord3D) { 1.0f, -0.5f, -1.0f };
}

float get_target_throttle() {
	// Placeholder function to simulate target throttle retrieval
	return 0.3f;
}

void write_motor_commands(void* gg, float motor_commands[NUM_MOTORS], void* gg2) {
	printf("Motor Commands: FR:%.3f, FL:%.3f, RR:%.3f, RL:%.3f\n", motor_commands[0] * 100.0f, motor_commands[1] * 100.0f, motor_commands[2] * 100.0f, motor_commands[3] * 100.0f);
	// Placeholder function to simulate writing motor commands
}


#define DSHOT_MIN_THROTTLE      48
#define DSHOT_MAX_THROTTLE     	2047
#define DSHOT_RANGE 			(DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)
uint16_t throttle2Dshot_value(float throttle) {
	uint16_t Dshot_value;

	Dshot_value = ((uint16_t)(((float)(DSHOT_RANGE + 1)) * throttle)) + (DSHOT_MIN_THROTTLE - 1);
	if (Dshot_value < (uint16_t)DSHOT_MIN_THROTTLE) {
		Dshot_value = (uint16_t)0;
	}
	else {
		Dshot_value = (uint16_t)(CLAMP(Dshot_value, DSHOT_MIN_THROTTLE, DSHOT_MAX_THROTTLE));
	}

	return Dshot_value;
}


static uint16_t validate_dshot_value(float dshot_value) {
	uint16_t Dshot_value;

	if (dshot_value < 1.0f) {
		Dshot_value = (uint16_t)0;
	}
	else if (dshot_value < (uint16_t)DSHOT_MIN_THROTTLE) {
		Dshot_value = (uint16_t)(dshot_value + (float)DSHOT_MIN_THROTTLE);
	}
	else {
		Dshot_value = (uint16_t)(CLAMP(dshot_value, DSHOT_MIN_THROTTLE, DSHOT_MAX_THROTTLE));
	}

	return Dshot_value;
}




void flight_control_loop_test() {
	random_seed_once();
	flight_control_loop_t fcl;
	flight_control_loop_init(&fcl);

	flight_control_loop_set_gyro_hook(&fcl, get_gyro_data, NULL);
	flight_control_loop_set_accel_hook(&fcl, get_accel_data, NULL);
	flight_control_loop_set_target_attitude_hook(&fcl, get_target_attitude, NULL);
	flight_control_loop_set_target_throttle_hook(&fcl, get_target_throttle, NULL);
	flight_control_loop_set_write_throttle_hook(&fcl, write_motor_commands, NULL);

	flight_control_loop_arm_esc(&fcl);
	//flight_control_loop_disarm_esc(&fcl);

	while (1) {
		Sleep(1); // Simulate 1 ms delay for 1000 Hz loop
		rc_control_tick(&fcl);
		imu_tick(&fcl);
		flight_control_loop_tick(&fcl);
		// Simulate a delay for the control loop frequency
		// In a real system, this would be handled by a timer interrupt or RTOS
	}
}

#endif
