#include "rc_control.h"
#include <math.h>
#include <string.h>
#include "geometry2D.h"

void rc_channel_filter_init(rc_channel_filter_t* channel, int enable_lpf, float lpf_cutoff_freq_hz, float deadband,float multiplier,  float expo, float sample_rate_hz) {
	memset(channel, 0, sizeof(rc_channel_filter_t));

	channel->enable_lpf = enable_lpf;
	if (enable_lpf) {
		pt1_filter_init_lowpass(&channel->lpf, lpf_cutoff_freq_hz, sample_rate_hz);
	}
	channel->smoothed = 0.0f;
	channel->raw = 0.0f;
	channel->prev_raw = 0.0f;
	channel->deadband = deadband;
	channel->multiplier = multiplier;
	channel->expo = expo;
	channel->sample_rate = sample_rate_hz;
}

void rc_channel_filter_update(rc_channel_filter_t* channel, float raw_input) {
	channel->prev_raw = channel->raw;
	channel->raw = raw_input;

	float processed_input = raw_input;

	// Apply deadband
	if (fabsf(processed_input) < channel->deadband) {
		processed_input = 0.0f;
	}
	else {
		// Scale to compensate for deadband
		if (processed_input > 0.0f) {
			processed_input = (processed_input - channel->deadband) / (1.0f - channel->deadband);
		}
		else {
			processed_input = (processed_input + channel->deadband) / (1.0f - channel->deadband);
		}
	}

	// Apply exponential curve
	if (channel->expo != 0.0f) {
		processed_input = copysignf(powf(fabsf(processed_input), 1.0f + channel->expo), processed_input);
	}

	processed_input = processed_input * channel->multiplier;

	if (channel->enable_lpf) {
		processed_input = pt1_filter_apply_lowpass(&channel->lpf, processed_input);
	}
	channel->smoothed = processed_input;
}

float rc_channel_filter_get_processed(rc_channel_filter_t* channel) {
	return channel->smoothed;
}

/* =============================== ROLL API ============================================= */
void rc_attitude_control_init_roll(
	rc_attitude_control_t* rc,
	int enable_lpf,
	float lpf_cutoff_freq_hz,
	float deadband,
	float multiplier,
	float expo,
	float sample_rate_hz
) {
	rc_channel_filter_init(&rc->roll_filter, enable_lpf, lpf_cutoff_freq_hz, deadband, multiplier, expo, sample_rate_hz);
}

void rc_attitude_control_update_roll(
	rc_attitude_control_t* rc,
	float raw_input
) {
	raw_input = CLAMP(raw_input, -1.0f, 1.0f);
	rc_channel_filter_update(&rc->roll_filter, raw_input);
}

float rc_attitude_control_get_processed_roll(
	rc_attitude_control_t* rc
) {
	return rc_channel_filter_get_processed(&rc->roll_filter);
}

/* =============================== PITCH API ============================================= */
void rc_attitude_control_init_pitch(
	rc_attitude_control_t* rc,
	int enable_lpf,
	float lpf_cutoff_freq_hz,
	float deadband,
	float multiplier,
	float expo,
	float sample_rate_hz
) {
	rc_channel_filter_init(&rc->pitch_filter, enable_lpf, lpf_cutoff_freq_hz, deadband, multiplier, expo, sample_rate_hz);
}

void rc_attitude_control_update_pitch(
	rc_attitude_control_t* rc,
	float raw_input
) {
	raw_input = CLAMP(raw_input, -1.0f, 1.0f);
	rc_channel_filter_update(&rc->pitch_filter, raw_input);
}

float rc_attitude_control_get_processed_pitch(
	rc_attitude_control_t* rc
) {
	return rc_channel_filter_get_processed(&rc->pitch_filter);
}

/* =============================== YAW API ============================================= */
void rc_attitude_control_init_yaw(
	rc_attitude_control_t* rc,
	int enable_lpf,
	float lpf_cutoff_freq_hz,
	float deadband,
	float multiplier,
	float expo,
	float sample_rate_hz
) {
	rc_channel_filter_init(&rc->yaw_filter, enable_lpf, lpf_cutoff_freq_hz, deadband, multiplier, expo, sample_rate_hz);
}

void rc_attitude_control_update_yaw(
	rc_attitude_control_t* rc,
	float raw_input
) {
	raw_input = CLAMP(raw_input, -1.0f, 1.0f);
	rc_channel_filter_update(&rc->yaw_filter, raw_input);
}

float rc_attitude_control_get_processed_yaw(
	rc_attitude_control_t* rc
) {
	return rc_channel_filter_get_processed(&rc->yaw_filter);
}

/* =============================== THROTTLE API ============================================= */
void rc_attitude_control_init_throttle(
	rc_attitude_control_t* rc,
	int enable_lpf,
	float lpf_cutoff_freq_hz,
	float deadband,
	float multiplier,
	float expo,
	float sample_rate_hz
) {
	rc_channel_filter_init(&rc->throttle_filter, enable_lpf, lpf_cutoff_freq_hz, deadband, multiplier, expo, sample_rate_hz);
}

void rc_attitude_control_update_throttle(
	rc_attitude_control_t* rc,
	float raw_input
) {
	raw_input = CLAMP(raw_input, 0.0f, 1.0f);
	rc_channel_filter_update(&rc->throttle_filter, raw_input);
}

float rc_attitude_control_get_processed_throttle(
	rc_attitude_control_t* rc
) {
	return rc_channel_filter_get_processed(&rc->throttle_filter);
}
/* ========================================================================================== */

void rc_attitude_control_update(
	rc_attitude_control_t* rc,
	float raw_roll,
	float raw_pitch,
	float raw_yaw,
	float raw_throttle
) {
	rc_attitude_control_update_roll(rc, raw_roll);
	rc_attitude_control_update_pitch(rc, raw_pitch);
	rc_attitude_control_update_yaw(rc, raw_yaw);
	rc_attitude_control_update_throttle(rc, raw_throttle);

}

void rc_attitude_control_get_processed(
	rc_attitude_control_t* rc,
	float* roll,
	float* pitch,
	float* yaw,
	float* throttle
) {
	if (roll) {
		*roll = rc_attitude_control_get_processed_roll(rc);
	}
	if (pitch) {
		*pitch = rc_attitude_control_get_processed_pitch(rc);
	}

	if (yaw) {
		*yaw = rc_attitude_control_get_processed_yaw(rc);
	}

	if (throttle) {
		*throttle = rc_attitude_control_get_processed_throttle(rc);
	}
}