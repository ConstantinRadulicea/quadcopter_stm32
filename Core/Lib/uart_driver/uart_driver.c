#include "uart_driver.h"
#include "stm32f4xx_hal_dma.h"
#include "string.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


static void read_rx_dma_buffer(uart_driver_t *uart_driver_handle)
{
	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
	{
		// Position DMA has written up to (bytes received so far)
		int dma_pos = (int)(uart_driver_handle->dma_rx_buffer_size - __HAL_DMA_GET_COUNTER(uart_driver_handle->uart_handle->hdmarx));

		if (dma_pos == uart_driver_handle->dma_last_pos) return; // nothing new

		if (dma_pos > uart_driver_handle->dma_last_pos) {
			// linear chunk
			size_t len = dma_pos - uart_driver_handle->dma_last_pos;
			ring_buffer_enqueue_arr(&(uart_driver_handle->rx_ring_buffer), &(uart_driver_handle->dma_rx_buffer[uart_driver_handle->dma_last_pos]), len);
		} else {
			// wrapped: tail then head
			size_t tail_len = uart_driver_handle->dma_rx_buffer_size - uart_driver_handle->dma_last_pos;
			ring_buffer_enqueue_arr(&(uart_driver_handle->rx_ring_buffer), &(uart_driver_handle->dma_rx_buffer[uart_driver_handle->dma_last_pos]), tail_len);
			if (dma_pos) {
				ring_buffer_enqueue_arr(&(uart_driver_handle->rx_ring_buffer), &(uart_driver_handle->dma_rx_buffer[0]), dma_pos);
			}
		}

		uart_driver_handle->dma_last_pos = dma_pos;
	}
}



