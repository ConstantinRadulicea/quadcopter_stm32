#include "rtos_flight_controller.h"

#include "driver_mpu6500_basic.h"
#include <stdio.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "imu.h"
#include "cmsis_os2.h"
#include "tim.h"
#include "atomic_custom.h"
#include "usart.h"
#include "crc.h"
#include "fp_cli.h"
#include "rc_control_recv_routine.h"
#include "gazebo_link.h"

#define ENABLE_HIL 1

#define ENABLE_ESC_CALIBRATION_BUILD 0
#define ENABLE_CLI 1

flight_control_loop_t fcl;

SemaphoreHandle_t g_motors_throttle_mutex;
float g_motors_throttle[4];
pwm_t esc_motors[4];

static TaskHandle_t ctrl_task_h;
static TaskHandle_t write_motor_main_h;

/*__attribute__((section(".ccmram"), aligned(32)))*/ uint8_t flight_stack[(2*1024)];
/*__attribute__((section(".ccmram"), aligned(32)))*/ uint8_t write_stack[(2*1024)];
/*__attribute__((section(".ccmram"), aligned(32)))*/ uint8_t rc_stack[(3*1024)];
/*__attribute__((section(".ccmram"), aligned(32)))*/ uint8_t telem_stack[(4*1024)];

#if ENABLE_CLI != 0
/*__attribute__((section(".ccmram"), aligned(32)))*/ uint8_t fp_cli_stack[(4*1024)];
#endif

/*__attribute__((section(".ccmram"), aligned(32)))*/ uint8_t lwip_feed_stack[(2*1024)];
/*__attribute__((section(".ccmram"), aligned(32)))*/ uint8_t lwip_example_stack[(2*1024)];

osThreadAttr_t flight_attr, write_attr, rc_attr, telem_attr, fp_cli_attr, lwip_feed_attr, lwip_example_attr;
StaticTask_t flight_h_taskControlBlock;
StaticTask_t write_h_taskControlBlock;
StaticTask_t rc_h_taskControlBlock;
StaticTask_t telem_h_taskControlBlock;
StaticTask_t fp_cli_h_taskControlBlock;
StaticTask_t lwip_feed_h_taskControlBlock;
StaticTask_t lwip_example_h_taskControlBlock;
// Thread IDs
static osThreadId_t flight_h;
static osThreadId_t write_h;
static osThreadId_t rc_h;
static osThreadId_t telem_h;
static osThreadId_t fp_cli_h;
static osThreadId_t lwip_feed_h;
static osThreadId_t lwip_example_h;


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
#if ENABLE_HIL != 0
        static float accelerometer_bias_hil[3] = { 0.0f, 0.0f, 0.0f};
        static float accelerometer_A_1_hil[3][3] = {
        		{1.0f, 0.0f, 0.0f},
        		{0.0f, 1.0f, 0.0f},
        		{0.0f, 0.0f, 1.0f}
        };
        static float gyro_bias_hil[3] = { 0.0f, 0.0f, 0.0f};
        static quaternion ground_default_position_q_hil = { .w = 1.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f };

    	imu_set_accel_bias(&fcl.imu, (coord3D) { accelerometer_bias_hil[0], accelerometer_bias_hil[1], accelerometer_bias_hil[2] }, accelerometer_A_1_hil);
    	imu_set_gyro_bias(&fcl.imu, (coord3D) { gyro_bias_hil[0], gyro_bias_hil[1], gyro_bias_hil[2] });
    	imu_set_leveled_attitude(&fcl.imu, ground_default_position_q_hil);
#endif

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
//        	accel[0] = degrees(fcl.imu.accel_bias[0];
//        	accel[1] = -degrees(fcl.imu.accel_bias[1];
//        	accel[2] = -degrees(fcl.imu.accel_bias[2];
//        	gyro[0] = degrees(fcl.imu.gyro_bias[0]);
//        	gyro[1] = -degrees(fcl.imu.gyro_bias[1]);
//        	gyro[2] = -degrees(fcl.imu.gyro_bias[2]);

            accel_data.x = G2MS2(accel[0]);
            accel_data.y = -G2MS2(accel[1]);
            accel_data.z = -G2MS2(accel[2]);
            gyro_data.x = radians(gyro[0]);
            gyro_data.y = -radians(gyro[1]);
            gyro_data.z = -radians(gyro[2]);

