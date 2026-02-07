#include "msp.h"
#include "stdint.h"
#include "crc.h"

#define CLAMP(x, lo, hi) (((x) < (lo)) ? (lo) : ((x) > (hi)) ? (hi) : (x))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MSP_V1_CRC8_START_CRC ((uint8_t) 0)

// Only on incoming raw data this is 100% correct on both little-endian and big-endian systems.
static inline uint16_t le16_to_host(const uint8_t *p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static msp_packet_state_t msp_receive_frame(msp_t *msp, uint8_t rxByte){
	uint32_t currentTime = msp->sys_now_ms();
	uint8_t crc;
	msp_direction_t direction;

	if (msp->rx_packet_state == MSP_PACKET_RECEIVED || msp->rx_frame_position >= MSP_RX_BUF_SIZE){
		msp->rx_packet_state = MSP_IDLE;
	}

	if(msp->rx_packet_state == MSP_IDLE){
		msp->rx_packet.valid = 0;
		msp->rx_packet.msp_version = MSP_VERSION_INVALID;
		msp->rx_frame_full_len = 0;
		msp->rx_frame_position = 0;
	}

	/* Reset the frame position if the frame time has expired. */
	if (currentTime - msp->rx_frame_start_time_ms > msp->_timeout_ms) {
		msp->rx_frame_position = 0;
		msp->rx_packet_state = MSP_IDLE;

		if (currentTime < msp->rx_frame_start_time_ms) {
			msp->rx_frame_start_time_ms = currentTime;
		}
	}

	msp->rx_frame_buffer[msp->rx_frame_position] = rxByte;
	msp->rx_frame_position++;

	if (msp->rx_frame_position == MSP_PREAMBLE_1_POSITION + MSP_PREAMBLE_1_SIZE) {
		msp->rx_frame_start_time_ms = currentTime;

		if(rxByte == MSP_PREAMBLE_1){
			msp->rx_packet_state = MSP_HEADER_START;
		}
		else{
			// invalid frame preamble 1, reset the frame
			msp->rx_packet_state = MSP_IDLE;
		}
	}

	// processing the header (first 3 bytes)
	else if(msp->rx_frame_position == MSP_HEADER_SIZE) {
		msp->rx_packet_state = MSP_HEADER_END;

		// processing the frame direction
		if(msp->rx_frame_buffer[MSP_DIRECTION_POSITION] == MSP_DIRECTION_REQUEST_CHAR){
			direction = MSP_DIRECTION_REQUEST;
		}
		else if(msp->rx_frame_buffer[MSP_DIRECTION_POSITION] == MSP_DIRECTION_RESPONSE_CHAR){
			direction = MSP_DIRECTION_REPLY;
		}
		else if(msp->rx_frame_buffer[MSP_DIRECTION_POSITION] == MSP_DIRECTION_ERROR_CHAR){
			direction = MSP_DIRECTION_ERROR;
		}
		else{
			// invalid frame direction, reset the frame
			msp->rx_packet_state = MSP_IDLE;
		}

		// processing the frame version
		if(msp->rx_packet_state == MSP_HEADER_END){
			if(msp->rx_frame_buffer[MSP_PREAMBLE_2_POSITION] == MSP_PREAMBLE_2_V1){
				msp->rx_packet.msp_version = MSP_V1;
				msp->rx_packet.v1.direction = direction;
			}
			else if(msp->rx_frame_buffer[MSP_PREAMBLE_2_POSITION] == MSP_PREAMBLE_2_V2){
				msp->rx_packet.msp_version = MSP_V2_NATIVE;
				msp->rx_packet.v2.direction = direction;
			}
			else{
				// invalid frame version, reset the frame
				msp->rx_packet_state = MSP_IDLE;
			}
		}
	}
	// check if the V1 frame is a Jumbo frame, else get the payload size of the V1 frame
	else if(msp->rx_frame_position == MSP_V1_PAYLOAD_SIZE_POSITION + MSP_V1_PAYLOAD_SIZE_SIZE && msp->rx_packet.msp_version == MSP_V1){
		if(msp->rx_frame_buffer[MSP_V1_PAYLOAD_SIZE_POSITION] == 255){
			// It is a JUMBO frame
			msp->rx_packet.msp_version = MSP_JUMBO;
			msp->rx_packet.jumbo.direction = msp->rx_packet.v1.direction;
		}
		else{
			// frame is version 1 (V1), lets calculate the rx_frame_full_len
			msp->rx_packet.v1.payload_size = msp->rx_frame_buffer[MSP_V1_PAYLOAD_SIZE_POSITION];
			msp->rx_packet_state = MSP_PAYLOAD_SIZE_END;
			msp->rx_frame_full_len = MSP_V1_NON_PAYLOAD_SIZE + msp->rx_packet.v1.payload_size;
		}
	}

	// At this point it is clear the version of the frame
	// Calculate the rx_frame_full_len of the JUMBO frame
	else if(msp->rx_frame_position == MSP_JUMBO_PAYLOAD_SIZE_POSITION + MSP_JUMBO_PAYLOAD_SIZE_SIZE && msp->rx_packet.msp_version == MSP_JUMBO){
		msp->rx_packet.jumbo.payload_size = le16_to_host(&(msp->rx_frame_buffer[MSP_JUMBO_PAYLOAD_SIZE_POSITION]));
		msp->rx_packet_state = MSP_PAYLOAD_SIZE_END;
		msp->rx_frame_full_len = MSP_JUMBO_NON_PAYLOAD_SIZE + msp->rx_packet.jumbo.payload_size;
	}

	// Calculate the rx_frame_full_len of the V2 frame
	else if(msp->rx_frame_position == MSP_V2_PAYLOAD_SIZE_POSITION + MSP_V2_PAYLOAD_SIZE_SIZE && msp->rx_packet.msp_version == MSP_V2_NATIVE){
		msp->rx_packet.v2.payload_size = le16_to_host(&(msp->rx_frame_buffer[MSP_V2_PAYLOAD_SIZE_POSITION]));
		msp->rx_packet_state = MSP_PAYLOAD_SIZE_END;
		msp->rx_frame_full_len = MSP_V2_NON_PAYLOAD_SIZE + msp->rx_packet.v2.payload_size;
	}

	// the entire frame was received and now CRC and other will be validated
	else if(msp->rx_frame_position >= msp->rx_frame_full_len && msp->rx_packet_state == MSP_PAYLOAD_SIZE_END){
		if(msp->rx_packet.msp_version == MSP_V1){

			msp->rx_packet.v1.command_id = msp->rx_frame_buffer[MSP_V1_COMMAND_ID_POSITION];
			msp->rx_packet.v1.payload = &(msp->rx_frame_buffer[MSP_V1_PAYLOAD_POSITION]);
			msp->rx_packet.v1.checksum = msp->rx_frame_buffer[MSP_V1_PAYLOAD_POSITION + msp->rx_packet.v1.payload_size];

			crc = MSP_V1_CRC8_START_CRC;
			crc = crc8_xor_add_arr(crc, &(msp->rx_frame_buffer[MSP_V1_CRC_START_POSITION]), msp->rx_packet.v1.payload_size + MSP_V1_NON_PAYLOAD_CRC_SIZE);

			if (msp->rx_packet.v1.checksum == crc){
				msp->rx_packet_state = MSP_PACKET_RECEIVED;
			}
			else{
				msp->rx_packet_state = MSP_IDLE;
			}
		}
		else if(msp->rx_packet.msp_version == MSP_JUMBO){
			msp->rx_packet.jumbo.command_id = msp->rx_frame_buffer[MSP_JUMBO_COMMAND_ID_POSITION];
			msp->rx_packet.jumbo.payload = &(msp->rx_frame_buffer[MSP_JUMBO_PAYLOAD_POSITION]);
			msp->rx_packet.jumbo.checksum = msp->rx_frame_buffer[MSP_JUMBO_PAYLOAD_POSITION + msp->rx_packet.jumbo.payload_size];

			crc = MSP_V1_CRC8_START_CRC;
			crc = crc8_xor_add_arr(crc, &(msp->rx_frame_buffer[MSP_JUMBO_CRC_START_POSITION]), msp->rx_packet.jumbo.payload_size + MSP_JUMBO_NON_PAYLOAD_CRC_SIZE);

			if (msp->rx_packet.jumbo.checksum == crc){
				msp->rx_packet_state = MSP_PACKET_RECEIVED;
			}
			else{
				msp->rx_packet_state = MSP_IDLE;
			}
		}
		else if(msp->rx_packet.msp_version == MSP_V2_NATIVE){
			msp->rx_packet.v2.flag = msp->rx_frame_buffer[MSP_V2_FLAG_POSITION];
			msp->rx_packet.v2.command_id = le16_to_host(&(msp->rx_frame_buffer[MSP_V2_COMMAND_ID_POSITION]));
			msp->rx_packet.v2.payload = &(msp->rx_frame_buffer[MSP_V2_PAYLOAD_POSITION]);
			msp->rx_packet.v2.checksum = msp->rx_frame_buffer[MSP_V2_PAYLOAD_POSITION + msp->rx_packet.v2.payload_size];

			crc = crc8_dvb_s2_init();
			crc = crc8_dvb_s2_add_arr(crc, &(msp->rx_frame_buffer[MSP_V2_CRC_START_POSITION]), msp->rx_packet.v2.payload_size + MSP_V2_NON_PAYLOAD_CRC_SIZE);

			if (msp->rx_packet.v2.checksum == crc){
				msp->rx_packet_state = MSP_PACKET_RECEIVED;
			}
			else{
				msp->rx_packet_state = MSP_IDLE;
			}
		}
	}


	if (msp->rx_packet_state == MSP_PACKET_RECEIVED){
		msp->rx_packet.valid = 1;
	}
	else {
		if((msp->rx_frame_position >= MSP_V2_PAYLOAD_POSITION) && (msp->rx_packet_state < MSP_PAYLOAD_SIZE_END)){
			msp->rx_packet_state = MSP_IDLE;
		}
		if(msp->rx_packet_state == MSP_IDLE){
			msp->rx_frame_full_len = 0;
			msp->rx_frame_position = 0;
		}
		msp->rx_packet.valid = 0;
	}

	return msp->rx_packet_state;
}



void msp_update(msp_t *msp, uint8_t* rx_buf, uint32_t rx_buf_size){
	msp_packet_state_t msp_result = MSP_IDLE;
	for(uint32_t i=0;i < rx_buf_size; i++){
		msp_result = msp_receive_frame(msp, rx_buf[i]);

		if(msp_result == MSP_PACKET_RECEIVED){ // new packet was received process the packet
//			#error  "implement"
		}
	}
}