static void start_tx_if_idle(uart_driver_t *uart_driver_handle, int force_state)
{
	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
	{
		if (((uart_driver_handle->uart_handle->gState == HAL_UART_STATE_READY) || force_state != 0) && ring_buffer_used_space(&(uart_driver_handle->tx_ring_buffer)) > 0)
		{

			size_t linear_used = ring_buffer_linear_used_space(&(uart_driver_handle->tx_ring_buffer));
			uint16_t frame_size = MIN(linear_used, uart_driver_handle->tx_chunk_size);
			uint8_t *data = ring_buffer_read_ptr(&(uart_driver_handle->tx_ring_buffer));

			uart_driver_handle->last_tx_size = frame_size;

			//EXIT_CRITICAL();
			if (HAL_UART_Transmit_DMA(uart_driver_handle->uart_handle, data, frame_size) != HAL_OK) {
				//ENTER_CRITICAL();
				uart_driver_handle->last_tx_size = 0;
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


void TxCpltCallback_routine(uart_driver_t *uart_driver_handle, UART_HandleTypeDef *huart)
{
	if(uart_driver_handle->uart_handle->Instance == huart->Instance){

    	if(uart_driver_handle->uart_handle->gState == HAL_UART_STATE_READY)
    	{
			// Advance tail for previously sent chunk
			ring_buffer_advance_tail(&(uart_driver_handle->tx_ring_buffer), uart_driver_handle->last_tx_size);

			size_t linear_used = ring_buffer_linear_used_space(&(uart_driver_handle->tx_ring_buffer));
			uint16_t frame_size = MIN(linear_used, uart_driver_handle->tx_chunk_size);
			uint8_t *next_chunk = ring_buffer_read_ptr(&(uart_driver_handle->tx_ring_buffer));
			uart_driver_handle->last_tx_size = frame_size;

			if (frame_size > 0)
			{
		        if (HAL_UART_Transmit_DMA(uart_driver_handle->uart_handle, next_chunk, frame_size) != HAL_OK) {
		        	uart_driver_handle->last_tx_size = 0;
		        }
				//HAL_UART_Transmit_IT(huart, next_chunk, frame_size);
				// Do NOT advance tail here. Advance it next time, after TX completes.
			}
    	}
	}
}


void IDLECallback_routine(uart_driver_t *uart_driver_handle, UART_HandleTypeDef *huart)
{
	if(uart_driver_handle->uart_handle->Instance == huart->Instance){
		read_rx_dma_buffer(uart_driver_handle);
	}
}


//void usart1_recover(void)
//{
//    HAL_UART_AbortTransmit(&huart1);
//    usart1_last_tx_size = 0;
//    usart1_start_tx_if_idle(1);
//
//    // Stop RX DMA if needed
//    if (HAL_DMA_GetState(huart1.hdmarx) != HAL_DMA_STATE_READY) {
//        HAL_UART_DMAStop(&huart1);
//    }
//
//    // Clear UART error flags
//    __HAL_UART_CLEAR_PEFLAG(&huart1);
//    __HAL_UART_CLEAR_FEFLAG(&huart1);
//    __HAL_UART_CLEAR_NEFLAG(&huart1);
//    __HAL_UART_CLEAR_OREFLAG(&huart1);
//
//    // Clear DMA TC flag safely
//    __HAL_DMA_CLEAR_FLAG(huart1.hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(huart1.hdmarx));
//
//    // Recover RX DMA
//    uint16_t remaining = __HAL_DMA_GET_COUNTER(huart1.hdmarx);
//    uint16_t received = usart1_last_rx_len - remaining;
//    ring_buffer_advance_head(&usart1_rx_ring_buffer, received);
//
//    uint16_t space = ring_buffer_linear_free_space(&usart1_rx_ring_buffer);
//    uint8_t *write_ptr = ring_buffer_write_ptr(&usart1_rx_ring_buffer);
//
//    // HAL_UART_Receive_DMA(&huart1, write_ptr, space);
//    // usart1_last_rx_len = space;
//
//      HAL_UART_Receive_DMA(&huart1, usart1_dma_rx, RX_DMA_BUF_SIZE);
//  usart1_last_rx_len = RX_DMA_BUF_SIZE;
//  usart1_dma_last_pos = 0;
//}

//void usart1_restart(uart_driver_t *uart_driver_handle)
//{
//    // 1. Deinit UART (also unlinks DMA internally)
//    HAL_UART_DeInit(uart_driver_handle->uart_handle);
//
//    // 2. Deinit DMA streams manually
//    HAL_DMA_DeInit(uart_driver_handle->uart_handle->hdmarx);
//    HAL_DMA_DeInit(uart_driver_handle->uart_handle->hdmatx);
//
//    // 3. Reset USART1 peripheral
//    __HAL_RCC_USART1_FORCE_RESET();
//    __HAL_RCC_USART1_RELEASE_RESET();
//
//    // 4. Reinit USART1 and DMA
//    MX_USART1_UART_Init();  // Reinitializes UART and links DMA
//    //MX_DMA_Init();          // Only needed if you have a custom DMA init function
//
//    // 5. Restart RX DMA if needed
//    uint16_t space = ring_buffer_linear_free_space(&(uart_driver_handle->rx_ring_buffer));
//    uint8_t *write_ptr = ring_buffer_write_ptr(&(uart_driver_handle->rx_ring_buffer));
//    // usart1_last_rx_len = space;
//    // HAL_UART_Receive_DMA(&huart1, write_ptr, space);
//
//      HAL_UART_Receive_DMA(uart_driver_handle->uart_handle, uart_driver_handle->dma_rx_buffer, uart_driver_handle->dma_rx_buffer_size);
//	  uart_driver_handle->last_rx_len = uart_driver_handle->dma_rx_buffer_size;
//	  uart_driver_handle->dma_last_pos = 0;
//
//	  start_tx_if_idle(uart_driver_handle, 1);
//}


size_t uart_send_data(uart_driver_t *uart_driver_handle, char* data, size_t len)
{
    if (data == NULL || len == 0) {
        return 0;
    }
    size_t bytes_written;
    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
        // Try to enqueue data
    	bytes_written = ring_buffer_enqueue_arr(&(uart_driver_handle->tx_ring_buffer), (uint8_t*)data, len);
    }

    start_tx_if_idle(uart_driver_handle, 0);

    return bytes_written;
}

size_t uart_recv_data(uart_driver_t *uart_driver_handle, char* out_buf, size_t max_len)
{
    if (out_buf == NULL || max_len == 0) {
        return 0;
    }
    size_t bytes_read;
    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
    	read_rx_dma_buffer(uart_driver_handle);
    	bytes_read = ring_buffer_dequeue_arr(&(uart_driver_handle->rx_ring_buffer), (uint8_t*)out_buf, max_len);
    }

    return bytes_read;
}

size_t uart_data_available_for_read(uart_driver_t *uart_driver_handle)
{
	size_t used;

    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
    	read_rx_dma_buffer(uart_driver_handle);
    	used = ring_buffer_used_space(&(uart_driver_handle->rx_ring_buffer));
    }

    return used;
}

size_t uart_data_available_for_write(uart_driver_t *uart_driver_handle)
{
	size_t free_space;

    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
    	free_space = ring_buffer_free_space(&(uart_driver_handle->tx_ring_buffer));
    }

    return free_space;
}

