/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define USART1_RX_BUFFER_SIZE 2048
#define USART1_TX_BUFFER_SIZE 2048
#define USART1_TX_CHUNK_SIZE 128
#define RX_DMA_BUF_SIZE 128
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
size_t usart1_send_data(char* data, size_t len);
void HAL_UART_IDLECallback(UART_HandleTypeDef *huart);
size_t usart1_recv_data(char* out_buf, size_t max_len);
size_t usart1_data_available_for_read();
size_t usart1_data_available_for_write();
void usart1_data_rx_flush();
//uint8_t usart1_is_connected(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

