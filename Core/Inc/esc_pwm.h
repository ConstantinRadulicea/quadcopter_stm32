#ifndef ESC_PWM_H
#define ESC_PWM_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    TIM_HandleTypeDef *htim;   // internal timer handle storage
    uint32_t channel;          // TIM_CHANNEL_X
    float tim_clk_hz;       // timer input clock in Hz
    float pwm_hz;           // current PWM frequency
    float duty;             // 0.0 .. 1.0
} pwm_t;

/**
 * Initialize one PWM output.
 * Caller MUST enable the timer RCC before calling, e.g. __HAL_RCC_TIM5_CLK_ENABLE().
 *
 * @param esc            state holder (provided by caller)
 * @param TIMx           timer instance (e.g., TIM5)
 * @param channel        TIM_CHANNEL_1..4
 * @param timer_clock_hz actual timer input clock (e.g., 84'000'000 on F4 APB1 timers)
 * @param pwm_hz         target PWM frequency (e.g., 50..450 for many ESCs)
 * @param duty           0.0 .. 1.0 initial duty
 */
HAL_StatusTypeDef pwm_init(pwm_t *esc,
		TIM_HandleTypeDef *TIMx, uint32_t channel,
                               uint32_t timer_clock_hz, float pwm_hz, float duty);

void pwm_set_duty(pwm_t *esc, float duty_0_1);   // 0..1



void pwm_set_freq(pwm_t *esc, float pwm_hz);   // păstrează duty

float esc_pwm_throttle_to_duty_cycle(float pwm_hz, float min_us, float max_us, float throttle);


#ifdef __cplusplus
}
#endif





#endif
