#include "fp_cli.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include "lwshell/lwshell.h"
#include "rtos_flight_controller.h"
#include <assert.h>

lwshell_t lwshell_cli;

static const char raw_accel[] = "raw_accel";
static const char raw_gyro[] = "raw_gyro";
static const char estimated_accel[] = "estimated_accel";
static const char estimated_attitude[] = "estimated_attitude";
static const char motors_throttle[] = "motors_throttle";
static const char target_attitude[] = "target_attitude";
static const char target_throttle[] = "target_throttle";
static const char target_rate[] = "target_rate";
static const char pid_output[] = "pid_output";

struct telemetry_data_fags{
	int get_telemetry_active : 1;
	int raw_accel : 1;
	int raw_gyro : 1;
	int estimated_accel : 1;
	int estimated_gyro : 1;
	int estimated_attitude : 1;
	int motors_throttle : 1;
	int target_attitude : 1;
	int target_throttle : 1;
	int target_rate : 1;
	int pid_output : 1;
	char separator_char;
};

struct telemetry_data_fags get_telemetry_active = {};

#define OUT_PRINTF(...) usb_printf(__VA_ARGS__)

typedef int32_t (*parameter_state_switch_set_fn)(int32_t argc, char** argv);
typedef int32_t (*parameter_state_switch_set_info_fn)();
typedef int32_t (*parameter_state_switch_get_fn)();

#define PARAMETER_MAX_SIZE 30
#define ASSERT_PARAMETER(param_str) ( assert(strlen(param_str) <= PARAMETER_MAX_SIZE) )

struct parameter_state_switch{
	const char *name;
	uint8_t total_input_parameters;
	parameter_state_switch_set_fn set_fn;
	parameter_state_switch_set_info_fn set_info_fn;
	parameter_state_switch_get_fn get_fn;
};

#define TOTAL_PARAMETERS 10
struct parameter_state_switch parameters_arr[TOTAL_PARAMETERS];

static const char rate_controller_roll_pid[] = "rate_controller_roll_pid";
static const char rate_controller_pitch_pid[] = "rate_controller_pitch_pid";
static const char rate_controller_yaw_pid[] = "rate_controller_yaw_pid";
static const char level_p[] = "level_p";
static const char max_angle[] = "max_angle";

/*=================================================================================*/
int32_t rate_controller_roll_pid_set_fn(int32_t argc, char** argv){
	if(argc < 5){return -1;}
	int32_t argc_index = 0;
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
			#endif
				int error_detected = 0;
				float kp, ki, kd, max_integral_error, kff;
				if(sscanf(argv[argc_index], "%f", &kp) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &ki) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &kd) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &max_integral_error) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &kff) != 1) {error_detected = 1;}

				if(error_detected == 0){
					fcl.rate_controller.pid_roll.Kp = kp;
					fcl.rate_controller.pid_roll.Ki = ki;
					fcl.rate_controller.pid_roll.Kd = kd;
					fcl.rate_controller.pid_roll.integral_max_error = max_integral_error;
					fcl.rate_controller.pid_roll_k_ff = kff;
				}
				else{
					OUT_PRINTF("\r\n");
					OUT_PRINTF("ERROR: Parsing error\r\n");
				}
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreGive(fcl.rate_controller_mutex);
			#endif
	return 0;
}

int32_t rate_controller_roll_pid_set_info_fn(){
	OUT_PRINTF("\r\n");
	OUT_PRINTF("set %s (float)kp (float)ki (float)kd (float)max_integral_error (float)Kff\r\n", rate_controller_roll_pid);
}

int32_t rate_controller_roll_pid_get_fn(){
	OUT_PRINTF("\r\nParameter: %s\r\n", rate_controller_roll_pid);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
#endif
	OUT_PRINTF("Kp: %f\r\n", fcl.rate_controller.pid_roll.Kp);
	OUT_PRINTF("Ki: %f\r\n", fcl.rate_controller.pid_roll.Ki);
	OUT_PRINTF("Kd: %f\r\n", fcl.rate_controller.pid_roll.Kd);
	OUT_PRINTF("integral_max_error: %f\r\n", fcl.rate_controller.pid_roll.integral_max_error);
	OUT_PRINTF("Kff: %f\r\n", fcl.rate_controller.pid_roll_k_ff);
	OUT_PRINTF("Max Output: +/- %f\r\n", fcl.rate_controller.pid_roll.max_output);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.rate_controller_mutex);
#endif
}
/*=================================================================================*/

