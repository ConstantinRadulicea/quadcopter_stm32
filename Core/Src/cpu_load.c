#include "stm32f4xx.h"     // adjust for your MCU
#include "stm32f4xx_hal.h" // for HAL_GetTick() if you use HAL
#include "atomic_custom.h"
#include "cpu_load.h"

#ifndef CPU_HZ
  #define CPU_HZ HAL_RCC_GetHCLKFreq()
#endif

/* State */
static uint32_t last_cyccnt;
static uint32_t last_ms;
static uint32_t last_val;

/* Call once at startup */
void cpu_load_init(void)
{
    // Enable DWT cycle counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM){
        last_ms    = HAL_GetTick();   // ms since boot (from SysTick ISR)
        last_val   = SysTick->VAL;    // current downcounter
    }

    last_cyccnt = DWT->CYCCNT;
}

/* Return CPU load since last call: 0.0 .. 1.0 */
float cpu_load_measure(void)
{
    uint32_t now_cyccnt = DWT->CYCCNT;

    // Take a coherent snapshot of SysTick (VAL + ms tick)
    uint32_t now_ms;
    uint32_t now_val;
    uint32_t reload;

    ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM){
        now_ms   = HAL_GetTick();
        now_val  = SysTick->VAL;
        reload   = SysTick->LOAD + 1U;   // number of core cycles per SysTick period
    }

    // Elapsed wall "cycles" = elapsed full SysTick periods + partial period progress
    // SysTick counts down from LOAD..1..0 then reloads
    uint32_t ms_delta = now_ms - last_ms;

    // How many cycles progressed within the current ms period:
    // When VAL decreases, progress increases. If it wrapped, VAL jumps up near LOAD.
    uint32_t val_delta_cycles;
    if (last_val >= now_val) {
        // normal case: progressed (last_val - now_val) ticks within the current ms
        val_delta_cycles = (last_val - now_val);
    } else {
        // wrapped once between samples: remaining from last_val to 0, plus from reload to now_val
        val_delta_cycles = (last_val + (reload - now_val));
    }

    // Total wall cycles elapsed
    uint64_t wall_cycles = (uint64_t)ms_delta * (uint64_t)reload + (uint64_t)val_delta_cycles;

    // Active CPU cycles
    uint32_t active_cycles = now_cyccnt - last_cyccnt;

    // Update baselines
    last_cyccnt = now_cyccnt;
    last_ms     = now_ms;
    last_val    = now_val;

    if (wall_cycles == 0ULL) return 0.0f;

    float load = (float)active_cycles / (float)wall_cycles;
    if (load > 1.0f) load = 1.0f;
    return load;  // 0.0 = fully idle, 1.0 = 100% busy
}
