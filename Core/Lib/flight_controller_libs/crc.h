#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize CRC-16-CCITT computation.
 *
 * This function returns the initial CRC value used for
 * CRC-16-CCITT (XModem) computations. The standard parameters
 * are:
 *   - Polynomial: 0x1021
 *   - Initial value: 0xFFFF
 *   - No reflection of input/output bits
 *   - No final XOR
 *
 * @return Initial CRC value (0xFFFF).
 */
uint16_t crc16_ccitt_init(void);


/**
 * @brief Update CRC-16-CCITT with a single byte.
 *
 * Processes one byte and updates the current CRC value according
 * to the CRC-16-CCITT (XModem) algorithm.
 *
 * @param crc Current CRC value (running CRC).
 * @param a   Next byte to process.
 * @return Updated CRC value after processing the byte.
 */
uint16_t crc16_ccitt_add(uint16_t crc, uint8_t a);


/**
 * @brief Update CRC-16-CCITT with a buffer of data.
 *
 * Processes a data buffer of arbitrary length and updates the
 * CRC value according to the CRC-16-CCITT (XModem) algorithm.
 *
 * Polynomial: 0x1021, Initial value: 0xFFFF,
 * No reflection, No final XOR.
 *
 * @param crc   Current CRC value (running CRC).
 * @param data  Pointer to the data buffer.
 * @param len   Number of bytes in the buffer.
 * @return Updated CRC value after processing the buffer.
 */
uint16_t crc16_ccitt_add_arr(uint16_t crc, uint8_t* data, size_t len);


uint8_t crc8_add(uint8_t crc, uint8_t data, uint8_t poly);
uint8_t crc8_add_arr(uint8_t crc, uint8_t* data, size_t len, uint8_t poly);

#define CRC8_DVB_S2_POLY 0xD5
#define CRC8_DVB_S2_START_CRC 0x0

#define crc8_dvb_s2_init(void) ((uint8_t) CRC8_DVB_S2_START_CRC)
#define crc8_dvb_s2_add(crc, data) (crc8_add(crc, data, CRC8_DVB_S2_POLY))
#define crc8_dvb_s2_add_arr(crc, data, len) (crc8_add_arr(crc, data, len, CRC8_DVB_S2_POLY))


#ifdef __cplusplus
}
#endif

#endif // !CRC_H
