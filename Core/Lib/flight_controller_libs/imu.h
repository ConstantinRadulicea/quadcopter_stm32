#ifndef IMU_H
#define IMU_H

#include "geometry3D.h"
#include "geometry2D.h"
#include "filters.h"
#include "madgwick_filter.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
	ROLL: roll to go left (NEGATIVE ROLL)
	ROLL: roll to go right (POSITIVE ROLL)

	PITCH: pitch to go forward (NEGATIVE PITCH)
	PITCH: pitch to go backward (POSITIVE PITCH)

	YAW: rotate left (CCW) (NEGATIVE YAW)
	YAW: rotate right (CW) (POSITIVE YAW)
 */

#define G_CONSTANT 9.80665f
#define G2MS2(g) ((g) * G_CONSTANT)

#define IMU_DEFAULT_LEVELD_ATTITUDE_Q ((quaternion){ 1.0f, 0.0f, 0.0f, 0.0f }) ///< Identity quaternion (scalar-first)
#define IMU_GYRO_MEAN_ERROR_RAD (radians(5.0f))                               ///< Gyro mean error used by Madgwick (rad/s)

/**
 * @file imu.h
 * @brief IMU pipeline: (bias/misalignment) → PT1 low-pass filters → Madgwick AHRS → reference offset.
 *
 * Conventions:
 *  - Quaternion layout: scalar-first (w,x,y,z)
 *  - Orientation: Madgwick output is body→world (left-multiply reference offset: q_estimated = q_offset * q_madgwick)
 *  - Units: accel [m/s²], gyro [rad/s]
 *
 * Calibration math used in the pipeline:
 *   a_corr = A_inv * (a_raw - b_a)     // accel bias/misalignment
 *   ω_corr = ω_raw - b_g               // gyro bias
 * where A_inv is a 3×3 matrix (inverse of accel scale/misalignment), b_a is accel bias, b_g is gyro bias.
 */

typedef struct imu_s {
    /* ---------------- RAW SENSOR DATA ---------------- */
    coord3D raw_accel;      ///< Raw accelerometer data [m/s²], as read from the sensor (unfiltered)
    coord3D raw_gyro;       ///< Raw gyroscope data [rad/s], as read from the sensor (unfiltered)

    /* ---------------- CALIBRATED SENSOR DATA ---------------- */
    coord3D calibrated_accel;      ///< Calibrated accelerometer data [m/s²], (after bias/misalignment correction) (unfiltered)
    coord3D calibrated_gyro;       ///< Calibrated gyroscope data [rad/s], (after bias/misalignment correction) (unfiltered)

    /* ---------------- FILTERED SENSOR DATA ---------------- */
    coord3D filtered_accel; ///< Low-pass filtered accelerometer data [m/s²] 
    coord3D filtered_gyro;  ///< Low-pass filtered gyroscope data [rad/s]

	/* ---------------- BODY-FRAME SENSOR DATA ---------------- */
	coord3D body_frame_accel; ///< Body-frame accelerometer data [m/s²] (after applying reference offset)
	coord3D body_frame_gyro;  ///< Body-frame gyroscope data [rad/s] (after applying reference offset)

    /* ---------------- ORIENTATION ESTIMATION ---------------- */
    quaternion estimated_q;     ///< Current orientation (world → body) estimate (Madgwick output from BODY-FRAME SENSOR DATA)

    /* ---------------- FILTER STRUCT INSTANCES ---------------- */
    madgwick_filter_t madgwick_filter; ///< Madgwick AHRS filter instance
    pt1_filter_t accel_pt1_filter_x;   ///< Low-pass filter for accel X-axis
    pt1_filter_t accel_pt1_filter_y;   ///< Low-pass filter for accel Y-axis
    pt1_filter_t accel_pt1_filter_z;   ///< Low-pass filter for accel Z-axis
    pt1_filter_t gyro_pt1_filter_x;    ///< Low-pass filter for gyro X-axis
    pt1_filter_t gyro_pt1_filter_y;    ///< Low-pass filter for gyro Y-axis
    pt1_filter_t gyro_pt1_filter_z;    ///< Low-pass filter for gyro Z-axis

    /* ---------------- CALIBRATION (SENSOR & REFERENCE) ---------------- */
    float accel_bias[3];                    ///< Accelerometer bias b_a [m/s²] (x,y,z)
    float accel_misalignment_A_inv[3][3];   ///< Accel misalignment/scale inverse matrix A_inv (row-major 3×3)
    float gyro_bias[3];                     ///< Gyroscope bias b_g [rad/s] (x,y,z)

    quaternion leveld_attitude_q;           ///< Orientation captured when IMU is physically leveled (reference pose)
    quaternion q_offset;                    ///< Reference offset: maps leveled pose to identity. Apply as q_est = q_offset * q_madgwick
} imu_t;

