#ifndef _IK_MAHONY_FILTER_H
#define _IK_MAHONY_FILTER_H

typedef struct 
{
    float ax, ay, az;
    float gx, gy, gz;
    float kp;   //proportional gain
    float ki;   //integral gain
    float frq;   //sample frequency in Hz
    float integralFBx;  // integral error terms scaled by Ki
    float integralFBy;
    float integralFBz;	

    float q0, q1, q2, q3;           // 四元数
    float rMat[3][3];               // 旋转矩阵
    float pitch, roll, yaw;         // 姿态角：俯仰角，滚转角，偏航角
}IK_MAHONY_FILTER;

void mahony_filter_init( IK_MAHONY_FILTER *filter, float kp, float ki, float fre );
void mahony_filter_input( IK_MAHONY_FILTER *filter, float gx, float gy, float gz, float ax, float ay, float az );
void mahony_filter_update( IK_MAHONY_FILTER *filter );
void mahony_filter_output( IK_MAHONY_FILTER *filter );

#endif // !_IK_MAHONY_FILTER_H

