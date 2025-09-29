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
    volatile float error;
    volatile float pre_error;
    volatile float derivative_error;
    volatile float integral;
    float integral_max_error;
    volatile float delta_time;
} pid_t;

void pid_init(pid_t *pid, float Kp, float Ki, float Kd, float min_output, float max_output);
void pid_init_with_integral_limit(pid_t *pid, float Kp, float Ki, float Kd, float min_output, float max_output, float integral_max_error);
float pid_calculate(pid_t *pid, float setpoint, float process_value, float dt);
void pid_set_parameters(pid_t *pid, float Kp, float Ki, float Kd);
void pid_set_max_output(pid_t *pid, float val);
void pid_set_min_output(pid_t *pid, float val);
void pid_set_Kp(pid_t *pid, float val);
void pid_set_Ki(pid_t *pid, float val);
void pid_set_Kd(pid_t *pid, float val);
void pid_set_integral_limit(pid_t *pid, float val);
float pid_get_integral_limit(pid_t *pid);
float pid_get_max_output(pid_t *pid);
float pid_get_min_output(pid_t *pid);
float pid_get_Kp(pid_t *pid);
float pid_get_Ki(pid_t *pid);
float pid_get_Kd(pid_t *pid);
float pid_get_integral_error(pid_t *pid);
void pid_reset(pid_t *pid);

#ifdef __cplusplus
}
#endif

#endif // __PID_H__
