#include "rtos_flight_controller.h"

#include "driver_mpu6500_basic.h"
#include <stdio.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "imu.h"
#include "cmsis_os2.h"
#include "esc_pwm.h"
#include "tim.h"
#include "atomic_custom.h"
#include "usart.h"
#include "crc.h"
#include "fp_cli.h"

#define ENABLE_ESC_CALIBRATION_BUILD 0

flight_control_loop_t fcl;

SemaphoreHandle_t g_motors_throttle_mutex;
float g_motors_throttle[4];
pwm_t esc_motors[4];

static TaskHandle_t ctrl_task_h;
static TaskHandle_t write_motor_main_h;

static uint8_t flight_stack[2048];
static uint8_t write_stack[2048];
static uint8_t rc_stack[2048];
static uint8_t telem_stack[2048];
static uint8_t fp_cli_stack[2048];

osThreadAttr_t flight_attr, write_attr, rc_attr, telem_attr, fp_cli_attr;
StaticTask_t flight_h_taskControlBlock;
StaticTask_t write_h_taskControlBlock;
StaticTask_t rc_h_taskControlBlock;
StaticTask_t telem_h_taskControlBlock;
StaticTask_t fp_cli_h_taskControlBlock;
// Thread IDs
static osThreadId_t flight_h;
static osThreadId_t write_h;
static osThreadId_t rc_h;
static osThreadId_t telem_h;
static osThreadId_t fp_cli_h;


#define STACK_WORDS(bytes) ((bytes)/sizeof(StackType_t))

static void ctrl_timer_cb(TimerHandle_t arg)
{
     BaseType_t hpw = pdFALSE;
     vTaskNotifyGiveFromISR(ctrl_task_h, &hpw);
     if (hpw) portYIELD_FROM_ISR(hpw);
    //xTaskNotifyGive(ctrl_task_h);   // task-context safe
}

static void write_motor_timer_cb(TimerHandle_t arg)
{
     BaseType_t hpw = pdFALSE;
     vTaskNotifyGiveFromISR(write_motor_main_h, &hpw);
     if (hpw) portYIELD_FROM_ISR(hpw);
    //xTaskNotifyGive(write_motor_main_h);   // task-context safe
}

static void init_imu(){
    int16_t temp_int16_t;
    uint8_t mpu_res;
	mpu_res = mpu6500_basic_init(MPU6500_INTERFACE_SPI, MPU6500_ADDRESS_0x68);
	if(mpu_res == 0){
		  mpu6500_gyro_offset_convert_to_register(&gs_handle_mpu6500, 0.0f, &temp_int16_t);
		  mpu6500_set_gyro_x_offset(&gs_handle_mpu6500, temp_int16_t);

		  mpu6500_gyro_offset_convert_to_register(&gs_handle_mpu6500, 0.0f, &temp_int16_t);
		  mpu6500_set_gyro_y_offset(&gs_handle_mpu6500, temp_int16_t);

		  mpu6500_gyro_offset_convert_to_register(&gs_handle_mpu6500, 0.0f, &temp_int16_t);
		  mpu6500_set_gyro_z_offset(&gs_handle_mpu6500, temp_int16_t);


		  mpu6500_accelerometer_offset_convert_to_register(&gs_handle_mpu6500, 0.0f, &temp_int16_t);
		  mpu6500_set_accelerometer_x_offset(&gs_handle_mpu6500, temp_int16_t);

		  mpu6500_accelerometer_offset_convert_to_register(&gs_handle_mpu6500, 0.0f, &temp_int16_t);
		  mpu6500_set_accelerometer_y_offset(&gs_handle_mpu6500, temp_int16_t);

		  mpu6500_accelerometer_offset_convert_to_register(&gs_handle_mpu6500, 0.0f, &temp_int16_t);
		  mpu6500_set_accelerometer_z_offset(&gs_handle_mpu6500, temp_int16_t);
	  }
	  else{
		  printf("MPU6500 failed to initialize");
	  }
}


