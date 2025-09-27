/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "atomic_custom.h"
#include "ringbuffer.h"
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static uint8_t usart1_rx_buffer[USART1_RX_BUFFER_SIZE];
static uint8_t usart1_tx_buffer[USART1_TX_BUFFER_SIZE];
static ring_buffer_t usart1_tx_ring_buffer;
static ring_buffer_t usart1_rx_ring_buffer;
volatile uint16_t usart1_last_tx_size = 0;
volatile uint16_t usart1_last_rx_len = 0;

/* ---- RX DMA state ---- */
static uint8_t  usart1_dma_rx[RX_DMA_BUF_SIZE];
static volatile uint16_t usart1_dma_last_pos = 0;

void usart1_start_tx_if_idle(int force_state);
void usart1_recover(void);
void usart1_restart(void);

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */
	ring_buffer_init(&usart1_tx_ring_buffer, usart1_tx_buffer, USART1_TX_BUFFER_SIZE);
	ring_buffer_init(&usart1_rx_ring_buffer, usart1_rx_buffer, USART1_RX_BUFFER_SIZE);
	usart1_last_tx_size = 0;
	usart1_last_rx_len = 0;
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_8;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  uint16_t rx_buffer_remaining_free = (uint16_t)ring_buffer_linear_free_space(&usart1_rx_ring_buffer);
  uint8_t *write_ptr = ring_buffer_write_ptr(&usart1_rx_ring_buffer);
//   usart1_last_rx_len = rx_buffer_remaining_free;
//   HAL_UART_Receive_DMA(&huart1, write_ptr, rx_buffer_remaining_free);

  HAL_UART_Receive_DMA(&huart1, usart1_dma_rx, RX_DMA_BUF_SIZE);
  usart1_last_rx_len = RX_DMA_BUF_SIZE;
  usart1_dma_last_pos = 0;

  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */
	usart1_last_tx_size = 0;
	usart1_last_rx_len = 0;
	ring_buffer_clear(&usart1_tx_ring_buffer);
	ring_buffer_clear(&usart1_rx_ring_buffer);
  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {

    	if(huart1.gState == HAL_UART_STATE_READY)
    	{
			// Advance tail for previously sent chunk
			ring_buffer_advance_tail(&usart1_tx_ring_buffer, usart1_last_tx_size);

			size_t linear_used = ring_buffer_linear_used_space(&usart1_tx_ring_buffer);
			uint16_t frame_size = MIN(linear_used, USART1_TX_CHUNK_SIZE);
			uint8_t *next_chunk = ring_buffer_read_ptr(&usart1_tx_ring_buffer);
			usart1_last_tx_size = frame_size;

			if (frame_size > 0)
			{
		        if (HAL_UART_Transmit_DMA(huart, next_chunk, frame_size) != HAL_OK) {
		            usart1_last_tx_size = 0;
		        }
				//HAL_UART_Transmit_IT(huart, next_chunk, frame_size);
				// Do NOT advance tail here. Advance it next time, after TX completes.
			}
    	}
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
    }
}




void HAL_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1){

    // Position DMA has written up to (bytes received so far)
    int dma_pos = (int)(RX_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx));

    if (dma_pos == usart1_dma_last_pos) return; // nothing new

    if (dma_pos > usart1_dma_last_pos) {
        // linear chunk
        size_t len = dma_pos - usart1_dma_last_pos;
        ring_buffer_enqueue_arr(&usart1_rx_ring_buffer,
                          &usart1_dma_rx[usart1_dma_last_pos], len);
    } else {
        // wrapped: tail then head
        size_t tail_len = RX_DMA_BUF_SIZE - usart1_dma_last_pos;
        ring_buffer_enqueue_arr(&usart1_rx_ring_buffer,
                          &usart1_dma_rx[usart1_dma_last_pos], tail_len);
        if (dma_pos) {
        	ring_buffer_enqueue_arr(&usart1_rx_ring_buffer,
                              &usart1_dma_rx[0], dma_pos);
        }
    }

    usart1_dma_last_pos = dma_pos;
    }
}


void usart1_read_dma_buffer()
{
	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
	{
    // Position DMA has written up to (bytes received so far)
    int dma_pos = (int)(RX_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx));

    if (dma_pos == usart1_dma_last_pos) return; // nothing new

    if (dma_pos > usart1_dma_last_pos) {
        // linear chunk
        size_t len = dma_pos - usart1_dma_last_pos;
        ring_buffer_enqueue_arr(&usart1_rx_ring_buffer,
                          &usart1_dma_rx[usart1_dma_last_pos], len);
    } else {
        // wrapped: tail then head
        size_t tail_len = RX_DMA_BUF_SIZE - usart1_dma_last_pos;
        ring_buffer_enqueue_arr(&usart1_rx_ring_buffer,
                          &usart1_dma_rx[usart1_dma_last_pos], tail_len);
        if (dma_pos) {
        	ring_buffer_enqueue_arr(&usart1_rx_ring_buffer,
                              &usart1_dma_rx[0], dma_pos);
        }
    }

    usart1_dma_last_pos = dma_pos;
	}
}







void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // Handle TX/RX recovery here
        //usart1_recover();
        usart1_restart();
    }
}


