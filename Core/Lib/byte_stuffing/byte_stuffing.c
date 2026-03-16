
#include "byte_stuffing.h"

int escape_byte_(
		uint8_t byte,
		uint8_t* out_buf,
		size_t out_len_remaining,
		uint8_t escaped_byte,
		uint8_t escape_byte,
		uint8_t escape_mask
		) {
    if (byte == escaped_byte || byte == escape_byte) {
    	if(out_len_remaining < 2){
    		return 0;
    	}
        out_buf[0] = escape_byte;
        out_buf[1] = byte ^ escape_mask;
        return 2;
    }
    else if (out_len_remaining > 0) {
        out_buf[0] = byte;
        return 1;
    }
    return 0;
}

size_t escape_buffer_(
		const uint8_t* in_buf,
		size_t in_len,
		uint8_t* out_buf,
		size_t max_out_len,
		uint8_t escaped_byte,
		uint8_t escape_byte,
		uint8_t escape_mask
		){
	size_t outbuf_len = 0;
	size_t outbuf_remaining_size = 0;
	int result_escaped_operation = 0;

	if(in_len > max_out_len){
		return max_out_len + 1;
	}

	for(size_t i = 0; i < in_len; i++){
		if(outbuf_len >= max_out_len){
			return max_out_len + 1;
		}

		outbuf_remaining_size = max_out_len - outbuf_len;
		result_escaped_operation = escape_byte_(in_buf[i], &(out_buf[outbuf_len]), outbuf_remaining_size, escaped_byte, escape_byte, escape_mask);
		outbuf_len += result_escaped_operation;

		if(result_escaped_operation == 0){
			return max_out_len + 1;
		}
	}
	return outbuf_len;
}


size_t unescape_buffer_(
		const uint8_t* in_buf,
		size_t in_len,
		uint8_t* out_buf,
		size_t max_out_len,
		uint8_t escaped_byte,
		uint8_t escape_byte,
		uint8_t escape_mask
		) {
    size_t out_len = 0;
    int8_t escaping = 0;

    for (size_t i = 0; i < in_len; i++) {
        uint8_t byte = in_buf[i];

        if (escaping) {
            byte ^= escape_mask;
            escaping = 0;
        }
        else if (byte == escape_byte) {
            escaping = 1;
            continue;
        }

        if (out_len >= max_out_len) return max_out_len + 1;

        out_buf[out_len] = byte;
        out_len++;
    }

    if (escaping) return 0;

    return out_len;
}
