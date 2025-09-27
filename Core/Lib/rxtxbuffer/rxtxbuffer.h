#ifndef __RXTXBUFFER_H__
#define __RXTXBUFFER_H__

/**
 * @file rxtxbuffer_t.h
 * @brief Linear FIFO-style transmit/receive buffer for UART with DMA support.
 *
 * This structure implements a linear buffer for managing TX/RX buffer queues, where:
 * - `data_size` marks how much valid buffer has been written to the buffer.
 * - `sent_size` marks how much of that buffer has already been consumed/transmitted.
 * - The unread portion lies between `sent_size` and `data_size`.
 * - New buffer is written after `data_size`, and the buffer can be compacted with `rxtxbuffer_shift_data_buf()`.
 *
 * The layout of the buffer is as follows:
 *
 *     +-------------------------------------------------------------------------+
 *     | Sent          |  To be transmitted          | Free space                |
 *     | [0:sent_size) |  [sent_size:data_size)      | [data_size:capacity)      |
 *     +-------------------------------------------------------------------------+
 *     ^               ^                             ^                           ^
 *     buffer          sent_size                     data_size                   capacity
 *
 * Example usage for UART TX with DMA:
 * - Write new buffer at `rxtxbuffer_free_space_ptr()`, increase with `rxtxbuffer_data_increase_size()`.
 * - Start DMA from `rxtxbuffer_data_ptr()` of length `rxtxbuffer_data_remaining()`.
 * - On completion, call `rxtxbuffer_sent_data_increase_size()` and optionally `rxtxbuffer_shift_data_buf()` to compact the buffer.
 *
 * This approach simplifies chunked DMA transmission without needing a true circular buffer.
 */


#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RXTXBUFFER_SUCCESS						0
#define RXTXBUFFER_ERROR_INVALID_PARAMETERS		1

#define RXTXBUFFER_DEFAULT(vec) vec.buffer=NULL; vec.capacity=(size_t)0; vec.sent_size=(size_t)0; vec.data_size=(size_t)0

#define RXTXBUFFER_CAST(ptr, type) ((type)(ptr))

typedef struct rxtxbuffer {
    uint8_t*  buffer;      // Pointer to raw memory buffer
    size_t capacity;    // Maximum size of buffer in bytes
    volatile size_t sent_size;   // Amount of data already consumed/transmitted
    volatile size_t data_size;   // Amount of valid data currently stored
} rxtxbuffer_t;


/**
 * @brief Get raw pointer to the internal buffer.
 * @param pt Pointer to RX/TX buffer struct
 * @return Pointer to underlying memory buffer
 */
uint8_t* rxtxbuffer_buffer_ptr(rxtxbuffer_t* pt);

/**
 * @brief Get number of bytes already consumed/transmitted.
 * @param pt Pointer to RX/TX buffer struct
 * @return Number of bytes marked as "sent"
 */
size_t rxtxbuffer_sent_size(rxtxbuffer_t* pt);

/**
 * @brief Get number of valid data bytes currently stored in the buffer.
 * @param pt Pointer to RX/TX buffer struct
 * @return Data size in bytes
 */
size_t rxtxbuffer_data_size(rxtxbuffer_t* pt);

/**
 * @brief Get total capacity of the buffer.
 * @param pt Pointer to RX/TX buffer struct
 * @return Maximum buffer size in bytes
 */
size_t rxtxbuffer_capacity(rxtxbuffer_t* pt);

/**
 * @brief Initialize a buffer manager with a memory block.
 * @param _Cvector Pointer to RX/TX buffer struct
 * @param buf Pointer to pre-allocated memory block
 * @param _capacity Size of the memory block in bytes
 * @return 0 on success
 */
uint8_t rxtxbuffer_init(rxtxbuffer_t* _Cvector, uint8_t* buf, size_t _capacity);

/**
 * @brief Get total free space available in the buffer (for new data).
 * @param sendbuf Pointer to RX/TX buffer struct
 * @return Number of free bytes available
 */
size_t rxtxbuffer_free_space(rxtxbuffer_t* sendbuf);

/**
 * @brief Get number of bytes still pending transmission (not sent yet).
 * @param sendbuf Pointer to RX/TX buffer struct
 * @return Number of remaining bytes to send
 */
size_t rxtxbuffer_data_remaining(rxtxbuffer_t* sendbuf);

/**
 * @brief Increase the number of bytes that were sent.
 * @param pt Pointer to RX/TX buffer struct
 * @param sent_data_size Number of bytes that were transmitted
 */
void rxtxbuffer_sent_data_increase_size(rxtxbuffer_t* pt, size_t sent_data_size);

/**
 * @brief Increase the number of valid data bytes (after receiving new data).
 * @param pt Pointer to RX/TX buffer struct
 * @param recved_data_size Number of bytes received and added
 */
void rxtxbuffer_data_increase_size(rxtxbuffer_t* pt, size_t recved_data_size);

/**
 * @brief Decrease the total valid data size in the buffer.
 *        Typically used when discarding processed or invalid data.
 * @param pt Pointer to RX/TX buffer struct
 * @param decreased_size Number of bytes to remove from data_size
 */
void rxtxbuffer_data_decrease_size(rxtxbuffer_t* pt, size_t decreased_size);

/**
 * @brief Decrease the "sent size" counter, effectively rewinding transmission progress.
 *        Typically used when a transmission fails and the same data must be re-sent.
 * @param pt Pointer to RX/TX buffer struct
 * @param decreased_size Number of bytes to subtract from sent_size
 */
void rxtxbuffer_sent_data_decrease_size(rxtxbuffer_t* pt, size_t decreased_size);

/**
 * @brief Clear the entire buffer.
 *        All data is discarded and both counters are reset to 0.
 * @param pt Pointer to RX/TX buffer struct
 */
void rxtxbuffer_clear(rxtxbuffer_t* pt);

/**
 * @brief Reset the "sent size" counter without modifying data_size.
 *        Typically used when transmission restarts or is retried.
 * @param pt Pointer to RX/TX buffer struct
 */
void rxtxbuffer_reset_sent_size(rxtxbuffer_t* pt);


size_t rxtxbuffer_push_arr(rxtxbuffer_t* pt, uint8_t* data, size_t data_size);

/**
 * @brief Get pointer to the valid data region in the buffer.
 * @param pt Pointer to RX/TX buffer struct
 * @return Pointer to start of valid data
 */
uint8_t* rxtxbuffer_data_ptr(rxtxbuffer_t* pt);

/**
 * @brief Get pointer to the free space region in the buffer (where new data can be written).
 * @param pt Pointer to RX/TX buffer struct
 * @return Pointer to first free byte
 */
uint8_t* rxtxbuffer_free_space_ptr(rxtxbuffer_t* pt);

/**
 * @brief Shift valid data in the buffer to the beginning, removing consumed bytes.
 *        Typically used after transmission to make space for new incoming data.
 * @param pt Pointer to RX/TX buffer struct
 */
void rxtxbuffer_shift_data_buf(rxtxbuffer_t* pt);



#ifdef __cplusplus
}
#endif

#endif // !__RXTXBUFFER_H__
