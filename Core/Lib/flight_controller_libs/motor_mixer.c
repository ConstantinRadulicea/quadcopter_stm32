#include "motor_mixer.h"
#include "rate_controller.h"
#include "flight_control_loop.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif /* MAX */

#ifndef CLAMP
#define CLAMP(x, lo, hi) (((x) < (lo)) ? (lo) : ((x) > (hi)) ? (hi) : (x))
#endif /* CLAMP */

#define PID_LIMIT CONTROLLER_PID_MAX_OUTPUT                 // Axis PID clamp

#define MAX_THROTTLE_MIX MAX_THROTTLE
#define AXIS_TO_MOTOR_GAIN (0.25f / PID_LIMIT)  // ±500 → ±0.5 motor range


/*
	ROLL: roll to go left (NEGATIVE ROLL)
	ROLL: roll to go right (POSITIVE ROLL)

	PITCH: pitch to go forward (NEGATIVE PITCH)
	PITCH: pitch to go backward (POSITIVE PITCH)

	YAW: rotate left (CCW) (NEGATIVE YAW)
	YAW: rotate right (CW) (POSITIVE YAW)
 */

static const float MIX[4][3] = {
    //   roll   pitch   yaw
    { +1.0f, +1.0f, -1.0f }, // Front Left  (0) (CW)
    { -1.0f, +1.0f, +1.0f }, // Front Right (1) (CCW)
    { +1.0f, -1.0f, +1.0f }, // Rear  Left  (2) (CCW)
    { -1.0f, -1.0f, -1.0f }  // Rear  Right (3) (CW)
};

// Motor mixer for Quad-X
void motor_mixer_quad_x(
    float throttle_norm,       // 0..1 throttle
    float roll_out, float pitch_out, float yaw_out,  // ±PID_LIMIT
	float throttle_idle,  // e.g., 0..1 idle throttle
    float motor_dshot[NUM_MOTORS]     // output throttle per motor
) {
	// Clamp inputs
	throttle_norm = CLAMP(throttle_norm, 0.0f, MAX_THROTTLE_MIX);
	throttle_idle = CLAMP(throttle_idle, 0.0f, MAX_THROTTLE_MIX);

    // Scale PID outputs into motor fraction
    float r = roll_out * AXIS_TO_MOTOR_GAIN;
    float p = pitch_out * AXIS_TO_MOTOR_GAIN;
    float y = yaw_out * AXIS_TO_MOTOR_GAIN;

    // Step 1: Compute raw motor mix
    float m[NUM_MOTORS];


	// Add throttle + roll + pitch + yaw for each motor
    for (int i = 0; i < NUM_MOTORS; i++) {
        m[i] = throttle_norm + MIX[i][0] * r + MIX[i][1] * p + MIX[i][2] * y;
    }

    // Step 2: Desaturate so all motors stay in [0..1]
    float mmin = m[0], mmax = m[0];
    for (int i = 1; i < NUM_MOTORS; i++) {
        if (m[i] < mmin) mmin = m[i];
        if (m[i] > mmax) mmax = m[i];
    }

	// If any motor is above 1.0, lower all motors by the excess
    if (mmax > MAX_THROTTLE_MIX) {
        float shift = mmax - MAX_THROTTLE_MIX;
        for (int i = 0; i < NUM_MOTORS; i++) m[i] -= shift;
    }

	// If any motor is below 0.0, raise all motors by the deficit
    if (mmin < 0.0f) {
        float shift = 0.0f - mmin;
        for (int i = 0; i < NUM_MOTORS; i++) m[i] += shift;
    }

	// Step 3: Clamp to [throttle_idle..1.0] and write output
    for (int i = 0; i < NUM_MOTORS; i++) {
        m[i] = CLAMP(m[i], throttle_idle, MAX_THROTTLE_MIX);
        motor_dshot[i] = m[i];
    }
}
