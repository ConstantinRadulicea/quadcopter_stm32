#include "ringbuffer.h"
#include <memory.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))


uint8_t ring_buffer_init(ring_buffer_t* rb, uint8_t* buffer, size_t size) {
	if (rb == NULL || buffer == NULL || size < (size_t)2) {
		// Must have valid pointers and buffer size ≥ 2
		return RING_BUFFER_ERROR_INVALID_PARAMETERS;
	}

	rb->buffer = buffer;
	rb->size = size;
	rb->head = (size_t)0;
	rb->tail = (size_t)0;

	return RING_BUFFER_SUCCESS;
}



//size_t ring_buffer_free_space(ring_buffer_t* rb) {
//	if (rb == NULL) {
//		return RING_BUFFER_ERROR_INVALID_PARAMETERS; // Invalid ring buffer
//	}
//
//	if (rb->head >= rb->tail) {
//		// |-----used-----|        ← head ahead of tail
//		// [tail.....head)
//		return rb->size - (rb->head - rb->tail) - (size_t)1; // Free space when head is ahead of tail
//	}
//	else {
//		// |--used--| |--free--|   ← tail ahead of head
//		// [head......tail)
//		return rb->tail - rb->head - (size_t)1; // Free space when tail is ahead of head
//	}
//}


size_t ring_buffer_free_space(ring_buffer_t* rb) {
	//if (rb == NULL) return (size_t)0;
	return (rb->tail + rb->size - rb->head - (size_t)1) % rb->size;
}

uint8_t ring_buffer_is_empty(ring_buffer_t* rb) {
	//if (rb == NULL) return (size_t)0;
	return (uint8_t)(rb->head == rb->tail);
}

uint8_t ring_buffer_is_full(ring_buffer_t* rb) {
	//if (rb == NULL) return (size_t)0;
	return (uint8_t) (((rb->head + (size_t)1) % rb->size) == rb->tail);
}

size_t ring_buffer_used_space(ring_buffer_t* rb) {
	//if (rb == NULL) return (size_t)0;
	return (rb->head + rb->size - rb->tail) % rb->size;
}

uint8_t ring_buffer_enqueue(ring_buffer_t* rb, uint8_t byte) {
	if (rb == NULL) {
		return RING_BUFFER_ERROR_INVALID_PARAMETERS; // Invalid buffer
	}

	size_t next_head = (rb->head + (size_t)1) % rb->size;

	if (next_head == rb->tail) {
		return RING_BUFFER_ERROR_BUFFER_IS_FULL; // Buffer full (would overwrite tail)
	}

	rb->buffer[rb->head] = byte;
	rb->head = next_head;
	return RING_BUFFER_SUCCESS;
}


uint8_t ring_buffer_dequeue(ring_buffer_t* rb, uint8_t* out_byte) {
	if (rb == NULL || rb->buffer == NULL || rb->size == (size_t)0 || out_byte == NULL) {
		return RING_BUFFER_ERROR_INVALID_PARAMETERS;
	}

	if (rb->head == rb->tail) {
		return RING_BUFFER_ERROR_BUFFER_IS_EMPTY; // Nothing to read
	}

	*out_byte = rb->buffer[rb->tail];
	rb->tail = (rb->tail + (size_t)1) % rb->size;

	return RING_BUFFER_SUCCESS;
}


/*
 buffer size = 8, tail = 2, head = 5
 [ _ _ T _ _ H _ _ ]
 returns: 8 - 5 = 3 bytes available before wrap

 buffer size = 8, tail = 7, head = 2
 [ _ _ H _ _ _ _ T ]
 returns: 7 - 2 - 1 = 4 bytes available before reaching tail
*/
size_t ring_buffer_linear_free_space(ring_buffer_t* rb) {
	//if (rb == NULL) return (size_t)0;

	if (rb->head >= rb->tail) {
		if (rb->tail == (size_t)0) {
			// Only leave one byte free to distinguish full/empty
			return rb->size - rb->head - (size_t)1;
		}
		else {
			return rb->size - rb->head;
		}
	}
	else {
		// Tail is before head, space is up to tail - 1
		return rb->tail - rb->head - (size_t)1;
	}
}



size_t ring_buffer_linear_used_space(ring_buffer_t* rb) {
	//if (rb == NULL) return (size_t)0;

	if (rb->head >= rb->tail) {
		// All used data is linear
		return rb->head - rb->tail;
	}
	else {
		// Used data wraps around, so linear part is from tail to end
		return rb->size - rb->tail;
	}
}



uint8_t* ring_buffer_write_ptr(ring_buffer_t* rb) {
//	if (rb == NULL || rb->buffer == NULL || rb->size == (size_t)0) {
//		return NULL;
//	}

	return &rb->buffer[rb->head];
}


uint8_t* ring_buffer_read_ptr(ring_buffer_t* rb) {
//	if (rb == NULL || rb->buffer == NULL || rb->size == (size_t)0) {
//		return NULL;
//	}

	return &rb->buffer[rb->tail];
}