#if ENABLE_HIL != 0
            gazebo_link_recv_loop();
            accel_data.x = hil_link_data_recv.accel_x_g;
            accel_data.y = hil_link_data_recv.accel_y_g;
            accel_data.z = hil_link_data_recv.accel_z_g;

            gyro_data.x = hil_link_data_recv.gyro_x_rad_s;
            gyro_data.y = hil_link_data_recv.gyro_y_rad_s;
            gyro_data.z = hil_link_data_recv.gyro_z_rad_s;
#endif
            flight_control_loop_update_imu(&fcl, gyro_data, accel_data);
        }
        flight_control_loop_tick(&fcl);
#if ENABLE_HIL != 0
        float motors_throttle_temp[4];
        flight_control_loop_get_motors_throttle(&fcl, motors_throttle_temp);

        // Quadcopter motors
        // Front Left  (0) (CW)
        // Front Right (1) (CCW)
        // Rear  Left  (2) (CCW)
        // Rear  Right (3) (CW)

        // motors		Gazebo		Quadcopter
        // Front Right (0) (CCW)	(1)
        // Rear  Left  (1) (CCW)	(2)
        // Front Left  (2) (CW)		(0)
        // Rear  Right (3) (CW)		(3)


        hil_link_data_send.motor_0 = motors_throttle_temp[1];
        hil_link_data_send.motor_1 = motors_throttle_temp[2];
        hil_link_data_send.motor_2 = motors_throttle_temp[0];
        hil_link_data_send.motor_3 = motors_throttle_temp[3];
        gazebo_link_send_loop();
#endif
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


#define TELEM_BUF_SZ 512
char telem_frame[TELEM_BUF_SZ];

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

//        OUT_PRINTF("%.3f;%.3f;%.3f;", degrees(angles.x), degrees(angles.y), degrees(angles.z));
//        OUT_PRINTF("%.3f;%.3f;%.3f;", body_frame_accel.x, body_frame_accel.y, body_frame_accel.z);
//        OUT_PRINTF("%.3f;%.3f;%.3f;", body_frame_gyro.x, body_frame_gyro.y, body_frame_gyro.z);
////        OUT_PRINTF("%.3f;%.3f;%.3f;", raw_accel.x, raw_accel.y, raw_accel.z);
////        OUT_PRINTF("%.3f;%.3f;%.3f;", raw_gyro.x, raw_gyro.y, raw_gyro.z);
//        OUT_PRINTF("%.3f;%.3f;%.3f;%.3f;", local_motors_throttle[0], local_motors_throttle[1], local_motors_throttle[2], local_motors_throttle[3]);

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
	//	OUT_PRINTF("%.3f;%.3f;%.3f;", target_attitude.x, target_attitude.y, target_attitude.z);
//	OUT_PRINTF("%.3f;", target_throttle);

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
//	OUT_PRINTF("%.3f;%.3f;%.3f;", target_roll_rate, target_pitch_rate, target_yaw_rate);


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
//	OUT_PRINTF("%.3f;%.3f;%.3f;", degrees(pid_roll_output), degrees(pid_pitch_output), degrees(pid_yaw_output));

//    OUT_PRINTF("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)flight_h) * sizeof(StackType_t)));
//    OUT_PRINTF("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)write_h) * sizeof(StackType_t)));
//    OUT_PRINTF("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)rc_h)     * sizeof(StackType_t)));
//    OUT_PRINTF("%lu;", (unsigned long)(uxTaskGetStackHighWaterMark((TaskHandle_t)telem_h)  * sizeof(StackType_t)));


//	OUT_PRINTF("\r\n");

	//	OUT_PRINTF("%.3f;%.3f;%.3f;", target_attitude.x, target_attitude.y, target_attitude.z);

	snprintf(telem_frame, TELEM_BUF_SZ,
			"%.3f;%.3f;%.3f;"
			"%.3f;%.3f;%.3f;"
			"%.3f;%.3f;%.3f;"
			"%.3f;%.3f;%.3f;%.3f;"
			"%.3f;%.3f;%.3f;"
			"%.3f;"
			"%.3f;%.3f;%.3f;"
			"\r\n",
			degrees(angles.x), degrees(angles.y), degrees(angles.z),
			body_frame_accel.x, body_frame_accel.y, body_frame_accel.z,
			degrees(body_frame_gyro.x), degrees(body_frame_gyro.y), degrees(body_frame_gyro.z),
			local_motors_throttle[0], local_motors_throttle[1], local_motors_throttle[2], local_motors_throttle[3],
			degrees(target_attitude.x), degrees(target_attitude.y), degrees(target_attitude.z),
			target_throttle,
			(pid_roll_output), (pid_pitch_output), (pid_yaw_output)
			);
	OUT_PRINTF("%s", telem_frame);
    }
}

