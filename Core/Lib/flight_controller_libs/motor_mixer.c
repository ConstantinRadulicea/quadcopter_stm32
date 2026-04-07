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

#define MAX_THROTTLE_MIX MAX_MOTOR_THROTTLE
//#define AXIS_TO_MOTOR_GAIN ((0.25f * MAX_THROTTLE_MIX) / PID_LIMIT)  // ±400 → ±0.5 motor range
//#define AXIS_TO_MOTOR_GAIN ((0.5f * MAX_THROTTLE_MIX) / PID_LIMIT)  // ±400 → ±1.0 motor range
#define AXIS_TO_MOTOR_GAIN ((1.0f * MAX_THROTTLE_MIX) / PID_LIMIT)


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
// Motor mixer for Quad-X
void motor_mixer_quad_x(
    float throttle_norm,                             // 0..1 throttle
    float roll_out, float pitch_out, float yaw_out,  // ±PID_LIMIT
    float throttle_idle,                             // e.g., 0..1 idle throttle
    float motor_dshot[NUM_MOTORS]                    // output throttle per motor
) {
    float motor_yaw[NUM_MOTORS];

    // Clamp inputs
    throttle_norm = CLAMP(throttle_norm, 0.0f, MAX_THROTTLE_MIX);
    throttle_idle = CLAMP(throttle_idle, 0.0f, MAX_THROTTLE_MIX);

    // Scale PID outputs into motor fraction
    float r = roll_out * AXIS_TO_MOTOR_GAIN;
    float p = pitch_out * AXIS_TO_MOTOR_GAIN;
    float y = yaw_out * AXIS_TO_MOTOR_GAIN;

    // CORECȚIE BUG 1: Permitem Yaw negativ pentru rotirea pe ambele sensuri
    y = CLAMP(y, -MAX_THROTTLE_MIX, MAX_THROTTLE_MIX) * MOT_YAW_HEADROOM;

    // Step 1: Compute raw motor mix (BAZA DE SUPRAVIEȚUIRE)
    float m[NUM_MOTORS];

    for (int i = 0; i < NUM_MOTORS; i++) {
        // CORECȚIE BUG 2: Aici NU includem Yaw. Salvăm exclusiv Roll și Pitch!
        m[i] = throttle_norm + MIX[i][0] * r + MIX[i][1] * p;

        // Calculăm cât Yaw va fi cerut separat pentru fiecare motor
        motor_yaw[i] = MIX[i][2] * y;
    }

    // Step 2: Desaturate base mix (Roll/Pitch Priority)
    float mmin = m[0], mmax = m[0];
    for (int i = 1; i < NUM_MOTORS; i++) {
        if (m[i] < mmin) mmin = m[i];
        if (m[i] > mmax) mmax = m[i];
    }

    // If any motor is above 1.0, lower all motors by the excess (Sacrificăm Throttle)
    if (mmax > MAX_THROTTLE_MIX) {
        float shift = mmax - MAX_THROTTLE_MIX;
        for (int i = 0; i < NUM_MOTORS; i++) m[i] -= shift;
    }

    // If any motor is below 0.0, raise all motors by the deficit (AirMode logic)
    if (mmin < 0.0f) {
        float shift = 0.0f - mmin;
        for (int i = 0; i < NUM_MOTORS; i++) m[i] += shift;
    }

    // Step 3: Verificăm saturația cu Yaw inclus (Shrink logic)
    float max_total = MAX(
        MAX(m[0] + motor_yaw[0], m[1] + motor_yaw[1]),
        MAX(m[2] + motor_yaw[2], m[3] + motor_yaw[3])
    );

    // Scalăm Yaw-ul dacă depășește limita totală
    float yaw_scale = 1.0f;
    if (max_total > MAX_THROTTLE_MIX) {
        float excess = max_total - MAX_THROTTLE_MIX;
        float max_yaw_requested = fabsf(y); // E ok să folosim y absolut aici

        if (max_yaw_requested > 0.001f) {
            yaw_scale = (max_yaw_requested - excess) / max_yaw_requested;
        } else {
            yaw_scale = 0.0f;
        }

        if (yaw_scale < 0.0f) yaw_scale = 0.0f;
    }

    // Adăugăm Yaw-ul scalat la baza care conține deja Throttle + Roll + Pitch
    for (int i = 0; i < NUM_MOTORS; i++) {
        m[i] = m[i] + (motor_yaw[i] * yaw_scale);
    }

    // Step 4: Clamp to [throttle_idle..1.0] and write output
    for (int i = 0; i < NUM_MOTORS; i++) {
        m[i] = CLAMP(m[i], throttle_idle, MAX_THROTTLE_MIX);
        motor_dshot[i] = m[i];
    }
}


