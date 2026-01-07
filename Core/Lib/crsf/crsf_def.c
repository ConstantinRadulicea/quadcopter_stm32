/**
 * @file
 * Common functions used throughout the stack.
 *
 * These are reference implementations of the byte swapping functions.
 * Again with the aim of being simple, correct and fully portable.
 * Byte swapping is the second thing you would want to optimize. You will
 * need to port it to your architecture and in your cc.h:
 *
 * \#define crsf_htons(x) your_htons
 * \#define crsf_htonl(x) your_htonl
 *
 * Note crsf_ntohs() and crsf_ntohl() are merely references to the htonx counterparts.
 *
 * If you \#define them to htons() and htonl(), you should
 * \#define CRSF_DONT_PROVIDE_BYTEORDER_FUNCTIONS to prevent lwIP from
 * defining htonx/ntohx compatibility macros.
 *
 */

#include "crsf_def.h"

#if CRSF_BYTE_ORDER == CRSF_LITTLE_ENDIAN

#if !defined(crsf_htons)
/**
 * Convert an uint16_t from host- to network byte order.
 *
 * @param n uint16_t in host byte order
 * @return n in network byte order
 */
uint16_t
crsf_htons(uint16_t n)
{
  return PP_HTONS(n);
}
#endif /* crsf_htons */

#if !defined(crsf_htonl)
/**
 * Convert an uint32_t from host- to network byte order.
 *
 * @param n uint32_t in host byte order
 * @return n in network byte order
 */
uint32_t
crsf_htonl(uint32_t n)
{
  return PP_HTONL(n);
}
#endif /* crsf_htonl */

#endif /* CRSF_BYTE_ORDER == CRSF_LITTLE_ENDIAN */

uint32_t writeU24BE(uint8_t* buffer, uint32_t value)
{
	buffer[0] = (value >> 16) & 0xFF;
	buffer[1] = (value >> 8) & 0xFF;
	buffer[2] = value & 0xFF;
	return 3;
}



uint16_t crsf_rcToUs(uint16_t rc)
{
    return (uint16_t)((rc * 0.62477120195241F) + 881);
}

uint16_t crsf_usToRc(uint16_t us)
{
    return (uint16_t)((us - 881) / 0.62477120195241F);
}
