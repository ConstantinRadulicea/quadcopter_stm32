#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

/* --- Includes available on STM32 with newlib --- */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>

/* ---- Endianness (ARM Cortex-M is little-endian) ---- */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* ---- Types required by lwIP ---- */
typedef uint32_t sys_prot_t;   /* Used for critical region protection (enter/exit) */

/* ---- Format specifiers for lwIP debug/printf helpers ----
   (Match lwIP expectations; keep them simple for 32-bit targets) */
#define X8_F   "02x"

#define U16_F  "hu"
#define S16_F  "hd"
#define X16_F  "hx"

#define U32_F  "u"
#define S32_F  "d"
#define X32_F  "x"

#define SZT_F  "u"

/* ---- Struct packing for GCC/Clang ----
   lwIP uses these in protocol header structs. */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#define PACK_STRUCT_STRUCT        __attribute__((packed))
#define PACK_STRUCT_FIELD(x)      x

/* If you prefer the include-style packing, you can alternatively:
   #define PACK_STRUCT_USE_INCLUDES
*/

/* ---- Diagnostics / Assertions ----
   Route to your logger (printf, ITM, SEGGER_RTT, etc.). */
#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x)     do { printf x; } while (0)
#endif

#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(msg)                                       \
  do {                                                                   \
    printf("LWIP ASSERT: %s (%s:%d)\r\n", (msg), __FILE__, __LINE__);    \
    /* trap here */                                                      \
    for (;;) { __asm volatile("bkpt #0"); }                              \
  } while (0)
#endif

/* ---- Random source for things like initial sequence numbers ----
   If you have HAL RNG, use it; otherwise fall back to rand(). */
#if defined(HAL_RNG_MODULE_ENABLED)
#include "stm32xxxx_hal.h" /* replace xxxx with your family, e.g. stm32f4xx_hal.h */
extern RNG_HandleTypeDef hrng;
static inline uint32_t lwip_hw_rand(void) {
  uint32_t v;
  if (HAL_RNG_GenerateRandomNumber(&hrng, &v) == HAL_OK) return v;
  return 0x6d5a56aaU; /* fallback */
}
#define LWIP_RAND()  lwip_hw_rand()
#else
#include <stdlib.h>
#define LWIP_RAND()  ((uint32_t)rand())
#endif

/* ---- Errno handling ----
   newlib provides errno; tell lwIP to include the system one. */
#define LWIP_ERRNO_STDINCLUDE 1
/* (Do NOT also define LWIP_PROVIDE_ERRNO when using newlib’s errno.) */

/* ---- timeval handling ----
   On bare-metal you usually don’t have <sys/time.h>; keep lwIP’s internal one.
   If you provide struct timeval yourself, set this to 0 and include <sys/time.h>. */
#define LWIP_TIMEVAL_PRIVATE 1

/* ---- Optional: pull in a project-specific PPP settings header if you use one ---- */
/* #define PPP_INCLUDE_SETTINGS_HEADER */

#endif /* LWIP_ARCH_CC_H */
