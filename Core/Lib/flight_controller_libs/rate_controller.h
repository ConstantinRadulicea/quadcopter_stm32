#ifndef RATE_CONTROLLER_H
#define RATE_CONTROLLER_H

#include "pid.h"
#include "filters.h"
#include "geometry2D.h"


/*
	ROLL: roll to go left (NEGATIVE ROLL)
	ROLL: roll to go right (POSITIVE ROLL)

	PITCH: pitch to go forward (NEGATIVE PITCH)
	PITCH: pitch to go backward (POSITIVE PITCH)

	YAW: rotate left (CCW) (NEGATIVE YAW)
	YAW: rotate right (CW) (POSITIVE YAW)
 */


/* ---------------- PID OUTPUT LIMITS ------------------ */
#define CONTROLLER_PID_MAX_OUTPUT radians(500.0f)   ///< Upper limit for PID outputs (before motor mixing)
#define CONTROLLER_PID_MIN_OUTPUT (-CONTROLLER_PID_MAX_OUTPUT)  ///< Lower limit for PID outputs (before motor mixing)


typedef struct rate_controller_s {
    /* ---------------- BODY-FRAME SENSOR DATA ---------------- */
	float body_frame_roll_rate;   ///< Body-frame gyroscope data [rad/s], corrected & aligned to chassis
    float body_frame_pitch_rate;  ///< Body-frame gyroscope data [rad/s], corrected & aligned to chassis
	float body_frame_yaw_rate;    ///< Body-frame gyroscope data [rad/s], corrected & aligned to chassis

    /* ---------------- CONTROL SETPOINTS --------------------- */
    float target_roll_rate;        ///< Target roll rate angle [rad]
    float target_pitch_rate;       ///< Target pitch rate angle [rad]
    float target_yaw_rate;         ///< Target yaw rate [rad/s]

    float prev_target_roll_rate;        
    float prev_target_pitch_rate;
    float prev_target_yaw_rate;

    /* ---------------- CONTROLLER PARAMETERS ---------------- */
    float sample_rate_hz;     ///< Controller loop frequency [Hz]

    /* ---------------- PID CONTROLLERS ----------------------- */
    pid_t pid_roll;           ///< Roll axis PID (X-axis)
    pid_t pid_pitch;          ///< Pitch axis PID (Y-axis)
    pid_t pid_yaw;            ///< Yaw axis PID (Z-axis)

    /* ---------------- PID D-TERM FILTERING ------------------ */
    float pid_roll_k_d;
    float pid_pitch_k_d;
    float pid_yaw_k_d;
    float pid_roll_k_ff;
	float pid_pitch_k_ff;
	float pid_yaw_k_ff;

	pt2_filter_t d_term_pid_roll_filter;   ///< PT2 low-pass filter for roll D-term
	pt2_filter_t d_term_pid_pitch_filter;  ///< PT2 low-pass filter for pitch D-term
	pt2_filter_t d_term_pid_yaw_filter;    ///< PT2 low-pass filter for yaw D-term

	pt1_filter_t ff_term_pid_roll_pt1_filter;    ///< PT1 filter for roll feed-forward term
	pt1_filter_t ff_term_pid_pitch_pt1_filter;   ///< PT1 filter for pitch feed-forward term
	pt1_filter_t ff_term_pid_yaw_pt1_filter;     ///< PT1 filter for yaw feed-forward term

    /* ---------------- PID OUTPUTS ---------------------------- */
    float out_pid_roll;       ///< PID output torque command [-1..1] for roll axis
    float out_pid_pitch;      ///< PID output torque command [-1..1] for pitch axis
    float out_pid_yaw;        ///< PID output torque command [-1..1] for yaw axis
} rate_controller_t;


/**
 * @brief Initialize the rate controller with given parameters.
 *        Sets up PID controllers, D-term filters, and loop timing.
 *
 * @param fc         Pointer to rate controller struct
 * @param sample_rate_hz   Control loop frequency [Hz]
 * @param d_term_pid_filter_cutoff_freq_hz  D-term LPF cutoff [Hz]
 */
void rate_controller_init(
    rate_controller_t* fc,
    float sample_rate_hz,
    float d_term_pid_filter_cutoff_freq_hz,
	float ff_term_pid_filter_cutoff_freq_hz
);

void rate_controller_init_roll_pid(
    rate_controller_t* fc,
    float pid_k_p,
    float pid_k_i,
    float pid_k_d,
    float pid_k_i_limit,
    float pid_k_ff
);

void rate_controller_init_pitch_pid(
	rate_controller_t* fc,
	float pid_k_p,
	float pid_k_i,
	float pid_k_d,
	float pid_k_i_limit,
    float pid_k_ff
);

void rate_controller_init_yaw_pid(
	rate_controller_t* fc,
	float pid_k_p,
	float pid_k_i,
	float pid_k_d,
	float pid_k_i_limit,
    float pid_k_ff
);

/**
 * @brief Run the Rate controller update:
 *        1) Runs PID rate controllers for roll/pitch/yaw
 *
 * @param fc             Pointer to rate controller struct
 * @param sensor_roll_rate   Current roll rate [rad/s]
 * @param sensor_pitch_rate  Current pitch rate [rad/s]
 * @param sensor_yaw_rate    Current yaw rate [rad/s]
 * @param target_roll_rate   Desired roll rate [rad/s]
 * @param target_pitch_rate  Desired pitch rate [rad/s]
 * @param target_yaw_rate    Desired yaw rate [rad/s]
 */
void rate_controller_update(
    rate_controller_t* fc,
    float sensor_roll_rate,
    float sensor_pitch_rate,
    float sensor_yaw_rate,
    float target_roll_rate,
    float target_pitch_rate,
    float target_yaw_rate
);

/**
 * @brief Retrieve the latest PID outputs for roll, pitch, and yaw axes.
 *
 * @param fc             Pointer to rate controller struct
 * @param out_pid_roll   Pointer to store roll PID output [-1..1]
 * @param out_pid_pitch  Pointer to store pitch PID output [-1..1]
 * @param out_pid_yaw    Pointer to store yaw PID output [-1..1]
 */
void rate_controller_get_pid_outputs(
    rate_controller_t* fc,
    float* out_pid_roll,
    float* out_pid_pitch,
    float* out_pid_yaw
);

void rate_controller_reset(rate_controller_t* fc);

#endif // !RATE_CONTROLLER_H