static void flight_controller_main(void *arg)
{
    uint16_t len;
    int16_t accel_raw[3];
    int16_t gyro_raw[3];
    float accel[3];
    float gyro[3];
    coord3D gyro_data;
    coord3D accel_data;

    ctrl_task_h = xTaskGetCurrentTaskHandle();

        static TimerHandle_t tmr = NULL;
        if (tmr == NULL) {
        	tmr = xTimerCreate(
                "main_control_loop",                 // name (like esp_timer name)
                pdMS_TO_TICKS(HzToMilliSec(MAIN_LOOP_HZ)),        // period
                pdTRUE,                          // auto-reload (periodic)
                NULL,                             // timer ID used to carry arg
				ctrl_timer_cb                // callback
            );
        }

        if (tmr != NULL) {
            xTimerStart(tmr, 0);
        }

    /* set 1 */
      for (;;)
      {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        len = 1;
        if (mpu6500_read(&gs_handle_mpu6500,
                    (int16_t (*)[3])&accel_raw, (float (*)[3])&accel,
                    (int16_t (*)[3])&gyro_raw, (float (*)[3])&gyro,
                     &len) != 0
                    )
        {
            printf("MPU6500 read error\n");
            flight_control_loop_disarm_esc(&fcl);
            //return; // implement some form of recovery if the mpu read fails
        }
        else{
            accel_data.x = G2MS2(accel[0]);
            accel_data.y = -G2MS2(accel[1]);
            accel_data.z = -G2MS2(accel[2]);
            gyro_data.x = radians(gyro[0]);
            gyro_data.y = -radians(gyro[1]);
            gyro_data.z = -radians(gyro[2]);
            flight_control_loop_update_imu(&fcl, gyro_data, accel_data);
        }
        flight_control_loop_tick(&fcl);
      }
}


static void write_motor_main(void *arg){
    write_motor_main_h = xTaskGetCurrentTaskHandle();

    float duty_cycle;
    static TimerHandle_t s_writeMotorTimer = NULL;
    if (s_writeMotorTimer == NULL) {
        s_writeMotorTimer = xTimerCreate(
            "write_mtr_cnt",                 // name (like esp_timer name)
            pdMS_TO_TICKS(HzToMilliSec(WRITE_MOTOR_TASK_HZ)),        // period
            pdTRUE,                          // auto-reload (periodic)
            NULL,                             // timer ID used to carry arg
			write_motor_timer_cb                // callback
        );
    }

    if (s_writeMotorTimer != NULL) {
        xTimerStart(s_writeMotorTimer, 0);
    }


    g_motors_throttle_mutex = xSemaphoreCreateMutex();
    //servos_init();
    memset(g_motors_throttle, 0, sizeof(g_motors_throttle));
    xSemaphoreGive(g_motors_throttle_mutex);

    for (;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        xSemaphoreTake(g_motors_throttle_mutex, portMAX_DELAY);
        flight_control_loop_get_motors_throttle(&fcl, g_motors_throttle);

		#if ENABLE_ESC_CALIBRATION_BUILD != 0
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreTake(fcl.rc_attitude_control_mutex, portMAX_DELAY);
			#endif
				coord3D target_attitude;
				float target_throttle = 0.0f;
				rc_attitude_control_get_processed(
					&fcl.rc_attitude_control,
					&(target_attitude.x),
					&(target_attitude.y),
					&(target_attitude.z),
					&target_throttle
				);
			#if MUTEX_ESP_ENABLE != 0
				xSemaphoreGive(fcl.rc_attitude_control_mutex);
			#endif
				for(int i=0; i<4; i++){
					g_motors_throttle[i] = target_throttle;
				}
		#endif

        for (size_t i = 0; i < 4; i++) {
        	duty_cycle = esc_pwm_throttle_to_duty_cycle(ESC_PWM_HZ, ESC_PWM_MIN_US, ESC_PWM_MAX_US, g_motors_throttle[i]);
        	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
        	{
        		pwm_set_duty(&esc_motors[i], duty_cycle);
        	}
        }
        xSemaphoreGive(g_motors_throttle_mutex);
    }
}

static int line_buffer_add_char(char c, char *buf, size_t buf_size, size_t *len)
{
    // buf: destination buffer
    // buf_size: total buffer capacity
    // len: pointer to current length (persistent between calls)
    // Returns 1 when a full line (ending with \n\r) is complete, 0 otherwise

    if (*len + 1 >= buf_size) {
        // buffer overflow: reset
        *len = 0;
        return 0;
    }

    buf[(*len)++] = c;

    // Check for \n\r at the end
    if (*len >= 2) {
    	if(buf[*len - 2] == '\r' && buf[*len - 1] == '\n'){
            buf[*len - 2] = '\0'; // terminate string before \n\r
            *len = 0;             // reset for next line
            return 1;             // line complete
    	}
    }

    return 0; // line not complete yet
}

