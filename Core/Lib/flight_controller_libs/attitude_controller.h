#ifndef ATTITUDE_CONTROLLER_H
#define ATTITUDE_CONTROLLER_H

#include "geometry3D.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
	ROLL: roll to go left (NEGATIVE ROLL)
	ROLL: roll to go right (POSITIVE ROLL)

	PITCH: pitch to go forward (NEGATIVE PITCH)
	PITCH: pitch to go backward (POSITIVE PITCH)

	YAW: rotate left (CCW) (NEGATIVE YAW)
	YAW: rotate right (CW) (POSITIVE YAW)
 */
typedef struct attitude_controller_s {
    /* ---------------- ORIENTATION ESTIMATION ---------------- */
    quaternion estimated_q;  ///< Estimated orientation (world → body) from IMU fusion (e.g., Madgwick)

	float rate_gain;  ///< Gain applied to calculated rates before sending to rate controller
	float max_angle; ///< Maximum tilt angle [rad] for safety limiting
	float max_rate;  ///< Maximum angular rate [rad/s] for safety limiting

    /* ---------------- CONTROL SETPOINTS --------------------- */
    float target_roll;        ///< Target roll angle [rad] (Angle mode)
    float target_pitch;       ///< Target pitch angle [rad] (Angle mode)
    float target_yaw;         ///< Target yaw rate [rad/s] (Angle mode uses yaw rate, not yaw angle)

    float calculated_roll_rate;        ///< calculated roll rate angle [rad]
    float calculated_pitch_rate;       ///< calculated pitch rate angle [rad]
    float calculated_yaw_rate;         ///< calculated yaw rate [rad/s]

} attitude_controller_t;


void attitude_controller_init(
    attitude_controller_t* fc,
	float rate_gain,
	float max_angle,
	float max_rate
);

void attitude_controller_angle_mode_update(
    attitude_controller_t* fc,
    quaternion estimated_q,
    float target_roll,
    float target_pitch,
    float target_yaw_rate
);


void attitude_controller_get_calculated_rate(
    attitude_controller_t* fc,
    float* out_roll_rate,
    float* out_pitch_rate,
    float* out_yaw_rate
);

#ifdef __cplusplus
}
#endif

#endif // ATTITUDE_CONTROLLER_H