/**
 * @brief Initialize IMU structure, filters, and Madgwick filter.
 *        Does NOT perform sensor calibration; biases/misalignment remain whatever you set later.
 *        Sets q_offset = identity and estimated_q = level reference (identity unless changed).
 *
 * Pipeline after initialization (when you call imu_update*):
 *   raw → (subtract biases, apply A_inv) → PT1 LPFs → Madgwick → q_offset → estimated_q
 *
 * @param im                          Pointer to IMU struct.
 * @param accel_filter_cutoff_freq_hz Low-pass cutoff freq for accelerometer [Hz].
 * @param gyro_filter_cutoff_freq_hz  Low-pass cutoff freq for gyroscope [Hz].
 * @param accel_sample_rate_hz        Accelerometer sample rate [Hz].
 * @param gyro_sample_rate_hz         Gyroscope sample rate [Hz].
 * @param madgwick_sample_rate_hz     Madgwick filter update rate [Hz].
 */
void imu_init(
    imu_t* im,
    float accel_filter_cutoff_freq_hz,
    float gyro_filter_cutoff_freq_hz,
    float accel_sample_rate_hz,
    float gyro_sample_rate_hz,
    float madgwick_sample_rate_hz
);

/**
 * @brief Set the reference orientation when the IMU is physically leveled.
 *        Typically followed by computing q_offset = conj(q_level) so that leveled → identity.
 *
 * @param im                 Pointer to IMU struct.
 * @param leveld_attitude_q  Quaternion captured when the device is level (scalar-first).
 */
void imu_set_leveled_attitude(imu_t* im, quaternion leveld_attitude_q);

/**
 * @brief Process raw gyroscope readings: store raw data, subtract gyro bias, low-pass filter,
 *        and keep filtered_gyro in rad/s ready for Madgwick.
 *
 * @param im                 Pointer to IMU struct.
 * @param imu_raw_gyro_rad   Raw gyroscope readings [rad/s].
 */
void imu_update_gyro(imu_t* im, coord3D imu_raw_gyro_rad);

/**
 * @brief Process raw accelerometer readings: store raw data, subtract accel bias, apply A_inv
 *        (misalignment/scale inverse), then low-pass filter into filtered_accel.
 *
 * @param im                     Pointer to IMU struct.
 * @param imu_raw_accel_m_s2     Raw accelerometer readings [m/s²].
 */
void imu_update_accel(imu_t* im, coord3D imu_raw_accel_m_s2);

/**
 * @brief Run Madgwick AHRS filter using the filtered sensor data,
 *        then apply q_offset → estimated_q = q_offset * q_madgwick.
 *
 * @param im   Pointer to IMU struct.
 */
void imu_update_madgwick(imu_t* im);

/**
 * @brief Full IMU update: accel path + gyro path + Madgwick + reference offset.
 *
 * @param im                     Pointer to IMU struct.
 * @param imu_raw_accel_m_s2     Raw accelerometer readings [m/s²].
 * @param imu_raw_gyro_rad       Raw gyroscope readings [rad/s].
 */
void imu_update(imu_t* im, coord3D imu_raw_accel_m_s2, coord3D imu_raw_gyro_rad);

/**
 * @brief Set gyroscope bias (zero-rate offsets) in rad/s. Applied BEFORE filtering.
 *        ω_corr = ω_raw - b_g
 *
 * @param im        Pointer to IMU struct.
 * @param gyro_bias Bias vector [rad/s] (x,y,z).
 */
void imu_set_gyro_bias(imu_t* im, coord3D gyro_bias);

/**
 * @brief Set accelerometer bias and misalignment matrix inverse (A_inv). Applied BEFORE filtering.
 *        a_corr = A_inv * (a_raw - b_a)
 *
 * @param im                Pointer to IMU struct.
 * @param accel_bias        Bias vector b_a [m/s²] (x,y,z).
 * @param accel_misalignment_A_1 3×3 inverse matrix A_inv (row-major). Pass identity if not calibrated.
 */
void imu_set_accel_bias(imu_t* im, coord3D accel_bias, float accel_misalignment_A_inv[3][3]);

/**
 * @brief Get the estimated orientation quaternion, body-frame accelerometer, and gyroscope data.
 *        This is the output of the IMU pipeline after applying all filters and offsets.
 *
 * @param im                 Pointer to IMU struct.
 * @param estimated_q        Pointer to store the estimated orientation quaternion (body → world).
 * @param estimated_acc      Pointer to store the body-frame accelerometer data [m/s²].
 * @param estimated_gyro     Pointer to store the body-frame gyroscope data [rad/s].
 */
void imu_get_estimated_data(imu_t* im, quaternion* estimated_q, coord3D* estimated_acc, coord3D* estimated_gyro);


void imu_get_raw_accel_data(imu_t* im, coord3D* raw_acc);

void imu_get_raw_gyro_data(imu_t* im, coord3D* raw_gyro);


#ifdef __cplusplus
}
#endif

#endif // IMU_H
