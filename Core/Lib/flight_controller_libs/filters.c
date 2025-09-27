#include "filters.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

void pt1_filter_init_lowpass(pt1_filter_t* filter, float cutoff_freq, float sample_rate) {
	if(cutoff_freq <= FLT_EPSILON || sample_rate <= FLT_EPSILON){
		filter->alpha = 0.0f;
	}
	else{
	    float dt = 1.0f / sample_rate;
	    float rc = 1.0f / (2.0f * M_PI * cutoff_freq);
	    filter->alpha = dt / (dt + rc);
	}
	filter->sample_freq = sample_rate;
    filter->cutoff_freq = cutoff_freq;
    filter->state = 0.0f;
}

float pt1_filter_apply_lowpass(pt1_filter_t* filter, float input) {
    filter->state += (filter->alpha * (input - filter->state));
    return filter->state;
}

void pt1_filter_reset(pt1_filter_t* filter) {
    filter->state = 0.0f;
}

float biquad_filter_apply(biquad_filter_t* f, float input) {
    float output = f->b0 * input + f->z1;
    f->z1 = f->b1 * input - f->a1 * output + f->z2;
    f->z2 = f->b2 * input - f->a2 * output;
    return output;
}

void biquad_filter_reset(biquad_filter_t* filter) {
    filter->z1 = 0.0f;
    filter->z2 = 0.0f;
}

/* -------- Band-Pass (biquad) --------
   "Constant skirt gain" variant (classic, RBJ):
   Peak gain depends on Q. */
void biquad_filter_init_bandpass(biquad_filter_t *f, float f0, float Q, float sample_rate)
{
    float w0   = 2.0f * (float)M_PI * f0 / sample_rate;
    float cw0  = cosf(w0);
    float sw0  = sinf(w0);
    float alpha = sw0 / (2.0f * Q);

    float a0 = 1.0f + alpha;

    float b0 =  alpha;
    float b1 =  0.0f;
    float b2 = -alpha;
    float a1 = -2.0f * cw0;
    float a2 =  1.0f - alpha;

    // normalize (a0 -> 1)
    f->b0 = b0 / a0;
    f->b1 = b1 / a0;
    f->b2 = b2 / a0;
    f->a1 = a1 / a0;
    f->a2 = a2 / a0;
    biquad_filter_reset(f);
}

/* -------- Band-Stop / Notch (biquad) -------- */
void biquad_filter_init_notch(biquad_filter_t *f, float f0, float Q, float sample_rate)
{
    float w0   = 2.0f * (float)M_PI * f0 / sample_rate;
    float cw0  = cosf(w0);
    float sw0  = sinf(w0);
    float alpha = sw0 / (2.0f * Q);

    float a0 = 1.0f + alpha;

    float b0 = 1.0f;
    float b1 = -2.0f * cw0;
    float b2 = 1.0f;
    float a1 = -2.0f * cw0;
    float a2 = 1.0f - alpha;

    f->b0 = b0 / a0;
    f->b1 = b1 / a0;
    f->b2 = b2 / a0;
    f->a1 = a1 / a0;
    f->a2 = a2 / a0;
    biquad_filter_reset(f);
}


void pt2_filter_init_lowpass(pt2_filter_t *f, float cutoff_freq, float sample_freq) {
    pt1_filter_init_lowpass(&f->s1, cutoff_freq, sample_freq);
    pt1_filter_init_lowpass(&f->s2, cutoff_freq, sample_freq);
	f->sample_freq = sample_freq;
    f->cutoff_freq = cutoff_freq;
}

float pt2_filter_apply_lowpass(pt2_filter_t *f, float x) {
    return pt1_filter_apply_lowpass(&f->s2, pt1_filter_apply_lowpass(&f->s1, x));
}

void pt2_filter_reset(pt2_filter_t *f) {
    pt1_filter_reset(&f->s1);
    pt1_filter_reset(&f->s2);
}
