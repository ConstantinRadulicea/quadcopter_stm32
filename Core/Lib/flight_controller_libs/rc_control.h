#ifndef RC_CONTROL_H
#define RC_CONTROL_H

#include "filters.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RC_MAX_CHANNELS
#define RC_MAX_CHANNELS 16
#endif

typedef struct rc_channel_filter_s{
    int enable_lpf;
	pt1_filter_t lpf;
    float smoothed;
    float raw;
    float prev_raw;
    float deadband;
    float expo;
    float multiplier;
    float sample_rate; // hz
} rc_channel_filter_t;

void rc_channel_filter_init(rc_channel_filter_t* channel, int enable_lpf, float lpf_cutoff_freq_hz, float deadband,float multiplier, float expo, float sample_rate_hz);
void rc_channel_filter_update(rc_channel_filter_t* channel, float raw_input);
float rc_channel_filter_get_processed(rc_channel_filter_t* channel);

/* ---------- 4-axis container ---------- */
typedef struct rc_attitude_control_s {
    rc_channel_filter_t roll_filter;     // expects -1..+1
    rc_channel_filter_t pitch_filter;    // expects -1..+1
    rc_channel_filter_t yaw_filter;      // expects -1..+1
    rc_channel_filter_t throttle_filter; // expects  0..1
} rc_attitude_control_t;

/* =============================== ROLL API ============================================= */
void rc_attitude_control_init_roll(
    rc_attitude_control_t* rc,
    int enable_lpf,
    float lpf_cutoff_freq_hz,
    float deadband,
    float multiplier,
    float expo,
    float sample_rate_hz
);

void rc_attitude_control_update_roll(
	rc_attitude_control_t* rc,
	float raw_input
);

float rc_attitude_control_get_processed_roll(
	rc_attitude_control_t* rc
);

/* =============================== PITCH API ============================================= */
void rc_attitude_control_init_pitch(
	rc_attitude_control_t* rc,
	int enable_lpf,
	float lpf_cutoff_freq_hz,
	float deadband,
	float multiplier,
	float expo,
	float sample_rate_hz
);

void rc_attitude_control_update_pitch(
	rc_attitude_control_t* rc,
	float raw_input
);

float rc_attitude_control_get_processed_pitch(
	rc_attitude_control_t* rc
);

/* =============================== YAW API ============================================= */
void rc_attitude_control_init_yaw(
	rc_attitude_control_t* rc,
	int enable_lpf,
	float lpf_cutoff_freq_hz,
	float deadband,
	float multiplier,
	float expo,
	float sample_rate_hz
);

void rc_attitude_control_update_yaw(
	rc_attitude_control_t* rc,
	float raw_input
);

float rc_attitude_control_get_processed_yaw(
	rc_attitude_control_t* rc
);

/* =============================== THROTTLE API ============================================= */
void rc_attitude_control_init_throttle(
	rc_attitude_control_t* rc,
	int enable_lpf,
	float lpf_cutoff_freq_hz,
	float deadband,
	float multiplier,
	float expo,
	float sample_rate_hz
);

void rc_attitude_control_update_throttle(
	rc_attitude_control_t* rc,
	float raw_input
);

float rc_attitude_control_get_processed_throttle(
	rc_attitude_control_t* rc
);

/* ============================================ ============================================= */

void rc_attitude_control_update(
	rc_attitude_control_t* rc,
	float raw_roll,
	float raw_pitch,
	float raw_yaw,
	float raw_throttle
);

void rc_attitude_control_get_processed(
	rc_attitude_control_t* rc,
	float *roll,
	float *pitch,
	float *yaw,
	float *throttle
);

#ifdef __cplusplus
}
#endif

#endif // !RC_CONTROL_H
