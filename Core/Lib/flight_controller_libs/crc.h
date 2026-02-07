#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t crc8_add(uint8_t crc, uint8_t data, uint8_t poly);
uint8_t crc8_add_arr(uint8_t crc, uint8_t* data, size_t len, uint8_t poly);

uint16_t crc16_add(uint16_t crc, uint8_t data, uint16_t poly);
uint16_t crc16_add_arr(uint16_t crc, uint8_t* data, size_t len, uint16_t poly);

uint8_t crc8_xor_add(uint8_t crc, uint8_t data);
uint8_t crc8_xor_add_arr(uint8_t crc, uint8_t* data, size_t len);


#define CRC8_DVB_S2_POLY ((uint8_t)0xD5)
#define CRC8_DVB_S2_START_CRC ((uint8_t)0x0)

#define crc8_dvb_s2_init(void) ((uint8_t) CRC8_DVB_S2_START_CRC)
#define crc8_dvb_s2_add(crc, data) (crc8_add(crc, data, CRC8_DVB_S2_POLY))
#define crc8_dvb_s2_add_arr(crc, data, len) (crc8_add_arr(crc, data, len, CRC8_DVB_S2_POLY))


#define CRC16_CCITT_POLY ((uint16_t)0x1021)
#define CRC16_CCITT_START_CRC ((uint16_t)0xFFFF)

#define crc16_ccitt_init(void) ((uint16_t) CRC16_CCITT_START_CRC)
#define crc16_ccitt_add(crc, data) (crc16_add(crc, data, CRC16_CCITT_POLY))
#define crc16_ccitt_add_arr(crc, data, len) (crc16_add_arr(crc, data, len, CRC16_CCITT_POLY))


#ifdef __cplusplus
}
#endif

#endif // !CRC_H
