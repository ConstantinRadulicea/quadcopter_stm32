#include "rate_controller.h"
#include "geometry2D.h"
#include <string.h>
#include <math.h>
#include <float.h>


void rate_controller_init(
    rate_controller_t* fc,
    float sample_rate_hz,
    float d_term_pid_filter_cutoff_freq_hz,
    float ff_term_pid_filter_cutoff_freq_hz
) {
    if (!fc) {
        return;
    }

    // Clear all fields
    memset(fc, 0, sizeof(rate_controller_t));

    // Store sample rate and D-term gain (applied manually in update loop)
    fc->sample_rate_hz = sample_rate_hz;

    // --- Initialize D-term filters (low-pass for noise reduction) ---
    pt2_filter_init_lowpass(&(fc->d_term_pid_roll_filter), d_term_pid_filter_cutoff_freq_hz, sample_rate_hz);
    pt2_filter_init_lowpass(&(fc->d_term_pid_pitch_filter), d_term_pid_filter_cutoff_freq_hz, sample_rate_hz);
    pt2_filter_init_lowpass(&(fc->d_term_pid_yaw_filter), d_term_pid_filter_cutoff_freq_hz, sample_rate_hz);

	// --- Initialize FF-term filters (PT1 for smoothing) ---
	pt1_filter_init_lowpass(&(fc->ff_term_pid_roll_pt1_filter), ff_term_pid_filter_cutoff_freq_hz, sample_rate_hz);
	pt1_filter_init_lowpass(&(fc->ff_term_pid_pitch_pt1_filter), ff_term_pid_filter_cutoff_freq_hz, sample_rate_hz);
	pt1_filter_init_lowpass(&(fc->ff_term_pid_yaw_pt1_filter), ff_term_pid_filter_cutoff_freq_hz, sample_rate_hz);

    // --- Initialize PID controllers (no D gain here; applied separately) ---
    pid_init_with_integral_limit(&(fc->pid_roll), 0.0f, 0.0f, 0.0f, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT, 0.0f);
    pid_init_with_integral_limit(&(fc->pid_pitch), 0.0f, 0.0f, 0.0f, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT, 0.0f);
    pid_init_with_integral_limit(&(fc->pid_yaw), 0.0f, 0.0f, 0.0f, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT, 0.0f);
}

void rate_controller_init_roll_pid(
    rate_controller_t* fc,
    float pid_k_p,
    float pid_k_i,
    float pid_k_d,
    float pid_k_i_limit,
    float pid_k_ff
) {
    pid_init_with_integral_limit(&(fc->pid_roll), pid_k_p, pid_k_i, 0.0f, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT, pid_k_i_limit);
    fc->pid_roll_k_d = pid_k_d;
	fc->pid_roll_k_ff = pid_k_ff;
}

void rate_controller_init_pitch_pid(
    rate_controller_t* fc,
    float pid_k_p,
    float pid_k_i,
    float pid_k_d,
    float pid_k_i_limit,
    float pid_k_ff
) {
    pid_init_with_integral_limit(&(fc->pid_pitch), pid_k_p, pid_k_i, 0.0f, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT, pid_k_i_limit);
    fc->pid_pitch_k_d = pid_k_d;
    fc->pid_pitch_k_ff = pid_k_ff;
}

void rate_controller_init_yaw_pid(
    rate_controller_t* fc,
    float pid_k_p,
    float pid_k_i,
    float pid_k_d,
    float pid_k_i_limit,
    float pid_k_ff
) {
    pid_init_with_integral_limit(&(fc->pid_yaw), pid_k_p, pid_k_i, 0.0f, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT, pid_k_i_limit);
    fc->pid_yaw_k_d = pid_k_d;
    fc->pid_yaw_k_ff = pid_k_ff;
}