#include "fp_cli.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "fp_cli_server.h"
//char fp_cli_read_buffer[128];
static void fp_cli_func(void *arg){
	//size_t read_bytes = 0;
	fp_cli_example_minimal_init();
//	for(;;){
//		read_bytes = CDC_recv_data(fp_cli_read_buffer, sizeof(fp_cli_read_buffer)-1);
//		lwshell_input_ex(&lwshell_cli, fp_cli_read_buffer, read_bytes);
//		vTaskDelay(pdMS_TO_TICKS(100));
//	}

	tcp_socket_fp_cli_server_task(NULL);

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

#include "lwip_example.h"

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

    net_ppp_start();

    lwip_feed_attr = (osThreadAttr_t){
        .name       = "lwip_feed",
        .priority   = osPriorityBelowNormal,
        .stack_mem  = lwip_feed_stack,
        .stack_size = sizeof(lwip_feed_stack),
		.cb_mem = &lwip_feed_h_taskControlBlock,
		.cb_size = sizeof(lwip_feed_h_taskControlBlock)
    };
    lwip_feed_h = osThreadNew(ppp_feed_task, NULL, &lwip_feed_attr);
    configASSERT(lwip_feed_h != NULL);

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
        .priority   = osPriorityHigh,
        .stack_mem  = write_stack,
        .stack_size = sizeof(write_stack),
		.cb_mem = &write_h_taskControlBlock,
		.cb_size = sizeof(write_h_taskControlBlock)
    };
    write_h = osThreadNew(write_motor_main, NULL, &write_attr);
    configASSERT(write_h != NULL);

    rc_attr = (osThreadAttr_t){
        .name       = "rc_control_main",
        .priority   = osPriorityAboveNormal,
        .stack_mem  = rc_stack,
        .stack_size = sizeof(rc_stack),
		.cb_mem = &rc_h_taskControlBlock,
		.cb_size = sizeof(rc_h_taskControlBlock)
    };
    rc_h = osThreadNew(rc_control_main, &fcl, &rc_attr);
    configASSERT(rc_h != NULL);

    telem_attr = (osThreadAttr_t){
        .name       = "print_telemetry_data",
        .priority   = osPriorityLow,
        .stack_mem  = telem_stack,
        .stack_size = sizeof(telem_stack),
		.cb_mem = &telem_h_taskControlBlock,
		.cb_size = sizeof(telem_h_taskControlBlock)
    };
    telem_h = osThreadNew(print_telemetry_data, NULL, &telem_attr);
    configASSERT(telem_h != NULL);

#if ENABLE_CLI != 0
    fp_cli_attr = (osThreadAttr_t){
        .name       = "fp_cli_func",
        .priority   = osPriorityLow,
        .stack_mem  = fp_cli_stack,
        .stack_size = sizeof(fp_cli_stack),
		.cb_mem = &fp_cli_h_taskControlBlock,
		.cb_size = sizeof(fp_cli_h_taskControlBlock)
    };
    fp_cli_h = osThreadNew(fp_cli_func, NULL, &fp_cli_attr);
    configASSERT(fp_cli_h != NULL);
#endif



//    lwip_example_attr = (osThreadAttr_t){
//        .name       = "lwip_example",
//        .priority   = osPriorityBelowNormal,
//        .stack_mem  = lwip_example_stack,
//        .stack_size = sizeof(lwip_example_stack),
//		.cb_mem = &lwip_example_h_taskControlBlock,
//		.cb_size = sizeof(lwip_example_h_taskControlBlock)
//    };
//    lwip_example_h = osThreadNew(tcp_echo_socket_task, NULL, &lwip_example_attr);
//    configASSERT(lwip_example_h != NULL);
//    for(;;){
//    	osDelay(1000);
//    }

}
