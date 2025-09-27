#ifndef __PID_H__
#define __PID_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float max_output;
    float min_output;
    float Kp;
    float Ki;
    float Kd;
    float error;
    float pre_error;
    float derivative_error;
    float integral;
    float integral_max_error;
    float delta_time;
} pid_t;

void pid_init(volatile pid_t *pid, float Kp, float Ki, float Kd, float min_output, float max_output);
void pid_init_with_integral_limit(volatile pid_t *pid, float Kp, float Ki, float Kd, float min_output, float max_output, float integral_max_error);
float pid_calculate(volatile pid_t *pid, float setpoint, float process_value, float dt);
void pid_set_parameters(volatile pid_t *pid, float Kp, float Ki, float Kd);
void pid_set_max_output(volatile pid_t *pid, float val);
void pid_set_min_output(volatile pid_t *pid, float val);
void pid_set_Kp(volatile pid_t *pid, float val);
void pid_set_Ki(volatile pid_t *pid, float val);
void pid_set_Kd(volatile pid_t *pid, float val);
void pid_set_integral_limit(volatile pid_t *pid, float val);
float pid_get_integral_limit(volatile pid_t *pid);
float pid_get_max_output(volatile pid_t *pid);
float pid_get_min_output(volatile pid_t *pid);
float pid_get_Kp(volatile pid_t *pid);
float pid_get_Ki(volatile pid_t *pid);
float pid_get_Kd(volatile pid_t *pid);
float pid_get_integral_error(volatile pid_t *pid);
void pid_reset(volatile pid_t *pid);

#ifdef __cplusplus
}
#endif

#endif // __PID_H__
