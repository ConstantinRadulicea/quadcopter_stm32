#include "lwipopts.h"
#include "lwip/mem.h"
#include "stdint.h"

/* Place the heap in a chosen RAM region */
#if defined ( __CC_ARM )
  __attribute__((section(".lwip_heap"))) __attribute__((aligned(4)))
#elif defined ( __GNUC__ )
  __attribute__((section(".lwip_heap"))) __attribute__((aligned(4)))
#endif
uint8_t lwip_heap[MEM_SIZE];
