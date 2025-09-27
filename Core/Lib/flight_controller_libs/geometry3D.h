#ifndef __GEOMETRY3D_H__
#define __GEOMETRY3D_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct coord2D {
	float x;
	float y;
}coord2D, rotation2D;

typedef struct coord3D {
	float x;
	float y;
	float z;
}coord3D, rotation3D, vector3D, angles3D, accel3D, veloc3D, pos3D;

typedef struct quaternion {
	float w;
	float x;
	float y;
	float z;
}quaternion;

struct EulerAngles {
	float roll, pitch, yaw;
};


void MagCal(float* _x, float* _y, float* _z, float _bias[3], float A_1[3][3]);

float Distance3D(float X1, float Y1, float Z1, float X2, float Y2, float Z2);

// Rotation order x, y, z (Roll, Pitch, Yaw)
void RotatePoint3D(coord3D* Point, rotation3D* Rotation);
// return the norm of a 3d vector
float norm3D(float x, float y, float z);

// return the angles between the axis and the vector
angles3D VectorAngles(vector3D* Vector);


// ================================================================
// ===						QUATERNIONS			                ===
// ================================================================
// https://www.mathworks.com/help/aerotbx/referencelist.html?type=function&category=flight-parameters-1&s_tid=CRUX_topnav

float quatnorm(quaternion* q);

quaternion quatconj(quaternion* q);

quaternion quatnormalize(quaternion* q);

quaternion quatinv(quaternion* q);

// Multiply a reference of a quaternion by a scalar, q = s*q
quaternion quatmultiply_scalar(quaternion *q, float scalar);

quaternion quatmultiply(quaternion* _quat1, quaternion* _quat2);

quaternion quatdivide(quaternion* _quat1, quaternion* _quat2);

vector3D quatrotate(quaternion* q, vector3D* vect);

// Quaternion to rotating angles (Euler angles)
// rotation order: yaw (Z), pitch (Y), roll (X)
angles3D quat2angle(quaternion* q);

vector3D quat2rotvec(quaternion q);

// rotation order: yaw (Z), pitch (Y), roll (X) in radians
quaternion angle2quat(angles3D* angles);

// Adds two quaternions together and the sum is the pointer to another quaternion, Sum = L + R
quaternion quatadd(quaternion *L, quaternion *R);

// Subtracts two quaternions together and the sum is the pointer to another quaternion, sum = L - R
quaternion quatsub(quaternion *L, quaternion *R);

// Compute error quaternion: q_err = conj(current) * desired
quaternion quat_error(quaternion *q_current, quaternion *q_desired);


// ================================================================
// ===						INTEGRALS				            ===
// ================================================================
float trapzY(float* y, size_t dim);

float trapzXY(float* x, float* y, size_t dim);

#ifdef __cplusplus
}
#endif

#endif // !__GEOMETRY3D_H__
