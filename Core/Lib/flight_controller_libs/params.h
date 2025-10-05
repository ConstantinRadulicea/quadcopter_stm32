#ifndef PARAMS_H
#define PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Parameter structure
//------------------------------------------------------------------------------
typedef struct params_s{
    // --- PID gains ---
    float rate_controller_roll_p;
    float rate_controller_roll_i;
    float rate_controller_roll_d;
	float rate_controller_roll_max_integral_limit;
	float rate_controller_roll_kff;

    float rate_controller_pitch_p;
    float rate_controller_pitch_i;
    float rate_controller_pitch_d;
    float rate_controller_pitch_max_integral_limit;
    float rate_controller_pitch_kff;

    float rate_controller_yaw_p;
    float rate_controller_yaw_i;
    float rate_controller_yaw_d;
    float rate_controller_yaw_max_integral_limit;
    float rate_controller_yaw_kff;

    float rate_controller_kff_filter_cutoff_hz;
    float rate_controller_d_term_filter_cutoff_hz;

    // --- Limits ---
    float angle_mode_max_angle_deg;
    float angle_mode_max_rate_rps;
    float angle_mode_max_throttle;
    float angle_mode_rate_gain;	// LEVEL_P

    // --- Filter cutoff frequencies ---
    float gyro_cutoff_hz;
    float accel_cutoff_hz;

    // --- RC controller
    float rc_controller_sample_rate_hz;
    float rc_controller_filter_cutoff_hz;
    float rc_controller_input_deadband;

    // --- Misc ---
    float idle_throttle;
} params_t;

//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------
void params_load_default(params_t *p);
void params_print(const params_t *p);

#ifdef __cplusplus
}
#endif

#endif // PARAMS_H
