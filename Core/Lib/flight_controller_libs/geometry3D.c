#include "geometry3D.h"

#include <math.h>
#include <float.h>
#include "geometry2D.h"

#ifdef __cplusplus
extern "C" {
#endif

void MagCal(float* _x, float* _y, float* _z, float _bias[3], float A_1[3][3]) {
	//h_cal = (xyz - _bias) * A^-1
	float x_temp, y_temp, z_temp;

	x_temp = *_x;
	y_temp = *_y;
	z_temp = *_z;

	x_temp -= _bias[0];
	y_temp -= _bias[1];
	z_temp -= _bias[2];


	x_temp = x_temp * A_1[0][0] + y_temp * A_1[1][0] + z_temp * A_1[2][0];
	y_temp = x_temp * A_1[0][1] + y_temp * A_1[1][1] + z_temp * A_1[2][1];
	z_temp = x_temp * A_1[0][2] + y_temp * A_1[1][2] + z_temp * A_1[2][2];

	*_x = x_temp;
	*_y = y_temp;
	*_z = z_temp;
}

float Distance3D(float X1, float Y1, float Z1, float X2, float Y2, float Z2) {
	return sqrtf(powf(X2 - X1, 2) + powf(Y2 - Y1, 2) + powf(Z2 - Z1, 2));
}

// Rotation order x, y, z (Roll, Pitch, Yaw)
void RotatePoint3D(coord3D* Point, rotation3D* Rotation) {
	float a, b, c;
	float cos_a, cos_b, cos_c;
	float sin_a, sin_b, sin_c;
	float x, y, z;

	// Rotation order x, y, z
	a = Rotation->z; // Roll u
	b = Rotation->y; //	Pitch v
	c = Rotation->x; // Yaw w

	//// Rotation order x, y, z (Roll, Pitch, Yaw)
	//a = Rotation->x; // Roll u
	//b = Rotation->y; //	Pitch v
	//c = Rotation->z; // Yaw w

	cos_a = cosf(a);
	cos_b = cosf(b);
	cos_c = cosf(c);

	sin_a = sinf(a);
	sin_b = sinf(b);
	sin_c = sinf(c);

	x = Point->x;
	y = Point->y;
	z = Point->z;

	Point->x = ((cos_a * cos_b) * x) + ((cos_a * sin_b * sin_c - sin_a * cos_c) * y) + ((cos_a * sin_b * cos_c + sin_a * sin_c) * z);
	Point->y = ((sin_a * cos_b) * x) + ((sin_a * sin_b * sin_c + cos_a * cos_c) * y) + ((sin_a * sin_b * cos_c - cos_a * sin_c) * z);
	Point->z = ((-sin_b) * x) + ((cos_b * sin_c) * y) + ((cos_b * cos_c) * z);
}

// return the norm of a 3d vector
float norm3D(float x, float y, float z) {
	return sqrtf((x*x) + (y*y) + (z*z));
}

// return the angles between the axis and the vector
angles3D VectorAngles(vector3D* Vector) {
    angles3D ResultAngles;
    float x = Vector->x;
    float y = Vector->y;
    float z = Vector->z;

    float v_norm = sqrtf(x*x + y*y + z*z);

    if (v_norm <= FLT_EPSILON) {
        ResultAngles.x = 0.0f;
        ResultAngles.y = 0.0f;
        ResultAngles.z = 0.0f;
        return ResultAngles;
    }

    ResultAngles.x = acosf(x / v_norm); // angle to X axis
    ResultAngles.y = acosf(y / v_norm); // angle to Y axis
    ResultAngles.z = acosf(z / v_norm); // angle to Z axis

    return ResultAngles;
}


// ================================================================
// ===						QUATERNIONS			                ===
// ================================================================
// https://www.mathworks.com/help/aerotbx/referencelist.html?type=function&category=flight-parameters-1&s_tid=CRUX_topnav

float quatnorm(quaternion* q) {
	return sqrtf((q->w * q->w) + (q->x * q->x) + (q->y * q->y) + (q->z * q->z));
}

quaternion quatconj(quaternion* q) {
    quaternion result;
    result.w =  q->w;
    result.x = -q->x;
    result.y = -q->y;
    result.z = -q->z;
    return result;
}

quaternion quatnormalize(quaternion* q) {
    float im;
    quaternion result;
	float q_norm = quatnorm(q);

	if (q_norm <= FLT_EPSILON){
	    result.w = 1.0f;
	    result.x = 0.0f;
	    result.y = 0.0f;
	    result.z = 0.0f;
		return result;
	}

	im = 1.0f / q_norm;
    result.w = q->w * im;
    result.x = q->x * im;
    result.y = q->y * im;
    result.z = q->z * im;
    return result;
}

quaternion quatinv(quaternion* q) {
	quaternion result;
    quaternion qinv = quatconj(q);
    float norm_sq = quatnorm(q);

    norm_sq = norm_sq * norm_sq;  // Use ||q||^2

	if (norm_sq <= FLT_EPSILON){
	    result.w = 1.0f;
	    result.x = 0.0f;
	    result.y = 0.0f;
	    result.z = 0.0f;
		return result;
	}

    result.w = qinv.w / norm_sq;
    result.x = qinv.x / norm_sq;
    result.y = qinv.y / norm_sq;
    result.z = qinv.z / norm_sq;
    return result;
}

// Multiply a reference of a quaternion by a scalar, q = s*q
quaternion quatmultiply_scalar(quaternion *q, float scalar){
	quaternion res;
    res.w = q->w * scalar;
    res.x = q->x * scalar;
    res.y = q->y * scalar;
    res.z = q->z * scalar;
    return res;
}

quaternion quatmultiply(quaternion* _quat1, quaternion* _quat2) {
	quaternion res;
	float w1, x1, y1, z1;
	float w2, x2, y2, z2;

	w1 = _quat1->w;
	x1 = _quat1->x;
	y1 = _quat1->y;
	z1 = _quat1->z;

	w2 = _quat2->w;
	x2 = _quat2->x;
	y2 = _quat2->y;
	z2 = _quat2->z;

	res.w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;  // new w
	res.x = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;  // new x
	res.y = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;  // new y
	res.z = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;	// new z

	return res;
}

quaternion quatdivide(quaternion* _quat1, quaternion* _quat2) {
	quaternion qtemp = quatinv(_quat2);
	return quatmultiply(&qtemp, _quat1);
}

vector3D quatrotate(quaternion* q, vector3D* vect) {
	// http://www.cprogramming.com/tutorial/3d/quaternions.html
	// http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
	// http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
	// ^ or: http://webcache.googleusercontent.com/search?q=cache:xgJAp3bDNhQJ:content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation&hl=en&gl=us&strip=1

	// P_out = q * P_in * conj(q)
	// - P_out is the output vector
	// - q is the orientation quaternion
	// - P_in is the input vector (a*aReal)
	// - conj(q) is the conjugate of the orientation quaternion (q=[w,x,y,z], q*=[w,-x,-y,-z])

	quaternion p = { 0.0f, vect->x, vect->y, vect->z }, qtemp;

	// quaternion multiplication: q * p, stored back in p
	p = quatmultiply(q, &p);

	// quaternion multiplication: p * conj(q), stored back in p
	qtemp = quatconj(q);
	p = quatmultiply(&p, &qtemp);

	// p quaternion is now [0, x', y', z']
	vector3D result;
	result.x = p.x;
	result.y = p.y;
	result.z = p.z;
	return result;
}

// Quaternion to rotating angles (Euler angles)
// rotation order: yaw (Z), pitch (Y), roll (X)
angles3D quat2angle(quaternion* q) {
	angles3D angles;
	//float sinr_cosp, cosr_cosp, sinp, siny_cosp, cosy_cosp;
	float w, x, y, z;

	w = q->w;
	x = q->x;
	y = q->y;
	z = q->z;

	// roll (x-axis rotation)
	//sinr_cosp = 2 * (w * x + y * z);
	//cosr_cosp = 1 - 2 * (x * x + y * y);
	//angles.x = atan2(sinr_cosp, cosr_cosp);

	//// pitch (y-axis rotation)
	//sinp = 2 * (w * y - z * x);
	//if (sinp >= 1)			angles.y = M_PI_2; // use 90 degrees if out of range
	//else if (sinp <= -1)	angles.y = -M_PI_2; // use 90 degrees if out of range
	//else					angles.y = asin(sinp);

	//// yaw (z-axis rotation)
	//siny_cosp = 2 * (w * z + x * y);
	//cosy_cosp = 1 - 2 * (y * y + z * z);
	//angles.z = atan2(siny_cosp, cosy_cosp);

//	angles.x = atan2(2 * x * y - 2 * w * z, 2 * w * w + 2 * x * x - 1);   // psi
//	angles.y = -asin(2 * x * z + 2 * w * y);                              // theta
//	angles.z = atan2(2 * y * z - 2 * w * x, 2 * w * w + 2 * z * z - 1);   // phi

	angles.x = atan2f(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y)); // roll (X)
	angles.y = asinf(2.0f * (w * y - z * x));                           // pitch (Y)
	angles.z = atan2f(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z)); // yaw (Z)

	return angles;
}

