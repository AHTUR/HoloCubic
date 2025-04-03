#include "ik_mahony_filter.h"
#include <math.h>

#define DEG2RAD 0.017453
#define RAD2DEG 57.295779

static float invSqrt(float x) 
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

void mahony_filter_init( IK_MAHONY_FILTER *filter, float kp, float ki, float fre )
{
    filter->kp = kp;
    filter->ki = ki;
    filter->frq = fre;
    filter->q0 = 1.0f;
    filter->q1 = 0.0f;
    filter->q2 = 0.0f;
    filter->q3 = 0.0f;
    filter->integralFBx = 0.0f;
    filter->integralFBy = 0.0f;
    filter->integralFBz = 0.0f;
    return;
}

void mahony_filter_input( IK_MAHONY_FILTER *filter, float gx, float gy, float gz, float ax, float ay, float az )
{
    filter->ax = ax;
    filter->ay = ay;
    filter->az = az;
    filter->gx = gx;
    filter->gy = gy;
    filter->gz = gz;
    return;
}

/*计算旋转矩阵*/
static void RotationMatrix_update( IK_MAHONY_FILTER *filter )
{
    float q1q1 = filter->q1 * filter->q1;
    float q2q2 = filter->q2 * filter->q2;
    float q3q3 = filter->q3 * filter->q3;

    float q0q1 = filter->q0 * filter->q1;
    float q0q2 = filter->q0 * filter->q2;
    float q0q3 = filter->q0 * filter->q3;
    float q1q2 = filter->q1 * filter->q2;
    float q1q3 = filter->q1 * filter->q3;
    float q2q3 = filter->q2 * filter->q3;

    filter->rMat[0][0] = 1.0f - 2.0f * q2q2 - 2.0f * q3q3;
    filter->rMat[0][1] = 2.0f * (q1q2 + -q0q3);
    filter->rMat[0][2] = 2.0f * (q1q3 - -q0q2);

    filter->rMat[1][0] = 2.0f * (q1q2 - -q0q3);
    filter->rMat[1][1] = 1.0f - 2.0f * q1q1 - 2.0f * q3q3;
    filter->rMat[1][2] = 2.0f * (q2q3 + -q0q1);

    filter->rMat[2][0] = 2.0f * (q1q3 + -q0q2);
    filter->rMat[2][1] = 2.0f * (q2q3 - -q0q1);
    filter->rMat[2][2] = 1.0f - 2.0f * q1q1 - 2.0f * q2q2;
    return;
}

void mahony_filter_update( IK_MAHONY_FILTER *filter )
{
    float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((filter->ax == 0.0f) && (filter->ay == 0.0f) && (filter->az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(filter->ax * filter->ax + filter->ay * filter->ay + filter->az * filter->az);
		filter->ax *= recipNorm;
		filter->ay *= recipNorm;
		filter->az *= recipNorm;        

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = filter->q1 * filter->q3 - filter->q0 * filter->q2;
		halfvy = filter->q0 * filter->q1 + filter->q2 * filter->q3;
		halfvz = filter->q0 * filter->q0 - 0.5f + filter->q3 * filter->q3;
	
		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (filter->ay * halfvz - filter->az * halfvy);
		halfey = (filter->az * halfvx - filter->ax * halfvz);
		halfez = (filter->ax * halfvy - filter->ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(filter->ki > 0.0f) {
			filter->integralFBx += filter->ki * halfex * (1.0f / filter->frq);	// integral error scaled by Ki
			filter->integralFBy += filter->ki * halfey * (1.0f / filter->frq);
			filter->integralFBz += filter->ki * halfez * (1.0f / filter->frq);
			filter->gx += filter->integralFBx;	// apply integral feedback
			filter->gy += filter->integralFBy;
			filter->gz += filter->integralFBz;
		}
		else {
			filter->integralFBx = 0.0f;	// prevent integral windup
			filter->integralFBy = 0.0f;
			filter->integralFBz = 0.0f;
		}

		// Apply proportional feedback
		filter->gx += filter->kp * halfex;
		filter->gy += filter->kp * halfey;
		filter->gz += filter->kp * halfez;
	}
	
	// Integrate rate of change of quaternion
	filter->gx *= (0.5f * (1.0f / filter->frq));		// pre-multiply common factors
	filter->gy *= (0.5f * (1.0f / filter->frq));
	filter->gz *= (0.5f * (1.0f / filter->frq));
	qa = filter->q0;
	qb = filter->q1;
	qc = filter->q2;
	filter->q0 += (-qb * filter->gx - qc * filter->gy - filter->q3 * filter->gz);
	filter->q1 += (qa * filter->gx + qc * filter->gz - filter->q3 * filter->gy);
	filter->q2 += (qa * filter->gy - qb * filter->gz + filter->q3 * filter->gx);
	filter->q3 += (qa * filter->gz + qb * filter->gy - qc * filter->gx); 
	
	// Normalise quaternion
	recipNorm = invSqrt(filter->q0 * filter->q0 + filter->q1 * filter->q1 + filter->q2 * filter->q2 + filter->q3 * filter->q3);
	filter->q0 *= recipNorm;
	filter->q1 *= recipNorm;
	filter->q2 *= recipNorm;
	filter->q3 *= recipNorm;
    RotationMatrix_update( filter );
    return;
}

void mahony_filter_output( IK_MAHONY_FILTER *filter )
{
    /*计算roll pitch yaw 欧拉角*/
    filter->pitch = -asinf(filter->rMat[2][0]) * RAD2DEG; 
    filter->roll = atan2f(filter->rMat[2][1], filter->rMat[2][2]) * RAD2DEG;
    filter->yaw = atan2f(filter->rMat[1][0], filter->rMat[0][0]) * RAD2DEG;

	// filter->roll =  atan2(2*filter->q2*filter->q3 + 2*filter->q0*filter->q1, -2*filter->q1*filter->q1 - 2*filter->q2*filter->q2 + 1)*RAD2DEG;
    // filter->pitch =  asin(2*filter->q1*filter->q3 - 2*filter->q0*filter->q2)*RAD2DEG;
    // filter->yaw  =  -atan2(2*filter->q1*filter->q2 + 2*filter->q0*filter->q3, -2*filter->q2*filter->q2 -2*filter->q3*filter->q3 + 1)*RAD2DEG;
}