static void rc_control_main(void *arg)
{
    uint8_t uart_recv_buffer[64];
    int recv_data;
    char linebuf[256];
    size_t line_len = 0;

    float target_throttle = 0;
    float temp_target_throttle = 0;

    coord3D target_attitude = {0};
    coord3D temp_target_attitude = {0};

    float arm_flag = 0;
    float temp_arm_flag = 0;

    float disarm_flag = 0;
    float temp_disarm_flag = 0;

    int errors = 0;
    int current_message_corrupted = 0;
    uint16_t crc_calculated;
    unsigned int crc_received = 0;
    float pid_p = 0.0f;
    float pid_i = 0.0f;
    float pid_d = 0.0f;
    float level_p = 0.0f;
    float pid_apply = 0.0f;


  /* Infinite loop */
  for(;;)
  {
	  current_message_corrupted = 1;
      while (usart1_data_available_for_read() > 0)
      {
          recv_data = usart1_recv_data((char*)uart_recv_buffer, sizeof(uart_recv_buffer));
          for(int i=0; i<recv_data; i++){


			  if (line_buffer_add_char(uart_recv_buffer[i], linebuf, sizeof(linebuf), &line_len)) {

				  int temp_str_len = strlen(linebuf);
				  temp_str_len -= 6;
				  crc_calculated = crc16_ccitt_init();
				  crc_calculated = crc16_ccitt_add_arr(crc_calculated, (uint8_t*)linebuf, temp_str_len);

				  if (sscanf(linebuf, "%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%x",
						  &temp_target_throttle,
						  &temp_target_attitude.x,
						  &temp_target_attitude.y,
						  &temp_target_attitude.z,
						  &temp_arm_flag,
						  &temp_disarm_flag,

						  &pid_p,
						  &pid_i,
						  &pid_d,
						  &level_p,
						  &pid_apply,
						  &crc_received

						  ) == 12) {
					  current_message_corrupted = 0;
					  //printf("%s\n", linebuf);

				  } else {
					  current_message_corrupted = 1;
//					  printf("%s\n", linebuf);
				  }
				  if(crc_received != (unsigned int)crc_calculated){
					  current_message_corrupted = 1;
					  continue;
				  }
			  }
          }
      }


      if(current_message_corrupted == 0){
    	  errors = 0;

          target_attitude = temp_target_attitude;
          arm_flag = (temp_arm_flag);
          disarm_flag = (temp_disarm_flag);
          target_throttle = temp_target_throttle;
          if(pid_apply > 0.5f){
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
#endif
	fcl.rate_controller.pid_pitch.Kp = pid_p;
	fcl.rate_controller.pid_pitch.Ki = pid_i;
	fcl.rate_controller.pid_pitch.Kd = pid_d;

	fcl.rate_controller.pid_roll.Kp = pid_p;
	fcl.rate_controller.pid_roll.Ki = pid_i;
	fcl.rate_controller.pid_roll.Kd = pid_d;

	fcl.rate_controller.pid_yaw.Kp = pid_p;
	fcl.rate_controller.pid_yaw.Ki = pid_i;
	fcl.rate_controller.pid_yaw.Kd = pid_d;

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.rate_controller_mutex);
#endif

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.attitude_controller_mutex, portMAX_DELAY);
#endif
	fcl.attitude_controller.rate_gain = level_p;
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.attitude_controller_mutex);
#endif
          }
      }

      errors += current_message_corrupted;

      if (errors >= (RC_CONTROLLER_HZ / 2.0f)){
    	  target_attitude.x = 0.0f;
    	  target_attitude.y = 0.0f;
    	  target_attitude.z = 0.0f;
    	  target_throttle = 0.0f;
    	  disarm_flag = 1.0f;
    	  arm_flag = 0.0f;
      }


      if (arm_flag > 0.5f){
    	  flight_control_loop_arm_esc(&fcl);
      }

      if (disarm_flag > 0.5f){
    	  flight_control_loop_disarm_esc(&fcl);
      }

      flight_control_loop_update_rc_control(&fcl, target_attitude, target_throttle);

      vTaskDelay(pdMS_TO_TICKS(HzToMilliSec(RC_CONTROLLER_HZ)));
  }
}