vector3D quat2rotvec(quaternion q) {
	q = quatnormalize(&q);
    // Ensure shortest rotation
    if (q.w < 0) {
        q.w = -q.w;
        q.x = -q.x;
        q.y = -q.y;
        q.z = -q.z;
    }

    vector3D rotvec;
    float angle = 2.0f * acosf(q.w);
    float s = sqrtf(1.0f - q.w * q.w);  // sin(angle/2)

    if (s <= FLT_EPSILON) {
        // If angle is too small, return zero vector
        rotvec.x = 0.0f;
        rotvec.y = 0.0f;
        rotvec.z = 0.0f;
    } else {
        // Normalize axis and scale by angle
        rotvec.x = angle * (q.x / s);
        rotvec.y = angle * (q.y / s);
        rotvec.z = angle * (q.z / s);
    }

    return rotvec;
}

// rotation order: yaw (Z), pitch (Y), roll (X) in radians
quaternion angle2quat(angles3D* angles) {
	// Abbreviations for the various angular functions
	float cy = cosf(angles->z * 0.5f);
	float sy = sinf(angles->z * 0.5f);
	float cp = cosf(angles->y * 0.5f);
	float sp = sinf(angles->y * 0.5f);
	float cr = cosf(angles->x * 0.5f);
	float sr = sinf(angles->x * 0.5f);

	quaternion q;
	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;

	return q;
}

