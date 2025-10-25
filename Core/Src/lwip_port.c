#include "lwipopts.h"
#include "lwip/mem.h"
#include "stdint.h"

#if defined ( __ICCARM__ )     /* IAR Compiler */
#pragma data_alignment=4
uint8_t lwip_heap[MEM_SIZE];

#elif defined ( __CC_ARM )     /* Keil / ARMCC */
__align(4) uint8_t lwip_heap[MEM_SIZE];

#elif defined ( __GNUC__ )     /* GCC / ARM-GCC (STM32CubeIDE) */
uint8_t lwip_heap[MEM_SIZE] __attribute__((aligned(32)));

#else
#warning "Compiler not supported for alignment"
uint8_t lwip_heap[MEM_SIZE];
#endif
