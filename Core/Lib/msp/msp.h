#ifndef MSP_H
#define MSP_H

// https://github.com/betaflight/betaflight/blob/master/src/main/msp/msp.h


/*

MSP frame header: First 3 bytes

MSP v1 frame
Byte Index		Field			Size			Description
0				Preamble 1		1 Byte			Always ASCII $ (0x24).
1				Preamble 2		1 Byte			Always ASCII M (0x4D).
2				Direction		1 Byte			"< (Request), > (Response), or ! (Error)."
3				Size			1 Byte			Length of the payload only (0 to 255).
4				Type (ID)		1 Byte			"The Command ID (e.g., 101 for Attitude)."
5 to (5+N-1)	Payload			N Bytes			The actual data (omitted if Size is 0).
5 + N			Checksum		1 Byte			"XOR sum of Size, Type, and all Payload bytes."



MSP V1 Jumbo Frame
Byte Index		Field			Size		Description
0				Preamble 1		1 Byte		Always ASCII $ (0x24).
1				Preamble 2		1 Byte		Always ASCII M (0x4D).
2				Direction		1 Byte		"< (Request), > (Response), or ! (Error)."
3				Size Flag		1 Byte		"Always 0xFF (255). This tells the parser: ""I am a Jumbo frame."""
4				Type (ID)		1 Byte		The Command ID (0 to 255).
5 - 6			Real Size		2 Bytes		"(Little Endian) The actual payload length (0 to 65,535)."
7 to (7+N-1)	Payload			N Bytes		The actual data.
7 + N			Checksum		1 Byte		XOR sum of all bytes from Byte 3 up to the last payload byte.



MSP v2 frame
Byte Index		Field			Size		Description
0				Preamble 1		1 Byte		Always ASCII $ (0x24).
1				Preamble 2		1 Byte		Always ASCII X (0x58) for V2.
2				Direction		1 Byte		"< (Request), > (Response), or ! (Error)."
3				Flag			1 Byte		"Usually 0x00. Used for ""incompatibility"" or special flags."
4 - 5			Function ID		2 Bytes		"(Little Endian) The Command ID (0 - 65,535)."
6 - 7			Payload Size	2 Bytes		"(Little Endian) Length of the data section (0 - 65,535)."
8 to (8+N-1)	Payload			N Bytes		The actual data being transmitted.
8 + N			Checksum		1 Byte		CRC8-DVB-S2 hash of everything from Byte 3 to the end of the payload.

*/



#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MSP_RX_BUF_SIZE 1024
#define MSP_TX_BUF_SIZE 512 // As of 2021/08/10 MSP_BOXNAMES generates a 307 byte response for page 1. There has been overflow issues with 320 byte buffer.

#define MSP_PREAMBLE_1 ((uint8_t)(0x24))
#define MSP_PREAMBLE_2_V1 ((uint8_t)(0x4D))
#define MSP_PREAMBLE_2_V2 ((uint8_t)(0x58))

#define MSP_DIRECTION_REQUEST_CHAR ((uint8_t)(0x3C))
#define MSP_DIRECTION_RESPONSE_CHAR ((uint8_t)(0x3E))
#define MSP_DIRECTION_ERROR_CHAR ((uint8_t)(0x21))

typedef enum{
	MSP_PREAMBLE_1_SIZE = 1,
	MSP_PREAMBLE_2_SIZE = 1,
	MSP_DIRECTION_SIZE = 1,
	MSP_V1_PAYLOAD_SIZE_SIZE = 1,
	MSP_JUMBO_PAYLOAD_SIZE_SIZE = 2,
	MSP_V2_PAYLOAD_SIZE_SIZE = 2,
	MSP_V2_FLAG_SIZE = 1,
	MSP_V1_COMMAND_ID_SIZE = 1,
	MSP_V2_COMMAND_ID_SIZE = 2,
	MSP_CHECKSUM_SIZE = 1
}msp_field_size_t;


typedef enum{
	MSP_HEADER_SIZE = MSP_PREAMBLE_1_SIZE + MSP_PREAMBLE_2_SIZE + MSP_DIRECTION_SIZE,

	MSP_V1_NON_PAYLOAD_SIZE = MSP_HEADER_SIZE + MSP_V1_PAYLOAD_SIZE_SIZE + MSP_V1_COMMAND_ID_SIZE + MSP_CHECKSUM_SIZE,
	MSP_JUMBO_NON_PAYLOAD_SIZE = MSP_HEADER_SIZE + MSP_V1_PAYLOAD_SIZE_SIZE + MSP_V1_COMMAND_ID_SIZE + MSP_JUMBO_PAYLOAD_SIZE_SIZE + MSP_CHECKSUM_SIZE,
	MSP_V2_NON_PAYLOAD_SIZE = MSP_HEADER_SIZE + MSP_V2_FLAG_SIZE + MSP_V2_COMMAND_ID_SIZE + MSP_V2_PAYLOAD_SIZE_SIZE + MSP_CHECKSUM_SIZE,

	MSP_V1_NON_PAYLOAD_CRC_SIZE = MSP_V1_PAYLOAD_SIZE_SIZE + MSP_V1_COMMAND_ID_SIZE,
	MSP_JUMBO_NON_PAYLOAD_CRC_SIZE = MSP_V1_PAYLOAD_SIZE_SIZE + MSP_V1_COMMAND_ID_SIZE + MSP_JUMBO_PAYLOAD_SIZE_SIZE,
	MSP_V2_NON_PAYLOAD_CRC_SIZE = MSP_V2_FLAG_SIZE + MSP_V2_COMMAND_ID_SIZE + MSP_V2_PAYLOAD_SIZE_SIZE
}msp_frame_size_t;