void uart_data_rx_flush(uart_driver_t *uart_driver_handle){
    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    {
    	read_rx_dma_buffer(uart_driver_handle);
        __HAL_UART_CLEAR_FLAG(uart_driver_handle->uart_handle, UART_FLAG_RXNE);
        __HAL_UART_CLEAR_OREFLAG(uart_driver_handle->uart_handle);
        ring_buffer_clear(&(uart_driver_handle->rx_ring_buffer));
    }
}


void uart_driver_init(
		uart_driver_t *uart_driver_handle,
		UART_HandleTypeDef *huart,
		uint8_t* rx_ring_buffer,
		uint8_t* tx_ring_buffer,
		uint8_t* dma_rx_ring_buffer,
		size_t rx_ring_buffer_size,
		size_t tx_ring_buffer_size,
		size_t dma_rx_ring_buffer_size,
		uint16_t tx_chunk_size

		){
	memset(uart_driver_handle, 0, sizeof(uart_driver_t));
	uart_driver_handle->uart_handle = huart;
	ring_buffer_init(&(uart_driver_handle->tx_ring_buffer), tx_ring_buffer, tx_ring_buffer_size);
	ring_buffer_init(&(uart_driver_handle->rx_ring_buffer), rx_ring_buffer, rx_ring_buffer_size);
	uart_driver_handle->tx_chunk_size = tx_chunk_size;

	uart_driver_handle->last_rx_len = 0;
	uart_driver_handle->last_tx_size = 0;

	uart_driver_handle->dma_rx_buffer = dma_rx_ring_buffer;
	uart_driver_handle->dma_rx_buffer_size = dma_rx_ring_buffer_size;
	uart_driver_handle->dma_last_pos = 0;

	  uint16_t rx_buffer_remaining_free = (uint16_t)ring_buffer_linear_free_space(&(uart_driver_handle->rx_ring_buffer));
	  uint8_t *write_ptr = ring_buffer_write_ptr(&(uart_driver_handle->rx_ring_buffer));
	//   usart1_last_rx_len = rx_buffer_remaining_free;
	//   HAL_UART_Receive_DMA(&huart1, write_ptr, rx_buffer_remaining_free);

	  HAL_UART_Receive_DMA(uart_driver_handle->uart_handle, uart_driver_handle->dma_rx_buffer, uart_driver_handle->dma_rx_buffer_size);
	  uart_driver_handle->last_rx_len = uart_driver_handle->dma_rx_buffer_size;
	  uart_driver_handle->dma_last_pos = 0;

	  __HAL_UART_ENABLE_IT(uart_driver_handle->uart_handle, UART_IT_IDLE);
}

void uart_driver_deinit(uart_driver_t *uart_driver_handle){
	ring_buffer_clear(&(uart_driver_handle->tx_ring_buffer));
	ring_buffer_clear(&(uart_driver_handle->rx_ring_buffer));
	uart_driver_handle->last_rx_len = 0;
	uart_driver_handle->last_tx_size = 0;
	uart_driver_handle->dma_last_pos = 0;
}