/*=================================================================================*/
int32_t rate_controller_pitch_pid_set_fn(int32_t argc, char** argv){
	if(argc < 5){return -1;}
	int32_t argc_index = 0;
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
			#endif
				int error_detected = 0;
				float kp, ki, kd, max_integral_error, kff;
				if(sscanf(argv[argc_index], "%f", &kp) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &ki) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &kd) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &max_integral_error) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &kff) != 1) {error_detected = 1;}

				if(error_detected == 0){
					fcl.rate_controller.pid_pitch.Kp = kp;
					fcl.rate_controller.pid_pitch.Ki = ki;
					fcl.rate_controller.pid_pitch.Kd = kd;
					fcl.rate_controller.pid_pitch.integral_max_error = max_integral_error;
					fcl.rate_controller.pid_pitch_k_ff = kff;
				}
				else{
					OUT_PRINTF("\r\n");
					OUT_PRINTF("ERROR: Parsing error\r\n");
				}
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreGive(fcl.rate_controller_mutex);
			#endif
	return 0;
}

int32_t rate_controller_pitch_pid_set_info_fn(){
	OUT_PRINTF("\r\n");
	OUT_PRINTF("set %s (float)kp (float)ki (float)kd (float)max_integral_error (float)Kff\r\n", rate_controller_pitch_pid);
}

int32_t rate_controller_pitch_pid_get_fn(){
	OUT_PRINTF("\r\nParameter: %s\r\n", rate_controller_pitch_pid);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
#endif
	OUT_PRINTF("Kp: %f\r\n", fcl.rate_controller.pid_pitch.Kp);
	OUT_PRINTF("Ki: %f\r\n", fcl.rate_controller.pid_pitch.Ki);
	OUT_PRINTF("Kd: %f\r\n", fcl.rate_controller.pid_pitch.Kd);
	OUT_PRINTF("integral_max_error: %f\r\n", fcl.rate_controller.pid_pitch.integral_max_error);
	OUT_PRINTF("Kff: %f\r\n", fcl.rate_controller.pid_pitch_k_ff);
	OUT_PRINTF("Max Output: +/- %f\r\n", fcl.rate_controller.pid_pitch.max_output);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.rate_controller_mutex);
#endif
}
/*=================================================================================*/

/*=================================================================================*/
int32_t rate_controller_yaw_pid_set_fn(int32_t argc, char** argv){
	if(argc < 5){return -1;}
	int32_t argc_index = 0;
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
			#endif
				int error_detected = 0;
				float kp, ki, kd, max_integral_error, kff;
				if(sscanf(argv[argc_index], "%f", &kp) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &ki) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &kd) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &max_integral_error) != 1) {error_detected = 1;}
				argc_index++;
				if(sscanf(argv[argc_index], "%f", &kff) != 1) {error_detected = 1;}

				if(error_detected == 0){
					fcl.rate_controller.pid_yaw.Kp = kp;
					fcl.rate_controller.pid_yaw.Ki = ki;
					fcl.rate_controller.pid_yaw.Kd = kd;
					fcl.rate_controller.pid_yaw.integral_max_error = max_integral_error;
					fcl.rate_controller.pid_yaw_k_ff = kff;
				}
				else{
					OUT_PRINTF("\r\n");
					OUT_PRINTF("ERROR: Parsing error\r\n");
				}
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreGive(fcl.rate_controller_mutex);
			#endif
	return 0;
}

int32_t rate_controller_yaw_pid_set_info_fn(){
	OUT_PRINTF("\r\n");
	OUT_PRINTF("set %s (float)kp (float)ki (float)kd (float)max_integral_error (float)Kff\r\n", rate_controller_yaw_pid);
}

int32_t rate_controller_yaw_pid_get_fn(){
	OUT_PRINTF("\r\nParameter: %s\r\n", rate_controller_yaw_pid);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
#endif
	OUT_PRINTF("Kp: %f\r\n", fcl.rate_controller.pid_yaw.Kp);
	OUT_PRINTF("Ki: %f\r\n", fcl.rate_controller.pid_yaw.Ki);
	OUT_PRINTF("Kd: %f\r\n", fcl.rate_controller.pid_yaw.Kd);
	OUT_PRINTF("integral_max_error: %f\r\n", fcl.rate_controller.pid_yaw.integral_max_error);
	OUT_PRINTF("Kff: %f\r\n", fcl.rate_controller.pid_yaw_k_ff);
	OUT_PRINTF("Max Output: +/- %f\r\n", fcl.rate_controller.pid_yaw.max_output);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.rate_controller_mutex);
#endif
}
/*=================================================================================*/

