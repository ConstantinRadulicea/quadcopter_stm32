/**
 * @file
 * various utility macros
 */

#ifndef CRSF_HDR_DEF_H
#define CRSF_HDR_DEF_H

#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

#define HzToUs_float(hz) (1000000.0f/(hz))
#define HzToUs_int(hz) (1000000/(hz))
#define MilliToMicro_int(val) ((val)*1000)

#ifndef CRSF_LITTLE_ENDIAN
#define CRSF_LITTLE_ENDIAN 1234
#endif

#ifndef CRSF_BIG_ENDIAN
#define CRSF_BIG_ENDIAN 4321
#endif

#define CRSF_MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define CRSF_MIN(x , y)  (((x) < (y)) ? (x) : (y))

#ifndef CRSF_CLAMP
#define CRSF_CLAMP(x, lo, hi) (((x) < (lo)) ? (lo) : ((x) > (hi)) ? (hi) : (x))
#endif /* CLAMP */

/* Get the number of entries in an array ('x' must NOT be a pointer!) */
#define CRSF_ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

/** Create uint32_t value from bytes */
#define CRSF_MAKEU32(a,b,c,d) (((uint32_t)((a) & 0xff) << 24) | \
                               ((uint32_t)((b) & 0xff) << 16) | \
                               ((uint32_t)((c) & 0xff) << 8)  | \
                                (uint32_t)((d) & 0xff))

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    // Logic for Little-Endian (Standard for STM32)
    #define CRSF_BYTE_ORDER CRSF_LITTLE_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    // Logic for Big-Endian
    #define CRSF_BYTE_ORDER CRSF_BIG_ENDIAN
#else
    #error "Unknown byte order"
#endif

#if CRSF_BYTE_ORDER == CRSF_BIG_ENDIAN
#define crsf_htons(x) ((uint16_t)(x))
#define crsf_ntohs(x) ((uint16_t)(x))
#define crsf_htonl(x) ((uint32_t)(x))
#define crsf_ntohl(x) ((uint32_t)(x))
#define PP_HTONS(x)   ((uint16_t)(x))
#define PP_NTOHS(x)   ((uint16_t)(x))
#define PP_HTONL(x)   ((uint32_t)(x))
#define PP_NTOHL(x)   ((uint32_t)(x))
#else /* CRSF_BYTE_ORDER != CRSF_BIG_ENDIAN */

#ifndef crsf_htons
uint16_t crsf_htons(uint16_t x);
#endif
#define crsf_ntohs(x) crsf_htons(x)

#ifndef crsf_htonl
uint32_t crsf_htonl(uint32_t x);
#endif
#define crsf_ntohl(x) crsf_htonl(x)

/* These macros should be calculated by the preprocessor and are used
   with compile-time constants only (so that there is no little-endian
   overhead at runtime). */
#define PP_HTONS(x) ((uint16_t)((((x) & (uint16_t)0x00ffU) << 8) | (((x) & (uint16_t)0xff00U) >> 8)))
#define PP_NTOHS(x) PP_HTONS(x)
#define PP_HTONL(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                     (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                     (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                     (((x) & (uint32_t)0xff000000UL) >> 24))
#define PP_NTOHL(x) PP_HTONL(x)
#endif /* CRSF_BYTE_ORDER == CRSF_BIG_ENDIAN */

/* Provide usual function names as macros for users, but this can be turned off */
//#ifndef CRSF_DONT_PROVIDE_BYTEORDER_FUNCTIONS
//#define htons(x) crsf_htons(x)
//#define ntohs(x) crsf_ntohs(x)
//#define htonl(x) crsf_htonl(x)
//#define ntohl(x) crsf_ntohl(x)
//#endif


uint32_t writeU24BE(uint8_t* buffer, uint32_t value);


#include <stdint.h>

/**
 * @brief Converts a 16-bit Little Endian value to the Host's native format.
 * @note  On STM32 (Little Endian), this compiles to nothing (zero overhead).
 */
static inline uint16_t crsf_le16_to_host(uint16_t val) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return val;
#else
    // This part only runs if you move this code to a Big Endian CPU
    return (uint16_t)((val << 8) | (val >> 8));
#endif
}

/**
 * @brief Converts a 32-bit Little Endian value to the Host's native format.
 */
static inline uint32_t crsf_le32_to_host(uint32_t val) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return val;
#else
    return (((val & 0xff000000U) >> 24) | ((val & 0x00ff0000U) >> 8) | \
            ((val & 0x0000ff00U) << 8)  | ((val & 0x000000ffU) << 24));
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* CRSF_HDR_DEF_H */
