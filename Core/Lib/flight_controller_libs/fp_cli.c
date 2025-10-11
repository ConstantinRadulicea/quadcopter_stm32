#include "fp_cli.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include "lwshell/lwshell.h"
#include "rtos_flight_controller.h"


lwshell_t lwshell_cli;

/* Command to get called */
//int32_t set_cmd_fn(int32_t argc, char** argv) {
//	usb_printf("\r\nNumber of argv: %d\r\n", (int)argc);
//    for (int32_t i = 0; i < argc; ++i) {
//    	usb_printf("ARG[%d]: %s\r\n", (int)i, argv[i]);
//    }
//
//    /* Successful execution */
//    return 0;
//}


/* Command to get called */
int32_t set_cmd_fn(int32_t argc, char** argv) {
	if(argc < 2) {
		usb_printf("\r\n");
		usb_printf("Type \"set list\" to list all parameters that can be set.\r\n");
		return 0;
	}

	int argc_index = 1;

	if(strcmp(argv[argc_index], "list") == 0){
		usb_printf("\r\n");
		usb_printf("rate_controller_roll_pid (float)kp (float)ki (float)kd (float)max_integral_error\r\n");
	}

	// Rate controller PID
	else if(strcmp(argv[argc_index], "rate_controller_roll_pid") == 0){
		if(argc >= (1+1+4)){
		#if MUTEX_ESP_ENABLE != 0
			xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
		#endif
			int error_detected = 0;
			float kp, ki, kd, max_integral_error;
			argc_index++;
			if(sscanf(argv[argc_index], "%f", &kp) != 1) {error_detected = 1;}
			argc_index++;
			if(sscanf(argv[argc_index], "%f", &ki) != 1) {error_detected = 1;}
			argc_index++;
			if(sscanf(argv[argc_index], "%f", &kd) != 1) {error_detected = 1;}
			argc_index++;
			if(sscanf(argv[argc_index], "%f", &max_integral_error) != 1) {error_detected = 1;}

			if(error_detected == 0){
				fcl.rate_controller.pid_roll.Kp = kp;
				fcl.rate_controller.pid_roll.Ki = ki;
				fcl.rate_controller.pid_roll.Kd = kd;
				fcl.rate_controller.pid_roll.integral_max_error = max_integral_error;
			}
			else{
				usb_printf("\r\n");
				usb_printf("ERROR: Parsing error\r\n");
			}
		#if MUTEX_ESP_ENABLE != 0
			xSemaphoreGive(fcl.rate_controller_mutex);
		#endif
		}
		else{
			usb_printf("\r\n");
			usb_printf("ERROR: Missing parameter\r\n");
		}
	}

    /* Successful execution */
    return 0;
}

int32_t get_cmd_fn(int32_t argc, char** argv) {
	if(argc < 2) {
		usb_printf("\r\n");
		usb_printf("Type \"get list\" to list all parameters that can be set.\r\n");
		return 0;
	}

	int argc_index = 1;

	if(strcmp(argv[argc_index], "list") == 0){
		usb_printf("\r\n");
		usb_printf("rate_controller_roll_pid\r\n");
	}

	// Rate controller PID
	else if(strcmp(argv[argc_index], "rate_controller_roll_pid") == 0){
	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
	#endif
	usb_printf("\r\nParameter: rate_controller_roll_pid\r\n");
	usb_printf("Kp: %f\r\n", fcl.rate_controller.pid_roll.Kp);
	usb_printf("Ki: %f\r\n", fcl.rate_controller.pid_roll.Ki);
	usb_printf("Kd: %f\r\n", fcl.rate_controller.pid_roll.Kd);
	usb_printf("integral_max_error: %f\r\n", fcl.rate_controller.pid_roll.integral_max_error);
	usb_printf("Max Output: +/- %f\r\n", fcl.rate_controller.pid_roll.max_output);

	#if MUTEX_ESP_ENABLE != 0
		xSemaphoreGive(fcl.rate_controller_mutex);
	#endif
	}

    /* Successful execution */
    return 0;
}

void output_fn(const char* str, struct lwshell* lwobj){
	usb_printf("%s", str);
}

/* Example code */
void fp_cli_example_minimal_init(void) {
    /* Init library */
    lwshell_init_ex(&lwshell_cli);

    lwshellr_t lwshell_set_output_fn_ex(lwshell_t* lwobj, lwshell_output_fn out_fn);
    lwshell_set_output_fn_ex(&lwshell_cli, output_fn);

    /* Define shell commands */
    lwshell_register_cmd_ex(&lwshell_cli, "set", set_cmd_fn, "set a parameter");
    lwshell_register_cmd_ex(&lwshell_cli, "get", get_cmd_fn, "get a parameter");

}
