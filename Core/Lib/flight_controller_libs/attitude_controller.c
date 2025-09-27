#include "attitude_controller.h"
#include "geometry2D.h"
#include <string.h>

#define FC_DEFAULT_LEVELD_POSITION_Q ((quaternion){ 1.0f, 0.0f, 0.0f, 0.0f }) ///< Identity quaternion (scalar-first)


void attitude_controller_init(
    attitude_controller_t* fc,
    float rate_gain,
    float max_angle,
    float max_rate
) {
    if (!fc) {
        return;
    }

    // Clear all fields
    memset(fc, 0, sizeof(attitude_controller_t));

	fc->rate_gain = rate_gain;
	fc->max_angle = max_angle;
	fc->max_rate = max_rate;

}


/* Remove yaw (rotation about world Z) from q */
static quaternion quat_remove_yaw(quaternion q)
{
    // Ensure q is unit (good practice)
    q = quatnormalize(&q);

    // Twist around world Z: keep only w and z, renormalize
    quaternion q_twist = { q.w, 0.0f, 0.0f, q.z };

    quaternion q_twist_normalized = quatnormalize(&q_twist);

    quaternion twist_conj = quatconj(&q_twist_normalized);

    // swing = q * conj(twist)  -> orientation with yaw removed
    quaternion q_no_yaw = quatmultiply(&q, &twist_conj);

    // Optional: normalize to clean tiny numeric errors
    return quatnormalize(&q_no_yaw);
}


void attitude_controller_angle_mode_update(
    attitude_controller_t* fc,
    quaternion estimated_q,
    float target_roll,
    float target_pitch,
    float target_yaw_rate
) {
    if (!fc) {
        return;
    }

    target_roll = CLAMP(target_roll, -fc->max_angle, fc->max_angle);
    target_pitch = CLAMP(target_pitch, -fc->max_angle, fc->max_angle);
    target_yaw_rate = CLAMP(target_yaw_rate, -fc->max_rate, fc->max_rate);

    // --- Store current sensor inputs and setpoints ---
    fc->estimated_q = estimated_q;
    fc->target_roll = target_roll;
    fc->target_pitch = target_pitch;
    fc->target_yaw = target_yaw_rate;

    // --- Step 1: Build target quaternion from commanded roll/pitch ---
    angles3D target_rp = { target_roll, target_pitch, 0.0f }; // yaw handled separately
    quaternion target_delta_q = angle2quat(&target_rp);

    // --- Step 2: Compose target quaternion relative to level reference ---
	quaternion temp_default_leveld_position_q = FC_DEFAULT_LEVELD_POSITION_Q;
    quaternion target_q = quatmultiply(&temp_default_leveld_position_q, &target_delta_q);

    // --- Step 3: Orientation error quaternion ---
    quaternion error_q = quat_error(&fc->estimated_q, &target_q);

    quaternion estimated_q_witouth_yaw = quat_remove_yaw(error_q);

    // --- Step 4: Convert error quaternion → rotation vector (rad) ---
    angles3D rotvec_err = quat2rotvec(estimated_q_witouth_yaw);

    fc->calculated_roll_rate = rotvec_err.x * fc->rate_gain;
	fc->calculated_pitch_rate = rotvec_err.y * fc->rate_gain;
	fc->calculated_yaw_rate = target_yaw_rate * fc->rate_gain;

	// --- Step 5: Apply limits ---
	fc->calculated_roll_rate = CLAMP(fc->calculated_roll_rate, -fc->max_rate, fc->max_rate);
	fc->calculated_pitch_rate = CLAMP(fc->calculated_pitch_rate, -fc->max_rate, fc->max_rate);
	fc->calculated_yaw_rate = CLAMP(fc->calculated_yaw_rate, -fc->max_rate, fc->max_rate);

}


void attitude_controller_get_calculated_rate(
    attitude_controller_t* fc,
    float* out_roll_rate,
    float* out_pitch_rate,
    float* out_yaw_rate
) {
    // Ensure valid pointers before copying
    if (!fc) {
        return; // Avoid null pointer access
    }
	if (out_roll_rate) {
		*out_roll_rate = fc->calculated_roll_rate;
	}
	if (out_pitch_rate) {
		*out_pitch_rate = fc->calculated_pitch_rate;
	}
	if (out_yaw_rate) {
		*out_yaw_rate = fc->calculated_yaw_rate;
	}
}
