#include "rtos_flight_controller.h"
#include "atomic_custom.h"
#include "emergency_handlers.h"

void emergency_shutdown(void) {
    for (size_t i = 0; i < (sizeof(esc_motors) / sizeof(esc_motors[0])); i++) {
    	float duty_cycle = esc_pwm_throttle_to_duty_cycle(ESC_PWM_HZ, ESC_PWM_MIN_US, ESC_PWM_MAX_US, 0.0f);
    	ATOMIC_BLOCK_CUSTOM(ATOMIC_RESTORESTATE_CUSTOM)
    	{
    		pwm_set_duty(&esc_motors[i], duty_cycle);
    	}
    }

    // HARDWARE LOCK: Disable all interrupts except absolute essentials
    enter_critical();

    while (1) {
        HAL_Delay(100);
    }
}