size_t ring_buffer_enqueue_arr(ring_buffer_t* rb, uint8_t* byte_array, size_t len) {
//	if (rb == NULL || rb->buffer == NULL || byte_array == NULL || rb->size == (size_t)0) {
//		return (size_t)0;
//	}

	size_t free_space = ring_buffer_free_space(rb);
	size_t to_write = MIN(len, free_space);

	size_t linear_part = MIN(ring_buffer_linear_free_space(rb), to_write);
	memcpy(&rb->buffer[rb->head], byte_array, linear_part);
	rb->head = (rb->head + linear_part) % rb->size;

	size_t wrapped_part = to_write - linear_part;
	if (wrapped_part > (size_t)0) {
		memcpy(&rb->buffer[rb->head], &byte_array[linear_part], wrapped_part);
		rb->head = (rb->head + wrapped_part) % rb->size;
	}

	return to_write;
}


size_t ring_buffer_dequeue_arr(ring_buffer_t* rb, uint8_t* out_array, size_t len) {
//	if (rb == NULL || rb->buffer == NULL || rb->size == (size_t)0 || out_array == NULL) {
//		return (size_t)0;
//	}

	size_t used = ring_buffer_used_space(rb);
	if (used == (size_t)0) {
		return (size_t)0; // Nothing to read
	}

	size_t to_read = MIN(len, used);

	// How many bytes we can read linearly (without wrapping)
	size_t linear_data = ring_buffer_linear_used_space(rb);

	
	size_t first_chunk = MIN(to_read, linear_data);

	memcpy(out_array, &rb->buffer[rb->tail], first_chunk);
	rb->tail = (rb->tail + first_chunk) % rb->size;

	if (to_read > first_chunk) {
		size_t second_chunk = to_read - first_chunk;
		memcpy(&out_array[first_chunk], &rb->buffer[rb->tail], second_chunk);
		rb->tail = (rb->tail + second_chunk) % rb->size;
	}

	return (size_t)to_read;
}

size_t ring_buffer_capacity(ring_buffer_t* rb) {
	return rb ? rb->size - (size_t)1 : (size_t)0;
}


uint8_t ring_buffer_peek(ring_buffer_t* rb, uint8_t* out_byte) {
	if (rb == NULL || rb->buffer == NULL || rb->size == (size_t)0 || out_byte == NULL) {
		return RING_BUFFER_ERROR_INVALID_PARAMETERS;
	}

	if (rb->head == rb->tail) {
		return RING_BUFFER_ERROR_BUFFER_IS_EMPTY;
	}

	*out_byte = rb->buffer[rb->tail];
	return RING_BUFFER_SUCCESS;
}

size_t ring_buffer_peek_arr(ring_buffer_t* rb, uint8_t* out_array, size_t len) {
//	if (rb == NULL || rb->buffer == NULL || rb->size == (size_t)0 || out_array == NULL) {
//		return (size_t)0;
//	}

	size_t used = ring_buffer_used_space(rb);
	if (used == (size_t)0) {
		return (size_t)0; // Nothing to read
	}

	size_t to_peek = MIN(len, used);

	// How many bytes we can read linearly from tail to end
	size_t linear_data = ring_buffer_linear_used_space(rb);
	size_t first_chunk = MIN(to_peek, linear_data);

	memcpy(out_array, &rb->buffer[rb->tail], first_chunk);

	if (to_peek > first_chunk) {
		// Wraparound peek from the beginning
		size_t second_chunk = to_peek - first_chunk;
		memcpy(&out_array[first_chunk], &rb->buffer[0], second_chunk);
	}

	return to_peek;
}


size_t ring_buffer_peek_arr_offset(ring_buffer_t* rb, uint8_t* out_array, size_t len, size_t offset) {
//	if (rb == NULL || rb->buffer == NULL || rb->size == (size_t)0 || out_array == NULL) {
//		return (size_t)0;
//	}

	size_t temp_offset = offset; // Preserve original offset
	size_t used = ring_buffer_used_space(rb);

	if (offset >= used) {
		return (size_t)0; // Invalid offset
	}

	if (used == (size_t)0) {
		return (size_t)0; // Nothing to read
	}

	size_t to_peek = MIN(len + offset, used);

	// How many bytes we can read linearly from tail to end
	size_t linear_data = ring_buffer_linear_used_space(rb);
	size_t first_chunk = MIN(to_peek, linear_data);

	if (first_chunk > offset) {
		memcpy(out_array, &rb->buffer[rb->tail + offset], first_chunk - offset);
		temp_offset = (size_t)0; // Reset offset after first chunk
	}
	else {
		temp_offset -= first_chunk; // Adjust offset for next chunk
	}

	if (to_peek > first_chunk) {
		// Wraparound peek from the beginning
		size_t second_chunk = to_peek - first_chunk;

		memcpy(&out_array[first_chunk - offset], &rb->buffer[(size_t)0 + temp_offset], second_chunk - temp_offset);
		temp_offset = (size_t)0; // Reset offset after first chunk
	}

	return to_peek - offset;
}

void ring_buffer_advance_tail(ring_buffer_t* rb, size_t len) {
//	if (rb == NULL) {
//		return;
//	}
	size_t used = ring_buffer_used_space(rb);
	size_t advance_len = MIN(len, used);
	rb->tail = (rb->tail + advance_len) % rb->size;
}

void ring_buffer_advance_head(ring_buffer_t* rb, size_t len) {
//	if (rb == NULL) {
//		return;
//	}
	size_t free_space = ring_buffer_free_space(rb);
	size_t advance_len = MIN(len, free_space);
	rb->head = (rb->head + advance_len) % rb->size;
}

void ring_buffer_clear(ring_buffer_t* rb) {
//	if (rb == NULL) {
//		return;
//	}
	rb->head = (size_t)0;
	rb->tail = (size_t)0;
}



