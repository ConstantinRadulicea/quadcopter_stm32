#ifndef ATOMIC_CUSTOM_H
#define ATOMIC_CUSTOM_H

#include "stdint.h"
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* util/atomic.h simplified */

#define ATOMIC_RESTORESTATE_CUSTOM  0
#define ATOMIC_FORCEON_CUSTOM       1

typedef uint32_t crit_state_t;

#define PRIMASK_ENABLE_INTERRUPTS ((crit_state_t) 0)
#define PRIMASK_DISABLE_INTERRUPTS ((crit_state_t) 1)

#define COMPILER_BARRIER() __asm volatile ("" ::: "memory")


__attribute__((always_inline)) static inline crit_state_t enter_critical(void)
{
    crit_state_t old = __get_PRIMASK();  // 0 or 1
    __disable_irq();                     // CPSID i (atomic)
    __DSB();
    __ISB();
    COMPILER_BARRIER();
    return old;
}

__attribute__((always_inline)) static inline void exit_critical(crit_state_t old)
{
    __DMB();
    __set_PRIMASK(old);                  // atomic restore (one instruction)
    __DSB();
    __ISB();
    COMPILER_BARRIER();
}

/* Helpers: */
__attribute__((always_inline)) static inline crit_state_t __iBegin_custom(crit_state_t type)
{
	crit_state_t sreg = enter_critical();   // save interrupt flag state and disable interrupts
    return (type == ATOMIC_FORCEON_CUSTOM) ? PRIMASK_ENABLE_INTERRUPTS : sreg;
}

__attribute__((always_inline)) static inline void __iRestore_custom(crit_state_t *sreg)
{
	exit_critical(*sreg);		// restore saved I-bit
}


#define ATOMIC_BLOCK_CUSTOM(type) for ( crit_state_t __atomic_tmp __attribute__((__cleanup__(__iRestore_custom))) = __iBegin_custom(type), __toDo = 1; __toDo ; __toDo = 0 )

#ifdef __cplusplus
}
#endif

#endif
