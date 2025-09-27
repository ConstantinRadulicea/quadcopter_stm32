#ifndef __MOTOR_MIXER_H__
#define __MOTOR_MIXER_H__


#ifdef __cplusplus
extern "C" {
#endif

#define NUM_MOTORS 4

/**
 * Motor mixer for Quad-X
 * Front Right (0)
 * Front Left  (1)
 * Rear Right  (2)
 * Rear Left   (3)
 * 
 *         FRONT
 *           ↑
 *      0         1
 *     (FL)     (FR)
 *
 *      2         3
 *     (RL)     (RR)
 *           ↓
 *          REAR
*/
void motor_mixer_quad_x(
    float throttle_norm,       // 0..1 throttle
    float roll_out, float pitch_out, float yaw_out,  // ±PID_LIMIT
    float throttle_idle,  // e.g., 0..1 idle throttle
    float motor_dshot[NUM_MOTORS]     // output throttle per motor
);

#ifdef __cplusplus
}
#endif

#endif // __MOTOR_MIXER_H__
