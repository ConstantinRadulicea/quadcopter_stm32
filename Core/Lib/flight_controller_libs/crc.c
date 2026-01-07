#include "crc.h"


uint16_t crc16_ccitt_init(void) {
	return (uint16_t)0xFFFF; // Initial value for CRC-16-CCITT
}

uint16_t crc16_ccitt_add(uint16_t crc, uint8_t a)
{
    crc ^= (uint16_t)a << (uint8_t)8;
    for (uint8_t ii = (uint8_t)0; ii < (uint8_t)8; ++ii) {
        if (crc & (uint16_t)0x8000) {
            crc = (crc << (uint8_t)1) ^ (uint16_t)0x1021;
        }
        else{
        	crc = (crc << (uint8_t)1);
        }
    }
    return crc;
}

uint16_t crc16_ccitt_add_arr(uint16_t crc, uint8_t* data, size_t len) {
    for (size_t i = (size_t)0; i < len; i++) {
        crc = crc16_ccitt_add(crc, data[i]);
    }
    return crc;
}


uint8_t crc8_add(uint8_t crc, uint8_t data, uint8_t poly){
    crc ^= data;
    for (int ii = 0; ii < 8; ++ii) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ poly;
        } else {
            crc = crc << 1;
        }
    }
    return crc;
}

uint8_t crc8_add_arr(uint8_t crc, uint8_t* data, size_t len, uint8_t poly){
    for (size_t i = (size_t)0; i < len; i++) {
        crc = crc8_add(crc, data[i], poly);
    }
    return crc;
}

