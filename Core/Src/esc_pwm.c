// esc_pwm_dc.c
#include "esc_pwm.h"

static inline uint32_t arr_from_freqf(uint32_t base_hz, float pwm_hz_f) {
    if (pwm_hz_f < 1e-3f) pwm_hz_f = 1e-3f;           // protecție
    double ticks = (float)base_hz / (float)pwm_hz_f; // folosește double pt. precizie
    uint32_t period_ticks = (uint32_t)(ticks + 0.5);   // rotunjire la cel mai apropiat
    if (period_ticks < 1u) period_ticks = 1u;
    return period_ticks - 1u;                          // ARR = top - 1
}

static inline uint32_t ccr_from_duty(uint32_t arr, float duty) {
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 1.0f) duty = 1.0f;
    uint32_t top = arr + 1u;
    uint32_t ccr = (uint32_t)((float)top * (float)duty + 0.5f);
    if (ccr > arr) ccr = arr;
    return ccr;
}


// One static TIM handle backing this instance (simple path).
// If you need multiple instances, either:
// 1) make this non-static and let caller provide storage, or
// 2) keep an array of TIM_HandleTypeDef.
HAL_StatusTypeDef pwm_init(pwm_t *esc,
    TIM_HandleTypeDef *htim, uint32_t channel,
    uint32_t timer_clock_hz, float pwm_hz, float duty)
{
    if (!esc || !htim) return HAL_ERROR;
    if (timer_clock_hz < 1000000u) return HAL_ERROR;

    HAL_StatusTypeDef st;
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};

    esc->htim       = htim;
    esc->channel    = channel;
    esc->tim_clk_hz = timer_clock_hz;
    esc->pwm_hz     = (pwm_hz <= 0.0f) ? 50.0f : pwm_hz;
    esc->duty       = (duty < 0.0f) ? 0.0f : (duty > 1.0f ? 1.0f : duty);

    uint32_t psc = (timer_clock_hz / 1000000u) - 1u;  // 1 MHz base
    if (psc > 0xFFFFu) return HAL_ERROR;

    //HAL_TIM_Base_DeInit(htim);
    HAL_TIM_PWM_DeInit(htim);

    htim->Init.Prescaler         = psc;
    htim->Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim->Init.Period            = arr_from_freqf(1000000u, esc->pwm_hz);
    htim->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    st = HAL_TIM_Base_Init(htim);
    if (st != HAL_OK) return st;


    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    st = HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig);

    st = HAL_TIM_PWM_Init(htim);
    if (st != HAL_OK) return st;


    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig);
    if (st != HAL_OK) return st;


    TIM_OC_InitTypeDef oc = {0};
    oc.OCMode     = TIM_OCMODE_PWM1;
    oc.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc.OCFastMode = TIM_OCFAST_DISABLE;
    oc.Pulse      = ccr_from_duty(htim->Init.Period, esc->duty);

    st = HAL_TIM_PWM_ConfigChannel(htim, &oc, channel);
    if (st != HAL_OK) return st;

    return HAL_TIM_PWM_Start(htim, channel);
}



void pwm_set_duty(pwm_t *esc, float duty_0_1) {
    if (!esc) return;
    esc->duty = (duty_0_1 < 0.0f) ? 0.0f : (duty_0_1 > 1.0f ? 1.0f : duty_0_1);
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(esc->htim);
    uint32_t ccr = ccr_from_duty(arr, esc->duty);
    __HAL_TIM_SET_COMPARE(esc->htim, esc->channel, ccr);
}

void pwm_set_freq(pwm_t *esc, float pwm_hz) {
    if (!esc || pwm_hz <= 0.f) return;
    esc->pwm_hz = pwm_hz;

    __HAL_TIM_DISABLE(esc->htim);

    uint32_t arr = arr_from_freqf(1000000u, pwm_hz);
    __HAL_TIM_SET_AUTORELOAD(esc->htim, arr);

    uint32_t ccr = ccr_from_duty(arr, esc->duty);
    __HAL_TIM_SET_COMPARE(esc->htim, esc->channel, ccr);

    // Latch imediat (dacă macro-ul nu există, folosește TIMx->EGR = TIM_EGR_UG;)
    HAL_TIM_GenerateEvent(esc->htim, TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_ENABLE(esc->htim);
}

float esc_pwm_throttle_to_duty_cycle(float pwm_hz, float min_us, float max_us, float throttle)
{
    if (throttle < 0.0f) throttle = 0.0f;
    if (throttle > 1.0f) throttle = 1.0f;

    // Period of the PWM in microseconds
    float period_us = 1000000.0f / pwm_hz;

    // Map throttle to pulse width in microseconds
    float pulse_us = min_us + throttle * (max_us - min_us);

    // Duty cycle = pulse / period
    float duty = pulse_us / period_us;

    // Clamp to [0..1]
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 1.0f) duty = 1.0f;

    return duty;
}
