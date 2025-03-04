/**
 * @file PID.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef ___PID_H
#define ___PID_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// #include <stdlib.h>
#include "kalman_filter.h"
#include "typedef.h"
#include <AddMath.h>
#include <math.h>

/**********PID�������ݽӿ�************/
typedef struct incrementalpid_t
{
    float Target;         //�趨Ŀ��ֵ
    float Measured;       //����ֵ
    float err;            //����ƫ��ֵ
    float err_last;       //��һ��ƫ��
    float err_beforeLast; //���ϴ�ƫ��
    float Kp;
    float Ki;
    float Kd; //Kp, Ki, Kd����ϵ��
    float p_out;
    float i_out;
    float d_out;            //���������ֵ
    float pwm;              //pwm���
    uint32_t MaxOutput;     //����޷�
    uint32_t IntegralLimit; //�����޷�
    float (*Incremental_PID)(struct incrementalpid_t *pid_t, float target, float measured);
} incrementalpid_t;

typedef struct positionpid_t
{
    float Target;     //�趨Ŀ��ֵ
    float Measured;   //����ֵ
    float err;        //����ƫ��ֵ
    float err_last;   //��һ��ƫ��
    float err_change; //���仯��
    float Kp;
    float Ki;
    float Kd; //Kp, Ki, Kd����ϵ��
    float p_out;
    float i_out;
    float d_out;               //���������ֵ
    float pwm;                 //pwm���
    float MaxOutput;           //����޷�
    float Integral_Separation; //���ַ�����ֵ
    float IntegralLimit;       //�����޷�
    float (*Position_PID)(struct positionpid_t *pid_t, float target, float measured);
} positionpid_t;

extern float Incremental_PID(incrementalpid_t *pid_t, float target, float measured);
extern float Position_PID(positionpid_t *pid_t, float target, float measured);
extern float ClassisTwister_PID(positionpid_t *pid_t, float target, float measured);
extern void Incremental_PIDInit(incrementalpid_t *pid_t, float Kp, float Kd, float Ki, uint32_t MaxOutput, uint32_t IntegralLimit);
extern void Position_PIDInit(positionpid_t *pid_t, float Kp, float Kd, float Ki, float MaxOutput, float IntegralLimit, float Integral_Separation);
extern float Vision_AutoTracPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_IMUYAWOPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_IMUYAWIPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_IMUPITCHOPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_IMUPITCHIPID(positionpid_t *pid_t, float target, float measured);
extern float ClassisFollow_PID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_VisionIMUYAWOPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_VisionIMUYAWIPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_VisionIMUPITCHOPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_VisionIMUPITCHIPID(positionpid_t *pid_t, float target, float measured);
extern void Clear_PositionPIDData(positionpid_t *pid_t);
extern void Clear_IncrementalPIDData(incrementalpid_t *pid_t);
extern float Cloud_YAWOPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_YAWIPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_PITCHOPID(positionpid_t *pid_t, float target, float measured);
extern float Cloud_PITCHIPID(positionpid_t *pid_t, float target, float measured);
extern float Vision_YAWIPID(positionpid_t *pid_t, float target, float measured);
extern float Vision_YAWOPID(positionpid_t *pid_t, float target, float measured);
extern float Vision_PITCHOPID(positionpid_t *pid_t, float target, float measured);
extern float Vision_PITCHIPID(positionpid_t *pid_t, float target, float measured);
extern extKalman_t Cloud_YAWODKalman;
extern extKalman_t Cloud_PITCHODKalman;

#endif