void rate_controller_update(
    rate_controller_t* fc,
	float sensor_roll_rate,
	float sensor_pitch_rate,
	float sensor_yaw_rate,
    float target_roll_rate,
    float target_pitch_rate,
    float target_yaw_rate
) {
    if (!fc) {
        return;
    }

    float roll_out;
    float pitch_out;
    float yaw_out;

    // --- Store current sensor inputs and setpoints ---
    fc->prev_target_roll_rate = fc->target_roll_rate;
	fc->prev_target_pitch_rate = fc->target_pitch_rate;
	fc->prev_target_yaw_rate = fc->target_yaw_rate;

    fc->target_roll_rate = target_roll_rate;
    fc->target_pitch_rate = target_pitch_rate;
    fc->target_yaw_rate = target_yaw_rate;

    fc->body_frame_roll_rate = sensor_roll_rate;
	fc->body_frame_pitch_rate = sensor_pitch_rate;
    fc->body_frame_yaw_rate = sensor_yaw_rate;

    float dt = HzToSec(fc->sample_rate_hz);

    if(fabsf(dt) > FLT_EPSILON){

    // --- Step 5: PID control ---
    roll_out = pid_calculate(&fc->pid_roll, target_roll_rate, sensor_roll_rate, dt);
    pitch_out = pid_calculate(&fc->pid_pitch, target_pitch_rate, sensor_pitch_rate, dt);
    yaw_out = pid_calculate(&fc->pid_yaw, target_yaw_rate, sensor_yaw_rate, dt);

    float roll_k_ff = fc->pid_roll_k_ff * ((fc->target_roll_rate - fc->prev_target_roll_rate) / dt);
	float pitch_k_ff = fc->pid_pitch_k_ff * ((fc->target_pitch_rate - fc->prev_target_pitch_rate) / dt);
	float yaw_k_ff = fc->pid_yaw_k_ff * ((fc->target_yaw_rate - fc->prev_target_yaw_rate) / dt);

	roll_k_ff = pt1_filter_apply_lowpass(&fc->ff_term_pid_roll_pt1_filter, roll_k_ff);
	pitch_k_ff = pt1_filter_apply_lowpass(&fc->ff_term_pid_pitch_pt1_filter, pitch_k_ff);
	yaw_k_ff = pt1_filter_apply_lowpass(&fc->ff_term_pid_yaw_pt1_filter, yaw_k_ff);

    // --- Step 6: D-term filtering ---
    float roll_d = pt2_filter_apply_lowpass(&fc->d_term_pid_roll_filter, fc->pid_roll.derivative_error * fc->pid_roll_k_d);
    float pitch_d = pt2_filter_apply_lowpass(&fc->d_term_pid_pitch_filter, fc->pid_pitch.derivative_error * fc->pid_pitch_k_d);
    float yaw_d = pt2_filter_apply_lowpass(&fc->d_term_pid_yaw_filter, fc->pid_yaw.derivative_error * fc->pid_yaw_k_d);

    // Add D-term back to PID outputs
    roll_out += roll_d;
    pitch_out += pitch_d;
    yaw_out += yaw_d;

    // Add FF term back to PID outputs
    roll_out += roll_k_ff;
    pitch_out += pitch_k_ff;
    yaw_out += yaw_k_ff;
    }

    // Clamp to allowed range
    roll_out = CLAMP(roll_out, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT);
    pitch_out = CLAMP(pitch_out, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT);
    yaw_out = CLAMP(yaw_out, CONTROLLER_PID_MIN_OUTPUT, CONTROLLER_PID_MAX_OUTPUT);

    // Store final PID outputs
    fc->out_pid_roll = roll_out;
    fc->out_pid_pitch = pitch_out;
    fc->out_pid_yaw = yaw_out;
}

void rate_controller_reset(rate_controller_t* fc) {
	pt2_filter_reset(&(fc->d_term_pid_roll_filter));
	pt2_filter_reset(&(fc->d_term_pid_pitch_filter));
	pt2_filter_reset(&(fc->d_term_pid_yaw_filter));

    pt1_filter_reset(&(fc->ff_term_pid_roll_pt1_filter));
    pt1_filter_reset(&(fc->ff_term_pid_pitch_pt1_filter));
    pt1_filter_reset(&(fc->ff_term_pid_yaw_pt1_filter));

    pid_reset(&(fc->pid_roll));
    pid_reset(&(fc->pid_pitch));
    pid_reset(&(fc->pid_yaw));

    fc->prev_target_roll_rate = 0.0f;
    fc->prev_target_pitch_rate = 0.0f;
    fc->prev_target_yaw_rate = 0.0f;

    fc->out_pid_roll = 0.0f;
    fc->out_pid_pitch = 0.0f;
    fc->out_pid_yaw = 0.0f;
}


void rate_controller_get_pid_outputs(
    rate_controller_t* fc,
    float* out_pid_roll,
    float* out_pid_pitch,
    float* out_pid_yaw
) {
    // Ensure valid pointers before copying
    if (!fc) {
        return; // Avoid null pointer access
    }
    if (out_pid_roll) {
        *out_pid_roll = fc->out_pid_roll;
    }
    if (out_pid_pitch) {
        *out_pid_pitch = fc->out_pid_pitch;
    }
    if (out_pid_yaw) {
        *out_pid_yaw = fc->out_pid_yaw;
    }
}
