#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "atomic_custom.h"
#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uart_driver_s{
	UART_HandleTypeDef *uart_handle;
	ring_buffer_t tx_ring_buffer;
	ring_buffer_t rx_ring_buffer;
	volatile uint16_t last_tx_size;
	volatile uint16_t last_rx_len;
	uint16_t tx_chunk_size;
	uint16_t dma_rx_buffer_size;

	uint8_t  *dma_rx_buffer;
	volatile uint16_t dma_last_pos;
}uart_driver_t;

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
		);

void uart_driver_deinit(uart_driver_t *uart_driver_handle);

void TxCpltCallback_routine(uart_driver_t *uart_driver_handle, UART_HandleTypeDef *huart);
void IDLECallback_routine(uart_driver_t *uart_driver_handle, UART_HandleTypeDef *huart);

size_t uart_send_data(uart_driver_t *uart_driver_handle, char* data, size_t len);
size_t uart_recv_data(uart_driver_t *uart_driver_handle, char* out_buf, size_t max_len);
size_t uart_data_available_for_read(uart_driver_t *uart_driver_handle);
size_t uart_data_available_for_write(uart_driver_t *uart_driver_handle);
void uart_data_rx_flush(uart_driver_t *uart_driver_handle);



#ifdef __cplusplus
}
#endif

#endif
