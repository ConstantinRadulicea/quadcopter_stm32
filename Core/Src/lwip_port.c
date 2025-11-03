#include "lwipopts.h"
#include "lwip/mem.h"
#include "stdint.h"

#if defined ( __ICCARM__ )     /* IAR Compiler */
#pragma data_alignment=MEM_ALIGNMENT
uint8_t lwip_heap[MEM_SIZE];

#elif defined ( __CC_ARM )     /* Keil / ARMCC */
__align(MEM_ALIGNMENT) uint8_t lwip_heap[MEM_SIZE];

#elif defined ( __GNUC__ )     /* GCC / ARM-GCC (STM32CubeIDE) */
__attribute__((section(".ccmram"), aligned(MEM_ALIGNMENT))) uint8_t lwip_heap[MEM_SIZE];
//__attribute__((aligned(MEM_ALIGNMENT))) uint8_t lwip_heap[MEM_SIZE];


#else
#warning "Compiler not supported for alignment"
uint8_t lwip_heap[MEM_SIZE];
#endif
