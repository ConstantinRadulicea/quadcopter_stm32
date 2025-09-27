#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    // Return codes
#define RING_BUFFER_TRUE                      1
#define RING_BUFFER_FALSE                     0

#define RING_BUFFER_SUCCESS                   0
#define RING_BUFFER_ERROR_INVALID_PARAMETERS  1
#define RING_BUFFER_ERROR_BUFFER_IS_FULL      2
#define RING_BUFFER_ERROR_BUFFER_IS_EMPTY     3

// Ring buffer structure
    typedef struct ring_buffer {
        uint8_t* buffer;
        size_t size;  // total size of buffer
        volatile size_t head;  // write position
        volatile size_t tail;  // read position
    } ring_buffer_t;

    /**
     * @brief Initialize the ring buffer
     * @param rb Pointer to ring buffer struct
     * @param buffer Pre-allocated byte buffer
     * @param size Size of the buffer (must be ≥ 2)
     * @return RING_BUFFER_SUCCESS or RING_BUFFER_ERROR_INVALID_PARAMETERS
     */
    uint8_t ring_buffer_init(ring_buffer_t* rb, uint8_t* buffer, size_t size);

    /**
     * @brief Returns total usable capacity (size - 1)
     */
    size_t ring_buffer_capacity(ring_buffer_t* rb);

    /**
     * @brief Returns number of bytes currently stored
     */
    size_t ring_buffer_used_space(ring_buffer_t* rb);

    /**
     * @brief Returns number of free bytes (excluding 1 reserved byte)
     */
    size_t ring_buffer_free_space(ring_buffer_t* rb);

    /**
     * @brief Returns number of contiguous used bytes from tail (before wrap)
     */
    size_t ring_buffer_linear_used_space(ring_buffer_t* rb);

    /**
     * @brief Returns number of contiguous free bytes from head (before wrap)
     */
    size_t ring_buffer_linear_free_space(ring_buffer_t* rb);

    /**
     * @brief Enqueue a byte into the buffer
     */
    uint8_t ring_buffer_enqueue(ring_buffer_t* rb, uint8_t byte);

    /**
     * @brief Dequeue a byte from the buffer
     */
    uint8_t ring_buffer_dequeue(ring_buffer_t* rb, uint8_t* out_byte);

    /**
     * @brief Peek the next byte without removing it
     */
    uint8_t ring_buffer_peek(ring_buffer_t* rb, uint8_t* out_byte);

    /**
     * @brief Enqueue a byte array into the buffer
     * @return number of bytes actually written
     */
    size_t ring_buffer_enqueue_arr(ring_buffer_t* rb, uint8_t* byte_array, size_t len);

    /**
     * @brief Dequeue a byte array from the buffer
     * @return number of bytes actually read
     */
    size_t ring_buffer_dequeue_arr(ring_buffer_t* rb, uint8_t* out_array, size_t len);

    /**
     * @brief Peek bytes from the buffer without advancing the tail
     * @return number of bytes peeked
     */
    size_t ring_buffer_peek_arr(ring_buffer_t* rb, uint8_t* out_array, size_t len);

    /**
 * @brief Peek bytes from the buffer without advancing the tail
 * @return number of bytes peeked at an offset from the tail
 */
    size_t ring_buffer_peek_arr_offset(ring_buffer_t* rb, uint8_t* out_array, size_t len, size_t offset);

    /**
     * @brief Get pointer to the current write position
     */
    uint8_t* ring_buffer_write_ptr(ring_buffer_t* rb);

    /**
     * @brief Get pointer to the current read position
     */
    uint8_t* ring_buffer_read_ptr(ring_buffer_t* rb);

    /**
     * @brief Advance the tail by len bytes (only if len ≤ used)
     */
    void ring_buffer_advance_tail(ring_buffer_t* rb, size_t len);

    /**
     * @brief Advance the head by len bytes (only if len ≤ free)
     */
    void ring_buffer_advance_head(ring_buffer_t* rb, size_t len);

    /**
     * @brief Check if buffer is empty
     */
    uint8_t ring_buffer_is_empty(ring_buffer_t* rb);

    /**
     * @brief Check if buffer is full
     */
    uint8_t ring_buffer_is_full(ring_buffer_t* rb);


    /**
	 * @brief clears the ring buffer
     */
    void ring_buffer_clear(ring_buffer_t* rb);

#ifdef __cplusplus
}
#endif

#endif // __RINGBUFFER_H__