static void print_telemetry_data(void *arg){
    // Get estimated attitude and body frame accel/gyro
	coord3D body_frame_accel, body_frame_gyro;
    coord3D raw_accel, raw_gyro;
	quaternion body_frame_estimated_q;
    float local_motors_throttle[4];
	coord3D target_attitude = { 0 };
	float target_throttle = 0.0f;

    for (;;)
    {
    	vTaskDelay(pdMS_TO_TICKS(HzToMilliSec(TELEMETRY_TASK_HZ)));
    	if(start_telemetry == 0){
    		continue;
    	}
    #if MUTEX_ESP_ENABLE != 0
        xSemaphoreTake(fcl.imu_mutex, portMAX_DELAY);
    #endif
        imu_get_raw_accel_data(&(fcl.imu), &raw_accel);
        imu_get_raw_gyro_data(&(fcl.imu), &raw_gyro);
        imu_get_estimated_data(&(fcl.imu), &body_frame_estimated_q, &body_frame_accel, &body_frame_gyro);
    #if MUTEX_ESP_ENABLE != 0
        xSemaphoreGive(fcl.imu_mutex);
    #endif

        flight_control_loop_get_motors_throttle(&fcl, local_motors_throttle);
        angles3D angles = quat2angle(&(body_frame_estimated_q));

        printf("%.3f;%.3f;%.3f;", degrees(angles.x), degrees(angles.y), degrees(angles.z));
        printf("%.3f;%.3f;%.3f;", body_frame_accel.x, body_frame_accel.y, body_frame_accel.z);
        printf("%.3f;%.3f;%.3f;", body_frame_gyro.x, body_frame_gyro.y, body_frame_gyro.z);
//        printf("%.3f;%.3f;%.3f;", raw_accel.x, raw_accel.y, raw_accel.z);
//        printf("%.3f;%.3f;%.3f;", raw_gyro.x, raw_gyro.y, raw_gyro.z);
        printf("%.3f;%.3f;%.3f;%.3f;", local_motors_throttle[0], local_motors_throttle[1], local_motors_throttle[2], local_motors_throttle[3]);

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.rc_attitude_control_mutex, portMAX_DELAY);
#endif
	rc_attitude_control_get_processed(
		&fcl.rc_attitude_control,
		&(target_attitude.x),
		&(target_attitude.y),
		&(target_attitude.z),
		&target_throttle
	);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.rc_attitude_control_mutex);
#endif
	//	printf("%.3f;%.3f;%.3f;", target_attitude.x, target_attitude.y, target_attitude.z);
	printf("%.3f;", target_throttle);

#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.attitude_controller_mutex, portMAX_DELAY);
#endif
	float target_roll_rate = 0.0f;
	float target_pitch_rate = 0.0f;
	float target_yaw_rate = 0.0f;
	attitude_controller_get_calculated_rate(
		&fcl.attitude_controller,
		&target_roll_rate,
		&target_pitch_rate,
		&target_yaw_rate
	);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.attitude_controller_mutex);
#endif
//	printf("%.3f;%.3f;%.3f;", target_roll_rate, target_pitch_rate, target_yaw_rate);


#if MUTEX_ESP_ENABLE != 0
	xSemaphoreTake(fcl.rate_controller_mutex, portMAX_DELAY);
#endif
	float pid_roll_output = 0.0f;
	float pid_pitch_output = 0.0f;
	float pid_yaw_output = 0.0f;
	// Get PID outputs
	rate_controller_get_pid_outputs(
		&fcl.rate_controller,
		&pid_roll_output,
		&pid_pitch_output,
		&pid_yaw_output
	);
#if MUTEX_ESP_ENABLE != 0
	xSemaphoreGive(fcl.rate_controller_mutex);
#endif
	printf("%.3f;%.3f;%.3f;", degrees(pid_roll_output), degrees(pid_pitch_output), degrees(pid_yaw_output));

//    printf("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)flight_h) * sizeof(StackType_t)));
//    printf("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)write_h) * sizeof(StackType_t)));
//    printf("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)rc_h)     * sizeof(StackType_t)));
//    printf("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)telem_h)  * sizeof(StackType_t)));


    printf("\n");

    }
}

