/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "main.h"
#include <stdio.h>
#include "geometry2D.h"
#include "string.h"
#include "atomic_custom.h"
#include "rtos_flight_controller.h"
#include "esc_pwm.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int line_buffer_add_char(char c, char *buf, size_t buf_size, size_t *len)
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
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

osThreadId_t flignt_controller_taskHandle;
uint32_t flignt_controller_taskBuffer[ 2048 ];
osStaticThreadDef_t flignt_controller_taskControlBlock;
const osThreadAttr_t flignt_controller_task_attributes = {
  .name = "usart1_task",
  .cb_mem = &flignt_controller_taskControlBlock,
  .cb_size = sizeof(flignt_controller_taskControlBlock),
  .stack_mem = &flignt_controller_taskBuffer[0],
  .stack_size = sizeof(flignt_controller_taskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for usart1_task */
osThreadId_t usart1_taskHandle;
uint32_t usart1_taskBuffer[ 512 ];
osStaticThreadDef_t usart1_taskControlBlock;
const osThreadAttr_t usart1_task_attributes = {
  .name = "usart1_task",
  .cb_mem = &usart1_taskControlBlock,
  .cb_size = sizeof(usart1_taskControlBlock),
  .stack_mem = &usart1_taskBuffer[0],
  .stack_size = sizeof(usart1_taskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void usart1_task_fcn(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of usart1_task */
  //usart1_taskHandle = osThreadNew(usart1_task_fcn, NULL, &usart1_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  flignt_controller_taskHandle = osThreadNew(app_main, NULL, &flignt_controller_task_attributes);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_usart1_task_fcn */
/**
* @brief Function implementing the usart1_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_usart1_task_fcn */
void usart1_task_fcn(void *argument)
{
  /* USER CODE BEGIN usart1_task_fcn */
    uint8_t uart_recv_buffer[64];
    char print_buffer[128];
    int recv_data;

    char linebuf[64];
    size_t line_len = 0;
    float throttle[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    float duty_cycle;
    pwm_t esc_motors[4];

    duty_cycle = esc_pwm_throttle_to_duty_cycle(ESC_PWM_HZ, ESC_PWM_MIN_US, ESC_PWM_MAX_US, 0.0f);

	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
	{
		pwm_init(&esc_motors[0], MOTOR_1_TIM, MOTOR_1_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
		pwm_init(&esc_motors[1], MOTOR_2_TIM, MOTOR_2_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
		pwm_init(&esc_motors[2], MOTOR_3_TIM, MOTOR_3_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
		pwm_init(&esc_motors[3], MOTOR_4_TIM, MOTOR_4_TIM_CHANNEL, TIMER_CLOCK, ESC_PWM_HZ, duty_cycle);
	}


  /* Infinite loop */
  for(;;)
  {
      while (usart1_data_available_for_read() > 0)
      {
          recv_data = usart1_recv_data((char*)uart_recv_buffer, sizeof(uart_recv_buffer));
          //usart1_send_data(uart_recv_buffer, recv_data);
          for(int i=0; i<recv_data; i++){

			  if (line_buffer_add_char(uart_recv_buffer[i], linebuf, sizeof(linebuf), &line_len)) {
				  // We got a full line ending with \n\r
				  //0.0;0.0;0.0;0.1
				  if (sscanf(linebuf, "%f;%f;%f;%f", &throttle[0], &throttle[1], &throttle[2], &throttle[3]) == 4) {
					  //sprintf(print_buffer, "received: %f;%f;%f;%f\n", throttle[0], throttle[1], throttle[2], throttle[3]);
					  //usart1_send_data(print_buffer, strlen(print_buffer));
				  } else {
					  //sprintf(print_buffer, "Invalid input: %s\n", linebuf);
					  //usart1_send_data(print_buffer, strlen(print_buffer));
				  }
			  }
          }
          //_write(stdout, uart_recv_buffer, recv_data);
      }
	  //sprintf(print_buffer, "received: %f;%f;%f;%f\n", throttle[0], throttle[1], throttle[2], throttle[3]);
	  //usart1_send_data(print_buffer, strlen(print_buffer));
      for (size_t i = 0; i < 4; i++) {
    	  duty_cycle = esc_pwm_throttle_to_duty_cycle(ESC_PWM_HZ, ESC_PWM_MIN_US, ESC_PWM_MAX_US, throttle[i]);
    	  ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    	  {
    		  pwm_set_duty(&esc_motors[i], duty_cycle);
    	  }
      }

	  sprintf(print_buffer, "%.3f;%.3f;%.3f;%.3f\n", throttle[0], throttle[1], throttle[2], throttle[3]);
	  usart1_send_data(print_buffer, strlen(print_buffer));

      vTaskDelay(pdMS_TO_TICKS(100));
  }
  /* USER CODE END usart1_task_fcn */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

