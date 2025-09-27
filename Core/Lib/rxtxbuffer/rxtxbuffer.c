#include "rxtxbuffer.h"
#include <memory.h>


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

// Utility function to initialize the rxtxbuffer_t
uint8_t rxtxbuffer_init(rxtxbuffer_t* _Cvector, uint8_t* buf, size_t _capacity) {
	if (_Cvector == NULL) {
		return RXTXBUFFER_ERROR_INVALID_PARAMETERS;
	}

	if (buf == NULL && _capacity != (size_t)0) {
		return RXTXBUFFER_ERROR_INVALID_PARAMETERS;
	}


	_Cvector->buffer = buf;
	_Cvector->data_size = (size_t)0;
	_Cvector->sent_size = (size_t)0;
	_Cvector->capacity = _capacity;

	return RXTXBUFFER_SUCCESS;
}

uint8_t* rxtxbuffer_buffer_ptr(rxtxbuffer_t* pt) {
	return pt->buffer;
}

size_t rxtxbuffer_sent_size(rxtxbuffer_t* pt) {
	return pt->sent_size;
}

size_t rxtxbuffer_data_size(rxtxbuffer_t* pt) {
	return pt->data_size;
}

size_t rxtxbuffer_capacity(rxtxbuffer_t* pt) {
	return pt->capacity;
}

size_t rxtxbuffer_free_space(rxtxbuffer_t* sendbuf) {
	return sendbuf->capacity - sendbuf->data_size;
}

size_t rxtxbuffer_data_remaining(rxtxbuffer_t* sendbuf) {
	return sendbuf->data_size - sendbuf->sent_size;
}

void rxtxbuffer_sent_data_increase_size(rxtxbuffer_t* pt, size_t sent_data_size) {
	pt->sent_size = MIN(MIN(pt->data_size, pt->sent_size + sent_data_size), pt->capacity);
}

void rxtxbuffer_data_increase_size(rxtxbuffer_t* pt, size_t recved_data_size) {
	pt->data_size = MIN(pt->data_size + recved_data_size, pt->capacity);
}

void rxtxbuffer_data_decrease_size(rxtxbuffer_t* pt, size_t decreased_size) {
	if (decreased_size >= pt->data_size) {
		pt->data_size = (size_t)0;
		pt->sent_size = (size_t)0;
	}
	else
	{
		pt->data_size -= decreased_size;
		if (pt->sent_size > pt->data_size) {
			pt->sent_size = pt->data_size; // Ensure sent size does not exceed data size
		}
	}
}

void rxtxbuffer_sent_data_decrease_size(rxtxbuffer_t* pt, size_t decreased_size) {
	if (decreased_size >= pt->data_size) {
		pt->sent_size = (size_t)0;
	}
	else {
		pt->sent_size -= decreased_size;
	}
}

void rxtxbuffer_clear(rxtxbuffer_t* pt) {
	pt->data_size = (size_t)0;
	pt->sent_size = (size_t)0;
}

void rxtxbuffer_reset_sent_size(rxtxbuffer_t* pt) {
	pt->sent_size = (size_t)0;
}

size_t rxtxbuffer_push_arr(rxtxbuffer_t* pt, uint8_t* data, size_t data_size) {
	size_t chunk_size = MIN(rxtxbuffer_free_space(pt), data_size);
	memcpy(rxtxbuffer_free_space_ptr(pt), data, chunk_size);
	rxtxbuffer_data_increase_size(pt, chunk_size);
	return chunk_size;
}

uint8_t* rxtxbuffer_data_ptr(rxtxbuffer_t* pt) {
	return ((uint8_t*)(pt->buffer)) + pt->sent_size;
}

uint8_t* rxtxbuffer_free_space_ptr(rxtxbuffer_t* pt) {
	return ((uint8_t*)(pt->buffer)) + pt->data_size;
}

void rxtxbuffer_shift_data_buf(rxtxbuffer_t* pt) {
	memmove(rxtxbuffer_buffer_ptr(pt), rxtxbuffer_data_ptr(pt), rxtxbuffer_data_remaining(pt));
	pt->data_size = rxtxbuffer_data_remaining(pt);
	pt->sent_size = (size_t)0;
}
