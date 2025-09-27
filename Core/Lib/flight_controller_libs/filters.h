#ifndef FILTERS_H
#define FILTERS_H


#ifdef __cplusplus
extern "C" {
#endif


#include <math.h>


// -----------------------------
// PT1 Filter Struct and API
// -----------------------------
typedef struct {
	float sample_freq;
    float cutoff_freq;  // Hz
    float alpha;        // Precomputed smoothing factor
    float state;        // Last output value
} pt1_filter_t;

/**
 * @brief Initialize a PT1 low-pass filter
 * @param filter Pointer to filter object
 * @param cutoff_freq Desired cutoff frequency in Hz
 * @param sample_rate Sampling rate in Hz
 */
void pt1_filter_init_lowpass(pt1_filter_t* filter, float cutoff_freq, float sample_rate);

/**
 * @brief Apply the PT1 filter to a new input sample
 * @param filter Pointer to filter object
 * @param input New input sample
 * @return Filtered output
 */
float pt1_filter_apply_lowpass(pt1_filter_t* filter, float input);

void pt1_filter_reset(pt1_filter_t* filter);




typedef struct {
	float sample_freq;
    float cutoff_freq;  // Hz
    float b0, b1, b2;   // Feedforward coefficients
    float a1, a2;       // Feedback coefficients
    float z1, z2;       // Filter state (delays)
} biquad_filter_t;


/**
 * @brief Compute center frequency f0 (Hz) from lower and upper cutoff frequencies.
 *
 * f0 = sqrt(f_low * f_high)
 *
 * @param f_low   Lower cutoff frequency in Hz (must be > 0)
 * @param f_high  Upper cutoff frequency in Hz (must be > f_low)
 */
#define BIQUAD_EDGES_TO_F0(f_low, f_high) (sqrtf((float)(f_low) * (float)(f_high)))

/**
 * @brief Compute quality factor Q from lower and upper cutoff frequencies.
 *
 * Q = f0 / (f_high - f_low)
 *
 * where f0 = sqrt(f_low * f_high)
 *
 * @param f_low   Lower cutoff frequency in Hz (must be > 0)
 * @param f_high  Upper cutoff frequency in Hz (must be > f_low)
 */
#define BIQUAD_EDGES_TO_Q_HZ(f_low, f_high) (BIQUAD_EDGES_TO_F0((f_low), (f_high)) / ((float)(f_high) - (float)(f_low)))

float biquad_filter_apply(biquad_filter_t* f, float input);

void biquad_filter_reset(biquad_filter_t* filter);


/**
 * @brief Initialize a second-order band-pass biquad filter.
 *
 * Configures the filter coefficients for a digital IIR band-pass filter
 * centered at frequency f0, with quality factor Q, using the given sample rate.
 *
 * @param f            Pointer to the filter state structure to initialize.
 * @param f0           Center frequency of the band-pass filter, in Hz.
 * @param Q            Quality factor (dimensionless).
 *                     - Defines the selectivity/sharpness of the filter.
 *                     - Bandwidth (Hz) is approximately f0 / Q.
 *                     - Lower Q (~0.7–1): wide pass band (gentle response).
 *                     - Higher Q (>5): narrow pass band, more selective.
 *                     - Q ≈ 0.707 gives a Butterworth (maximally flat) response.
 * @param sample_rate  Sampling rate of the signal in Hz.
 */
void biquad_filter_init_bandpass(biquad_filter_t *f,
                                 float f0,
                                 float Q,
                                 float sample_rate);

/**
 * @brief Initialize a second-order notch (band-stop) biquad filter.
 *
 * Configures the filter coefficients for a digital IIR notch filter
 * centered at frequency f0, with quality factor Q, using the given sample rate.
 *
 * @param f            Pointer to the filter state structure to initialize.
 * @param f0           Center frequency of the notch filter, in Hz.
 * @param Q            Quality factor (dimensionless).
 *                     - Defines the sharpness of the notch.
 *                     - Notch width (Hz) is approximately f0 / Q.
 *                     - Lower Q (~0.7–1): wide notch (removes broad frequency range).
 *                     - Higher Q (>10): very narrow, precise notch (e.g., 50/60 Hz hum removal).
 * @param sample_rate  Sampling rate of the signal in Hz.
 */
void biquad_filter_init_notch(biquad_filter_t *f,
                              float f0,
                              float Q,
                              float sample_rate);




typedef struct pt2_filter_s{
	pt1_filter_t s1;
	pt1_filter_t s2;
	float sample_freq;
    float cutoff_freq;  // Hz
} pt2_filter_t;

void pt2_filter_init_lowpass(pt2_filter_t *f, float cutoff_freq, float sample_freq);
float pt2_filter_apply_lowpass(pt2_filter_t *f, float x);
void pt2_filter_reset(pt2_filter_t *f);




#ifdef __cplusplus
}
#endif

#endif // FILTERS_H