/*=================================================================================*/
int32_t attitude_controller_level_p_set_fn(int32_t argc, char** argv){
	if(argc < 1){return -1;}
	int32_t argc_index = 0;
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreTake(fcl.attitude_controller_mutex, portMAX_DELAY);
			#endif
				int error_detected = 0;
				float temp_level_p;
				if(sscanf(argv[argc_index], "%f", &temp_level_p) != 1) {error_detected = 1;}

				if(error_detected == 0){
					fcl.attitude_controller.rate_gain = temp_level_p;
				}
				else{
					OUT_PRINTF("\r\n");
					OUT_PRINTF("ERROR: Parsing error\r\n");
				}
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreGive(fcl.attitude_controller_mutex);
			#endif
	return 0;
}

int32_t attitude_controller_level_p_set_info_fn(){
	OUT_PRINTF("\r\n");
	OUT_PRINTF("set %s (float)level_p\r\n", level_p);
}

int32_t attitude_controller_level_p_get_fn(){
	OUT_PRINTF("\r\nParameter: %s\r\n", level_p);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.attitude_controller_mutex, portMAX_DELAY);
#endif
	OUT_PRINTF("level_p: %f\r\n", fcl.attitude_controller.rate_gain);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.attitude_controller_mutex);
#endif
}
/*=================================================================================*/

/*=================================================================================*/
int32_t attitude_controller_max_angle_set_fn(int32_t argc, char** argv){
	if(argc < 1){return -1;}
	int32_t argc_index = 0;
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreTake(fcl.attitude_controller_mutex, portMAX_DELAY);
			#endif
				int error_detected = 0;
				float max_angle_temp;
				if(sscanf(argv[argc_index], "%f", &max_angle_temp) != 1) {error_detected = 1;}

				if(error_detected == 0){
					fcl.attitude_controller.max_angle = max_angle_temp;
				}
				else{
					OUT_PRINTF("\r\n");
					OUT_PRINTF("ERROR: Parsing error\r\n");
				}
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreGive(fcl.attitude_controller_mutex);
			#endif
	return 0;
}

int32_t attitude_controller_max_angle_set_info_fn(){
	OUT_PRINTF("\r\n");
	OUT_PRINTF("set %s (float radians)max_angle\r\n", max_angle);
}

int32_t attitude_controller_max_angle_get_fn(){
	OUT_PRINTF("\r\nParameter: %s\r\n", level_p);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.attitude_controller_mutex, portMAX_DELAY);
#endif
	OUT_PRINTF("max_angle [radians]: %f\r\n", fcl.attitude_controller.max_angle);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.attitude_controller_mutex);
#endif
}
/*=================================================================================*/

void init_parameters_arr(){
	memset(parameters_arr, 0, sizeof(parameters_arr));
	int param_index = 0;
	parameters_arr[param_index].name = rate_controller_roll_pid;
	parameters_arr[param_index].total_input_parameters = 5;
	parameters_arr[param_index].set_fn = rate_controller_roll_pid_set_fn;
	parameters_arr[param_index].set_info_fn = rate_controller_roll_pid_set_info_fn;
	parameters_arr[param_index].get_fn = rate_controller_roll_pid_get_fn;

	param_index++;
	parameters_arr[param_index].name = rate_controller_pitch_pid;
	parameters_arr[param_index].total_input_parameters = 5;
	parameters_arr[param_index].set_fn = rate_controller_pitch_pid_set_fn;
	parameters_arr[param_index].set_info_fn = rate_controller_pitch_pid_set_info_fn;
	parameters_arr[param_index].get_fn = rate_controller_pitch_pid_get_fn;

	param_index++;
	parameters_arr[param_index].name = rate_controller_yaw_pid;
	parameters_arr[param_index].total_input_parameters = 5;
	parameters_arr[param_index].set_fn = rate_controller_yaw_pid_set_fn;
	parameters_arr[param_index].set_info_fn = rate_controller_yaw_pid_set_info_fn;
	parameters_arr[param_index].get_fn = rate_controller_yaw_pid_get_fn;

	param_index++;
	parameters_arr[param_index].name = level_p;
	parameters_arr[param_index].total_input_parameters = 1;
	parameters_arr[param_index].set_fn = attitude_controller_level_p_set_info_fn;
	parameters_arr[param_index].set_info_fn = attitude_controller_level_p_set_info_fn;
	parameters_arr[param_index].get_fn = attitude_controller_level_p_get_fn;

	param_index++;
	parameters_arr[param_index].name = max_angle;
	parameters_arr[param_index].total_input_parameters = 1;
	parameters_arr[param_index].set_fn = attitude_controller_max_angle_set_info_fn;
	parameters_arr[param_index].set_info_fn = attitude_controller_max_angle_set_info_fn;
	parameters_arr[param_index].get_fn = attitude_controller_max_angle_get_fn;

	param_index++;
}