void usart1_start_tx_if_idle(int force_state)
{
	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
	{
		if (((huart1.gState == HAL_UART_STATE_READY) || force_state != 0) && ring_buffer_used_space(&usart1_tx_ring_buffer) > 0)
		{

			size_t linear_used = ring_buffer_linear_used_space(&usart1_tx_ring_buffer);
			uint16_t frame_size = MIN(linear_used, USART1_TX_CHUNK_SIZE);
			uint8_t *data = ring_buffer_read_ptr(&usart1_tx_ring_buffer);

			usart1_last_tx_size = frame_size;

			//EXIT_CRITICAL();
			if (HAL_UART_Transmit_DMA(&huart1, data, frame_size) != HAL_OK) {
				//ENTER_CRITICAL();
				usart1_last_tx_size = 0;
				//EXIT_CRITICAL();
			}
			//HAL_UART_Transmit_IT(&huart1, data, frame_size);
		}
		else
		{
			//EXIT_CRITICAL();
		}
	}
}

void usart1_recover(void)
{
    HAL_UART_AbortTransmit(&huart1);
    usart1_last_tx_size = 0;
    usart1_start_tx_if_idle(1);

    // Stop RX DMA if needed
    if (HAL_DMA_GetState(huart1.hdmarx) != HAL_DMA_STATE_READY) {
        HAL_UART_DMAStop(&huart1);
    }

    // Clear UART error flags
    __HAL_UART_CLEAR_PEFLAG(&huart1);
    __HAL_UART_CLEAR_FEFLAG(&huart1);
    __HAL_UART_CLEAR_NEFLAG(&huart1);
    __HAL_UART_CLEAR_OREFLAG(&huart1);

    // Clear DMA TC flag safely
    __HAL_DMA_CLEAR_FLAG(huart1.hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(huart1.hdmarx));

    // Recover RX DMA
    uint16_t remaining = __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    uint16_t received = usart1_last_rx_len - remaining;
    ring_buffer_advance_head(&usart1_rx_ring_buffer, received);

    uint16_t space = ring_buffer_linear_free_space(&usart1_rx_ring_buffer);
    uint8_t *write_ptr = ring_buffer_write_ptr(&usart1_rx_ring_buffer);

    // HAL_UART_Receive_DMA(&huart1, write_ptr, space);
    // usart1_last_rx_len = space;

      HAL_UART_Receive_DMA(&huart1, usart1_dma_rx, RX_DMA_BUF_SIZE);
  usart1_last_rx_len = RX_DMA_BUF_SIZE;
  usart1_dma_last_pos = 0;
}


void usart1_restart(void)
{
    // 1. Deinit UART (also unlinks DMA internally)
    HAL_UART_DeInit(&huart1);

    // 2. Deinit DMA streams manually
    HAL_DMA_DeInit(huart1.hdmarx);
    HAL_DMA_DeInit(huart1.hdmatx);

    // 3. Reset USART1 peripheral
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

    // 4. Reinit USART1 and DMA
    MX_USART1_UART_Init();  // Reinitializes UART and links DMA
    //MX_DMA_Init();          // Only needed if you have a custom DMA init function

    // 5. Restart RX DMA if needed
    uint16_t space = ring_buffer_linear_free_space(&usart1_rx_ring_buffer);
    uint8_t *write_ptr = ring_buffer_write_ptr(&usart1_rx_ring_buffer);
    // usart1_last_rx_len = space;
    // HAL_UART_Receive_DMA(&huart1, write_ptr, space);

      HAL_UART_Receive_DMA(&huart1, usart1_dma_rx, RX_DMA_BUF_SIZE);
  usart1_last_rx_len = RX_DMA_BUF_SIZE;
  usart1_dma_last_pos = 0;

    usart1_start_tx_if_idle(1);
}




size_t usart1_send_data(char* data, size_t len)
{
    if (data == NULL || len == 0) {
        return 0;
    }
    size_t bytes_written;
    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
        // Try to enqueue data
    	bytes_written = ring_buffer_enqueue_arr(&usart1_tx_ring_buffer, (uint8_t*)data, len);
    }

    usart1_start_tx_if_idle(0);

    return bytes_written;
}

size_t usart1_recv_data(char* out_buf, size_t max_len)
{
    if (out_buf == NULL || max_len == 0) {
        return 0;
    }
    size_t bytes_read;
    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
        usart1_read_dma_buffer();
    	bytes_read = ring_buffer_dequeue_arr(&usart1_rx_ring_buffer, (uint8_t*)out_buf, max_len);
    }

    return bytes_read;
}

size_t usart1_data_available_for_read()
{
	size_t used;

    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
        usart1_read_dma_buffer();
    	used = ring_buffer_used_space(&usart1_rx_ring_buffer);
    }

    return used;
}

size_t usart1_data_available_for_write()
{
	size_t free_space;

    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
    	free_space = ring_buffer_free_space(&usart1_tx_ring_buffer);
    }

    return free_space;
}

void usart1_data_rx_flush(){
    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
        usart1_read_dma_buffer();
        __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
        __HAL_UART_CLEAR_OREFLAG(&huart1);
        usart1_rx_ring_buffer.tail = usart1_rx_ring_buffer.head = 0;
    }
}

/* USER CODE END 1 */