#include "fp_cli.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
char fp_cli_read_buffer[128];
static void fp_cli_func(void *arg){
	size_t read_bytes = 0;
	//CDC_Transmit_FS((uint8_t*)err_header, sizeof(err_header) - 1);
	fp_cli_example_minimal_init();
	//lwshellr_t lwshell_input_ex(lwshell_t* lwobj, const void* in_data, size_t len);
	for(;;){
		read_bytes = CDC_recv_data(fp_cli_read_buffer, sizeof(fp_cli_read_buffer)-1);
		lwshell_input_ex(&lwshell_cli, fp_cli_read_buffer, read_bytes);
		vTaskDelay(pdMS_TO_TICKS(100));
	}

}


void app_init(){
    init_imu();
    flight_control_loop_init(&fcl);
    flight_control_loop_disarm_esc(&fcl);

    float duty_cycle = esc_pwm_throttle_to_duty_cycle(ESC_PWM_HZ, ESC_PWM_MIN_US, ESC_PWM_MAX_US, 0.0f);
	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
	{
		pwm_init(&esc_motors[0], MOTOR_1_TIM, MOTOR_1_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
		pwm_init(&esc_motors[1], MOTOR_2_TIM, MOTOR_2_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
		pwm_init(&esc_motors[2], MOTOR_3_TIM, MOTOR_3_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
		pwm_init(&esc_motors[3], MOTOR_4_TIM, MOTOR_4_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
	}
}


void app_main_start(void *argument)
{
	app_init();

    // Priority mapping:
    //   original 15  -> high
    //   original 14  -> above normal
    //   original 13  -> normal
    //   original 5   -> below normal
    //
    // If you need finer spacing, use osPriorityHigh1..7, osPriorityAboveNormal1..7 (if available).

    flight_attr = (osThreadAttr_t){
        .name       = "flight_controller_main",
        .priority   = osPriorityRealtime,
        .stack_mem  = flight_stack,
        .stack_size = sizeof(flight_stack),
		.cb_mem = &flight_h_taskControlBlock,
		.cb_size = sizeof(flight_h_taskControlBlock)
    };
    flight_h = osThreadNew(flight_controller_main, NULL, &flight_attr);
    configASSERT(flight_h != NULL);

    write_attr = (osThreadAttr_t){
        .name       = "write_motor_main",
        .priority   = osPriorityAboveNormal,
        .stack_mem  = write_stack,
        .stack_size = sizeof(write_stack),
		.cb_mem = &write_h_taskControlBlock,
		.cb_size = sizeof(write_h_taskControlBlock)
    };
    write_h = osThreadNew(write_motor_main, NULL, &write_attr);
    configASSERT(write_h != NULL);

    rc_attr = (osThreadAttr_t){
        .name       = "rc_control_main",
        .priority   = osPriorityNormal,
        .stack_mem  = rc_stack,
        .stack_size = sizeof(rc_stack),
		.cb_mem = &rc_h_taskControlBlock,
		.cb_size = sizeof(rc_h_taskControlBlock)
    };
    rc_h = osThreadNew(rc_control_main, NULL, &rc_attr);
    configASSERT(rc_h != NULL);

    telem_attr = (osThreadAttr_t){
        .name       = "print_telemetry_data",
        .priority   = osPriorityBelowNormal,
        .stack_mem  = telem_stack,
        .stack_size = sizeof(telem_stack),
		.cb_mem = &telem_h_taskControlBlock,
		.cb_size = sizeof(telem_h_taskControlBlock)
    };
    telem_h = osThreadNew(print_telemetry_data, NULL, &telem_attr);
    configASSERT(telem_h != NULL);


    fp_cli_attr = (osThreadAttr_t){
        .name       = "fp_cli_func",
        .priority   = osPriorityBelowNormal,
        .stack_mem  = fp_cli_stack,
        .stack_size = sizeof(fp_cli_stack),
		.cb_mem = &fp_cli_h_taskControlBlock,
		.cb_size = sizeof(fp_cli_h_taskControlBlock)
    };
    fp_cli_h = osThreadNew(fp_cli_func, NULL, &fp_cli_attr);
    configASSERT(fp_cli_h != NULL);

}