void command_set_list(){
	for(int i=0; i < TOTAL_PARAMETERS; i++){
		if(parameters_arr[i].set_info_fn != NULL && parameters_arr[i].set_fn != NULL){
			parameters_arr[i].set_info_fn();
		}
	}
}

void command_get_list(){
	for(int i=0; i < TOTAL_PARAMETERS; i++){
		if(parameters_arr[i].get_fn != NULL && parameters_arr[i].name != NULL){
			OUT_PRINTF("\r\n");
			OUT_PRINTF("%s\r\n", parameters_arr[i].name);
		}
	}
}




/* Command to get called */
int32_t set_cmd_fn(int32_t argc, char** argv) {
	if(argc < 2) {
		OUT_PRINTF("\r\n");
		OUT_PRINTF("Type \"set list\" to list all parameters that can be set.\r\n");
		return 0;
	}

	int argc_index = 1;
	int parameter_found = 0;

	if(strcmp(argv[argc_index], "list") == 0){
		command_set_list();
		parameter_found = 1;
	}
	else{
		for(int i=0; i < TOTAL_PARAMETERS; i++){
			if(parameters_arr[i].name == NULL) continue;

			if(strcmp(argv[argc_index], parameters_arr[i].name) == 0){
				parameter_found = 1;
				if(argc < (1+1+parameters_arr[i].total_input_parameters)) {
					OUT_PRINTF("\r\n");
					OUT_PRINTF("ERROR: Missing parameter\r\n");
					parameters_arr[i].set_info_fn();
				}
				else{
					if(parameters_arr[i].set_fn == NULL){
						OUT_PRINTF("\r\n");
						OUT_PRINTF("ERROR: Parameter Cannot be set\r\n");
					}
					parameters_arr[i].set_fn(argc - argc_index - 1, &(argv[argc_index + 1]));
					break;
				}
			}
		}
	}

	if(parameter_found == 0){
		OUT_PRINTF("\r\n");
		OUT_PRINTF("ERROR: Not found\r\n");
	}

    /* Successful execution */
    return 0;
}

int32_t get_cmd_fn(int32_t argc, char** argv) {
	if(argc < 2) {
		OUT_PRINTF("\r\n");
		OUT_PRINTF("Type \"get list\" to list all parameters that can be set.\r\n");
		return 0;
	}

	int argc_index = 1;
	int parameter_found = 0;

	if(strcmp(argv[argc_index], "list") == 0){
		command_get_list();
		parameter_found = 1;
	}
	else{
		for(int i=0; i < TOTAL_PARAMETERS; i++){
			if(parameters_arr[i].name == NULL) continue;

			if(strcmp(argv[argc_index], parameters_arr[i].name) == 0){
				parameter_found = 1;
				if(parameters_arr[i].get_fn == NULL){
					OUT_PRINTF("\r\n");
					OUT_PRINTF("ERROR: Parameter Cannot be get\r\n");
				}
				else{
					parameters_arr[i].get_fn();
					break;
				}
			}
		}
	}

	if(parameter_found == 0){
		OUT_PRINTF("\r\n");
		OUT_PRINTF("ERROR: Not found\r\n");
	}

    /* Successful execution */
    return 0;
}



// get_telemetry frequency_hz separator_char data1 data2 data3 data...
int32_t get_telemetry_cmd_fn(int32_t argc, char** argv){
	memset(&get_telemetry_active, 0, sizeof(get_telemetry_active));
	get_telemetry_active.get_telemetry_active = 0;

}

void output_fn(const char* str, struct lwshell* lwobj){
	OUT_PRINTF("%s", str);
}

/* Example code */
void fp_cli_example_minimal_init(void) {
    /* Init library */
	init_parameters_arr();
    lwshell_init_ex(&lwshell_cli);

    lwshellr_t lwshell_set_output_fn_ex(lwshell_t* lwobj, lwshell_output_fn out_fn);
    lwshell_set_output_fn_ex(&lwshell_cli, output_fn);

    /* Define shell commands */
    lwshell_register_cmd_ex(&lwshell_cli, "set", set_cmd_fn, "set a parameter");
    lwshell_register_cmd_ex(&lwshell_cli, "get", get_cmd_fn, "get a parameter");

}
