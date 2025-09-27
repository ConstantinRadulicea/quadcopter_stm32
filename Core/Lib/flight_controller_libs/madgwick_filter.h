//
//  madgwickFilter.h
//  madgwickFilter
//
//  Created by Blake Johnson on 4/28/20.
//

#ifndef MADGWICK_FILTER_H
#define MADGWICK_FILTER_H

#include "geometry3D.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct madgwick_filter{
	volatile quaternion q_est;			// quaternion
	float sampling_freq;		// hz
	float gyro_mean_error;		//radians

}madgwick_filter_t;


void madgwick_filter_init(madgwick_filter_t* filter, float sampling_freq, float gyro_mean_error);


// IMU consists of a Gyroscope plus Accelerometer sensor fusion
void madgwick_filter_apply(madgwick_filter_t *filter, float ax, float ay, float az, float gx, float gy, float gz);


#ifdef __cplusplus
}
#endif

#endif /* MADGWICK_FILTER_H */