// Adds two quaternions together and the sum is the pointer to another quaternion, Sum = L + R
quaternion quatadd(quaternion *L, quaternion *R){
	quaternion res;
	res.w = L->w + R->w;
	res.x = L->x + R->x;
	res.y = L->y + R->y;
	res.z = L->z + R->z;
    return res;
}

// Subtracts two quaternions together and the sum is the pointer to another quaternion, sum = L - R
quaternion quatsub(quaternion *L, quaternion *R){
	quaternion res;
	res.w = L->w - R->w;
	res.x = L->x - R->x;
	res.y = L->y - R->y;
	res.z = L->z - R->z;
    return res;
}

// Compute error quaternion: q_err = conj(current) * desired
quaternion quat_error(quaternion *q_current, quaternion *q_desired) {
    quaternion q_conj = quatconj(q_current);
    return quatmultiply(q_desired, &q_conj);
}


// ================================================================
// ===						INTEGRALS				            ===
// ================================================================
float trapzY(float* y, size_t dim) {
	float sum = 0.0f, trapz;
	int h = 1;

	for (size_t i = 0; i < dim; ++i) {
		if (i == 0 || i == dim - 1) // for the first and last elements
			sum += y[i] / 2.0f;
		else
			sum += y[i]; // the rest of data
	}
	trapz = sum * h; // the result
	return trapz;
}

float trapzXY(float* x, float* y, size_t dim) {
	float _diff, sum;
	_diff = 0.0;
	sum = 0.0;
	for (size_t i = 1; i < dim; i++) {
		_diff = x[i] - x[i - 1];
		sum += _diff * (y[i - 1] + y[i]) / 2.0f;
	}
	return sum;
}

