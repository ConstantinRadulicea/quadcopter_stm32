#include "imu.h"
#include <string.h>

void imu_init(
	imu_t* im,
	float accel_filter_cutoff_freq_hz,
	float gyro_filter_cutoff_freq_hz,
	float accel_sample_rate_hz,
	float gyro_sample_rate_hz,
	float madgwick_sample_rate_hz
) {
	memset(im, 0, sizeof(imu_t));

	imu_set_leveled_attitude(im, IMU_DEFAULT_LEVELD_ATTITUDE_Q); // set default leveld position to identity quaternion
	im->estimated_q = im->leveld_attitude_q; // identity quaternion

	im->accel_misalignment_A_inv[0][0] = 1.0f; // default: no misalignment
	im->accel_misalignment_A_inv[1][1] = 1.0f; // default: no misalignment
	im->accel_misalignment_A_inv[2][2] = 1.0f; // default: no misalignment

	pt1_filter_init_lowpass(&(im->accel_pt1_filter_x), accel_filter_cutoff_freq_hz, accel_sample_rate_hz);
	pt1_filter_init_lowpass(&(im->accel_pt1_filter_y), accel_filter_cutoff_freq_hz, accel_sample_rate_hz);
	pt1_filter_init_lowpass(&(im->accel_pt1_filter_z), accel_filter_cutoff_freq_hz, accel_sample_rate_hz);

	pt1_filter_init_lowpass(&(im->gyro_pt1_filter_x), gyro_filter_cutoff_freq_hz, gyro_sample_rate_hz);
	pt1_filter_init_lowpass(&(im->gyro_pt1_filter_y), gyro_filter_cutoff_freq_hz, gyro_sample_rate_hz);
	pt1_filter_init_lowpass(&(im->gyro_pt1_filter_z), gyro_filter_cutoff_freq_hz, gyro_sample_rate_hz);

	madgwick_filter_init(&(im->madgwick_filter), madgwick_sample_rate_hz, (float)IMU_GYRO_MEAN_ERROR_RAD);
}

void imu_set_leveled_attitude(imu_t* im, quaternion leveld_attitude_q) {
	quaternion q_ref = IMU_DEFAULT_LEVELD_ATTITUDE_Q;      // "leveled = identity"
	im->q_offset = quat_error(&q_ref, &leveld_attitude_q);
}

void imu_update_gyro(imu_t* im, coord3D imu_raw_gyro_rad) {

	im->raw_gyro = imu_raw_gyro_rad; // store raw gyro data

	im->calibrated_gyro.x = imu_raw_gyro_rad.x - im->gyro_bias[0];
	im->calibrated_gyro.y = imu_raw_gyro_rad.y - im->gyro_bias[1];
	im->calibrated_gyro.z = imu_raw_gyro_rad.z - im->gyro_bias[2];

	im->filtered_gyro.x = pt1_filter_apply_lowpass(&(im->gyro_pt1_filter_x), im->calibrated_gyro.x);
	im->filtered_gyro.y = pt1_filter_apply_lowpass(&(im->gyro_pt1_filter_y), im->calibrated_gyro.y);
	im->filtered_gyro.z = pt1_filter_apply_lowpass(&(im->gyro_pt1_filter_z), im->calibrated_gyro.z);

	im->body_frame_gyro = quatrotate(&(im->q_offset), &(im->filtered_gyro));
}

void imu_update_accel(imu_t* im, coord3D imu_raw_accel_m_s2) {
	im->raw_accel = imu_raw_accel_m_s2;
	
	MagCal(&(imu_raw_accel_m_s2.x), &(imu_raw_accel_m_s2.y), &(imu_raw_accel_m_s2.z), im->accel_bias, im->accel_misalignment_A_inv);

	im->calibrated_accel = imu_raw_accel_m_s2;

	im->filtered_accel.x = pt1_filter_apply_lowpass(&(im->accel_pt1_filter_x), imu_raw_accel_m_s2.x);
	im->filtered_accel.y = pt1_filter_apply_lowpass(&(im->accel_pt1_filter_y), imu_raw_accel_m_s2.y);
	im->filtered_accel.z = pt1_filter_apply_lowpass(&(im->accel_pt1_filter_z), imu_raw_accel_m_s2.z);

	im->body_frame_accel = quatrotate(&(im->q_offset), &(im->filtered_accel));
}

void imu_update_madgwick(imu_t* im) {
	madgwick_filter_apply(
		&(im->madgwick_filter),
		-im->body_frame_accel.x,
		-im->body_frame_accel.y,
		-im->body_frame_accel.z,
		im->body_frame_gyro.x,
		im->body_frame_gyro.y,
		im->body_frame_gyro.z
	);

//	madgwick_filter_apply(
//		&(im->madgwick_filter),
//		im->body_frame_accel.x,
//		-im->body_frame_accel.y,
//		-im->body_frame_accel.z,
//		im->body_frame_gyro.x,
//		-im->body_frame_gyro.y,
//		-im->body_frame_gyro.z
//	);

	//im->estimated_q = quatmultiply(&(im->q_offset), &(im->madgwick_filter.q_est));
	//im->estimated_q = quatnormalize(&(im->madgwick_filter.q_est));
	im->estimated_q = im->madgwick_filter.q_est;
}

void imu_update(imu_t* im, coord3D imu_raw_accel_m_s2, coord3D imu_raw_gyro_rad) {
	imu_update_accel(im, imu_raw_accel_m_s2);
	imu_update_gyro(im, imu_raw_gyro_rad);
	imu_update_madgwick(im);
}

void imu_set_gyro_bias(imu_t* im, coord3D gyro_bias) {
	im->gyro_bias[0] = gyro_bias.x;
	im->gyro_bias[1] = gyro_bias.y;
	im->gyro_bias[2] = gyro_bias.z;
}

void imu_set_accel_bias(imu_t* im, coord3D accel_bias, float accel_misalignment_A_1[3][3]) {
	im->accel_bias[0] = accel_bias.x;
	im->accel_bias[1] = accel_bias.y;
	im->accel_bias[2] = accel_bias.z;

	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++) {
			im->accel_misalignment_A_inv[i][j] = accel_misalignment_A_1[i][j];
		}
	}
}


void imu_get_estimated_data(imu_t* im, quaternion* estimated_q, coord3D* estimated_acc, coord3D* estimated_gyro) {
	if (estimated_q) {
		*estimated_q = im->estimated_q;
	}

	if (estimated_acc) {
		*estimated_acc = im->body_frame_accel;
	}

	if (estimated_gyro) {
		*estimated_gyro = im->body_frame_gyro;
	}
}

void imu_get_raw_accel_data(imu_t* im, coord3D* raw_acc){
	if (raw_acc) {
		*raw_acc = im->raw_accel;
	}
}

void imu_get_raw_gyro_data(imu_t* im, coord3D* raw_gyro){
		if (raw_gyro) {
		*raw_gyro = im->raw_gyro;
	}
}
