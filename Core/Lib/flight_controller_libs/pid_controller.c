#include <float.h>
#include <math.h>
#include <pid_controller.h>

#define CLAMP(x, lo, hi) (((x) < (lo)) ? (lo) : ((x) > (hi)) ? (hi) : (x))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void pid_init(pid_controller_t *pid, float Kp, float Ki, float Kd, float min_output, float max_output) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->min_output = min_output;
    pid->max_output = max_output;
    pid->error = 0.0f;
    pid->pre_error = 0.0f;
    pid->integral = 0.0f;
    pid->integral_max_error = -1.0;
    pid->delta_time = 0.0f;
    pid->derivative_error = 0.0f;
    pid->stop_integral_accumulation_increase = 0;
    pid->stop_integral_accumulation_decrease = 0;
}

void pid_init_with_integral_limit(pid_controller_t *pid, float Kp, float Ki, float Kd, float min_output, float max_output, float integral_max_error) {
    pid_init(pid, Kp, Ki, Kd, min_output, max_output);
    pid_set_integral_limit(pid, integral_max_error);
}

float pid_calculate(pid_controller_t *pid, float setpoint, float process_value, float dt) {
    float error = setpoint - process_value;
    float Pout = pid->Kp * error;
    float Iout = 0.0f;
    float Dout = 0.0f;
    float derivative = 0.0f;
    dt = fabsf(dt);
    pid->delta_time = dt;
    pid->error = error;

    if (dt > FLT_EPSILON) {

    	if(pid->stop_integral_accumulation_increase != 0 && error > 0.0f) {}
    	else if(pid->stop_integral_accumulation_decrease != 0 && error < 0.0f) {}
    	else{
    		pid->integral += error * dt;
    	}

        if (pid->integral_max_error > FLT_EPSILON) {
        	pid->integral = CLAMP(pid->integral, -pid->integral_max_error, pid->integral_max_error);
        }

        Iout = pid->Ki * pid->integral;
        derivative = (error - pid->pre_error) / dt;
        Dout = pid->Kd * derivative;
    }

    float output = Pout + Iout + Dout;

    output = CLAMP(output, pid->min_output, pid->max_output);

    pid->derivative_error = derivative;
    pid->pre_error = error;
    return output;
}

void pid_set_parameters(pid_controller_t *pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

void pid_set_max_output(pid_controller_t *pid, float val) { pid->max_output = val; }
void pid_set_min_output(pid_controller_t *pid, float val) { pid->min_output = val; }
void pid_set_Kp(pid_controller_t *pid, float val) { pid->Kp = val; }
void pid_set_Ki(pid_controller_t *pid, float val) { pid->Ki = val; }
void pid_set_Kd(pid_controller_t *pid, float val) { pid->Kd = val; }

void pid_set_integral_limit(pid_controller_t *pid, float val) {
    val = fabsf(val);
    pid->integral_max_error = val;

    if (pid->integral_max_error > FLT_EPSILON) {
    	pid->integral = CLAMP(pid->integral, -pid->integral_max_error, pid->integral_max_error);
    }
}

float pid_get_integral_limit(pid_controller_t *pid) { return pid->integral_max_error; }
float pid_get_max_output(pid_controller_t *pid) { return pid->max_output; }
float pid_get_min_output(pid_controller_t *pid) { return pid->min_output; }
float pid_get_Kp(pid_controller_t *pid) { return pid->Kp; }
float pid_get_Ki(pid_controller_t *pid) { return pid->Ki; }
float pid_get_Kd(pid_controller_t *pid) { return pid->Kd; }
float pid_get_integral_error(pid_controller_t *pid) { return pid->integral; }

void pid_reset(pid_controller_t *pid) {
    pid->pre_error = 0.0f;
    pid->integral = 0.0f;
    pid->error = 0.0f;
    pid->derivative_error = 0.0f;
    pid->delta_time = 0.0f;
}


void pid_stop_integral_accumulation_increase_error(pid_controller_t *pid){
	pid->stop_integral_accumulation_increase = 1;
}

void pid_stop_integral_accumulation_decrease_error(pid_controller_t *pid){
	pid->stop_integral_accumulation_decrease = 1;
}

void pid_resume_integral_accumulation_increase_error(pid_controller_t *pid){
	pid->stop_integral_accumulation_increase = 0;
}

void pid_resume_integral_accumulation_decrease_error(pid_controller_t *pid){
	pid->stop_integral_accumulation_decrease = 0;
}

int pid_get_stop_integral_accumulation_decrease_error(pid_controller_t *pid){
	return pid->stop_integral_accumulation_decrease;
}

int pid_get_stop_integral_accumulation_increase_error(pid_controller_t *pid){
	return pid->stop_integral_accumulation_increase;
}