typedef enum{
	MSP_PREAMBLE_1_POSITION = 0,
	MSP_PREAMBLE_2_POSITION = MSP_PREAMBLE_1_SIZE,
	MSP_DIRECTION_POSITION = MSP_PREAMBLE_2_POSITION + MSP_PREAMBLE_2_SIZE,

	MSP_V1_PAYLOAD_SIZE_POSITION = MSP_HEADER_SIZE,
	MSP_JUMBO_PAYLOAD_SIZE_POSITION = MSP_HEADER_SIZE + MSP_V1_PAYLOAD_SIZE_SIZE + MSP_V1_COMMAND_ID_SIZE,
	MSP_V2_PAYLOAD_SIZE_POSITION = MSP_HEADER_SIZE + MSP_V2_FLAG_SIZE + MSP_V2_COMMAND_ID_SIZE,

	MSP_V1_PAYLOAD_POSITION = MSP_HEADER_SIZE + MSP_V1_PAYLOAD_SIZE_SIZE + MSP_V1_COMMAND_ID_SIZE,
	MSP_JUMBO_PAYLOAD_POSITION = MSP_HEADER_SIZE + MSP_V1_PAYLOAD_SIZE_SIZE + MSP_V1_COMMAND_ID_SIZE + MSP_JUMBO_PAYLOAD_SIZE_SIZE,
	MSP_V2_PAYLOAD_POSITION = MSP_HEADER_SIZE + MSP_V2_FLAG_SIZE + MSP_V2_COMMAND_ID_SIZE + MSP_V2_PAYLOAD_SIZE_SIZE,

	MSP_V1_COMMAND_ID_POSITION = MSP_HEADER_SIZE + MSP_V1_PAYLOAD_SIZE_SIZE,
	MSP_JUMBO_COMMAND_ID_POSITION = MSP_HEADER_SIZE + MSP_V1_PAYLOAD_SIZE_SIZE,
	MSP_V2_COMMAND_ID_POSITION = MSP_HEADER_SIZE + MSP_V2_FLAG_SIZE,

	MSP_V1_CRC_START_POSITION = MSP_HEADER_SIZE,
	MSP_JUMBO_CRC_START_POSITION = MSP_HEADER_SIZE,
	MSP_V2_CRC_START_POSITION = MSP_HEADER_SIZE,

	MSP_V2_FLAG_POSITION = MSP_HEADER_SIZE
}msp_frame_field_position_t;


typedef enum {
    MSP_IDLE,
    MSP_HEADER_START,
	MSP_HEADER_END,
	MSP_PAYLOAD_SIZE_END,
    MSP_PACKET_RECEIVED
} msp_packet_state_t;

typedef enum {
	MSP_VERSION_INVALID = 0,
    MSP_V1     		= 1,
    MSP_JUMBO		= 2,
    MSP_V2_NATIVE   = 3
} msp_version_t;

typedef enum msp_direction_e{
    MSP_DIRECTION_REPLY = 0,
    MSP_DIRECTION_REQUEST = 1,
	MSP_DIRECTION_ERROR = 2
} msp_direction_t;

typedef struct msp_v1_packet_s {
	msp_direction_t direction;
	uint8_t command_id;
	uint8_t payload_size;
    uint8_t *payload;         // payload only w/o header or crc
    uint8_t checksum;
} msp_v1_packet_t;

typedef struct msp_jumbo_packet_s {
	msp_direction_t direction;
	uint8_t command_id;
	uint16_t payload_size;
    uint8_t *payload;         // payload only w/o header or crc
    uint8_t checksum;
} msp_jumbo_packet_t;

typedef struct msp_v2_packet_s {
	msp_direction_t direction;
	uint8_t flag;
	uint16_t command_id;
	uint16_t payload_size;
    uint8_t *payload;         // payload only w/o header or crc
    uint8_t checksum;
} msp_v2_packet_t;


typedef struct msp_pachet_s{
	int valid : 1;
	msp_version_t msp_version;
    union {
    	msp_v1_packet_t v1;
    	msp_jumbo_packet_t jumbo;
    	msp_v2_packet_t v2;
    };
}msp_pachet_s;


typedef uint32_t (*msp_sys_now_ms_cb_fn)(void);

typedef struct msp_s{
	uint32_t _timeout_ms;
	msp_sys_now_ms_cb_fn sys_now_ms;
	msp_packet_state_t rx_packet_state;
	msp_pachet_s rx_packet;
	uint8_t rx_frame_buffer[MSP_RX_BUF_SIZE];
	uint32_t rx_frame_position;
	uint32_t rx_frame_full_len;
	uint32_t rx_frame_start_time_ms;

//	uint8_t tx_frame_buffer[MSP_TX_BUF_SIZE];
}msp_t;


void msp_update(msp_t *msp, uint8_t* rx_buf, uint32_t rx_buf_size);

#ifdef __cplusplus
}
#endif


#endif
