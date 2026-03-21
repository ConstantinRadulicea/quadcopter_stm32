//
//  madgwickFilter.h
//  madgwickFilter
//
//  Created by Blake Johnson on 4/28/20.
// https://github.com/bjohnsonfl/Madgwick_Filter/tree/master
//

#ifndef MADGWICK_FILTER_H
#define MADGWICK_FILTER_H

#include "geometry3D.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef G_CONSTANT
	#define G_CONSTANT 9.80665f
#endif

#ifndef G2MS2
	#define G2MS2(g) ((g) * G_CONSTANT)
#endif

#ifndef MS2TOG
	#define MS2TOG(ms2) ((ms2) / G_CONSTANT)
#endif

typedef struct madgwick_filter{
	volatile quaternion q_est;			// quaternion
	float sampling_freq;		// hz
	float gyro_mean_error;		/////< gyro Zero-Rate Output (rad/s). Taken by the gyro datasheet

}madgwick_filter_t;


void madgwick_filter_init(madgwick_filter_t* filter, float sampling_freq, float gyro_mean_error);


// Accelerometer data is in m/s^2
// Gyroscope Angular Velocity components are in Radians per Second
void madgwick_filter_apply_6dof(madgwick_filter_t *filter, float ax, float ay, float az, float gx, float gy, float gz);

// Accelerometer data is in m/s^2
// Gyroscope Angular Velocity components are in Radians per Second
// magnetometer data is in micro Tesla (uT)
void madgwick_filter_apply_9dof(madgwick_filter_t *filter, float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);



void madgwick_filter_apply(madgwick_filter_t *filter, float ax, float ay, float az, float gx, float gy, float gz);

#ifdef __cplusplus
}
#endif

#endif /* MADGWICK_FILTER_H */
