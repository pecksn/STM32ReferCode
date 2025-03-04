/**
 * @file Cloud_control.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __CLOUD_CONTROL_H
#define __CLOUD_CONTROL_H

#include "PID.h"
#include "Robot_Config.h"
#include "SpeedRamp.h"
#include "kalman_filter.h"
#include "typedef.h"
#include <AddMath.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define Cloud_Mode_Normal 0
#define Cloud_Mode_WithIMU 1


//���°�װ��������ô���ʱ��Ҫ���²�����Щֵ��toalAngle�����������˶���

#if Infantry_Year == Infantry_2020
#define Cloud_Yaw_Center 4060
#define Cloud_Yaw_ReverseCenter 7809
#elif Infantry_Year == Infantry_2021
/***********************/
#if Who_Infantry == ZJ_Infantry
#define Cloud_Yaw_Center 3030
#define Cloud_Yaw_ReverseCenter 7126
#elif Who_Infantry == ZF_Infantry
#define Cloud_Yaw_Center 4800
#define Cloud_Yaw_ReverseCenter 705

#endif
/***********************/

#endif

#define Center(Min, Max) (((Max) + (Min)) / 2)

#define No_Compensate 0
#define Kalman 1
#define Fourth_Ring 2

#define VisionYaw 0
#define VisionPitch 1

/****************���PID*******************/
#define M6020s_YawOPIDInit  \
    {                       \
        0,                  \
            0,              \
            0,              \
            0,              \
            0,              \
            0.3f,           \
            0.0f,           \
            18.0f,          \
            0,              \
            0,              \
            0,              \
            0,              \
            100,            \
            0,              \
            0,              \
            &Cloud_YAWOPID, \
    }

#define M6020s_YawIPIDInit   \
    {                        \
        0,                   \
            0,               \
            0,               \
            0,               \
            0,               \
            300.0f,          \
            0.07f,           \
            0.0f,            \
            0,               \
            0,               \
            0,               \
            0,               \
            M6020_MaxOutput, \
            30,              \
            5000,            \
            &Cloud_YAWIPID,  \
    }

#define M6020s_PitchOPIDInit  \
    {                         \
        0,                    \
            0,                \
            0,                \
            0,                \
            0,                \
            0.35f,            \
            0.0f,             \
            4.5f,             \
            0,                \
            0,                \
            0,                \
            0,                \
            100,              \
            0,                \
            3000,             \
            &Cloud_PITCHOPID, \
    }

#define M6020s_PitchIPIDInit  \
    {                         \
        0,                    \
            0,                \
            0,                \
            0,                \
            0,                \
            350.0f,           \
            2.5f,             \
            0.0f,             \
            0,                \
            0,                \
            0,                \
            0,                \
            M6020_MaxOutput,  \
            30,               \
            4000,             \
            &Cloud_PITCHIPID, \
    }
/****************���PID END*******************/

/***************���+�Ӿ�PID****************/
#define VisionM6020s_YawOPIDInit \
    {                            \
        0,                       \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            0.3f,                \
            0.0f,                \
            18.0f,               \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            100,                 \
            0,                   \
            0,                   \
            &Vision_YAWOPID,     \
    }

#define VisionM6020s_YawIPIDInit \
    {                            \
        0,                       \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            300.0f,              \
            0.07f,               \
            0.0f,                \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            M6020_MaxOutput,     \
            30,                  \
            5000,                \
            &Vision_YAWIPID,     \
    }

#define VisionM6020s_PitchOPIDInit \
    {                              \
        0,                         \
            0,                     \
            0,                     \
            0,                     \
            0,                     \
            0.35f,                 \
            0.0f,                  \
            4.5f,                  \
            0,                     \
            0,                     \
            0,                     \
            0,                     \
            100,                   \
            0,                     \
            3000,                  \
            &Vision_PITCHOPID,     \
    }

#define VisionM6020s_PitchIPIDInit \
    {                              \
        0,                         \
            0,                     \
            0,                     \
            0,                     \
            0,                     \
            350.0f,                \
            2.5f,                  \
            0.0f,                  \
            0,                     \
            0,                     \
            0,                     \
            0,                     \
            M6020_MaxOutput,       \
            30,                    \
            4000,                  \
            &Vision_PITCHIPID,     \
    }
/***************���+�Ӿ�PID END****************/

/***************������PID**************/
#define YawAttitude_PIDInit    \
    {                          \
        0,                     \
            0,                 \
            0,                 \
            0,                 \
            0,                 \
            -8.5f,             \
            0.0f,              \
            0.0f,              \
            0,                 \
            0,                 \
            0,                 \
            0,                 \
            18000,             \
            0,                 \
            0,                 \
            &Cloud_IMUYAWOPID, \
    }

#define YawSpeed_PIDInit       \
    {                          \
        0,                     \
            0,                 \
            0,                 \
            0,                 \
            0,                 \
            50.0f,             \
            0.3f,              \
            0.0f,              \
            0,                 \
            0,                 \
            0,                 \
            0,                 \
            M6020_MaxOutput,   \
            1000,              \
            15000,             \
            &Cloud_IMUYAWIPID, \
    }

#define PitchAttitude_PIDInit    \
    {                            \
        0,                       \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            18.0f,               \
            0.0f,                \
            0.0f,                \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            10000,               \
            0,                   \
            3000,                \
            &Cloud_IMUPITCHOPID, \
    }

#define PitchSpeed_PIDInit       \
    {                            \
        0,                       \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            9.1f,                \
            0.09f,               \
            0.0f,                \
            0,                   \
            0,                   \
            0,                   \
            0,                   \
            M6020_MaxOutput,     \
            4500,                \
            9000,                \
            &Cloud_IMUPITCHIPID, \
    }
/***************������PID END**************/

/***************������+�Ӿ�PID***************/
#define VisionYawAttitude_PIDInit    \
    {                                \
        0,                           \
            0,                       \
            0,                       \
            0,                       \
            0,                       \
            -8.5f,                   \
            0.0f,                    \
            0.0f,                    \
            0,                       \
            0,                       \
            0,                       \
            0,                       \
            18000,                   \
            0,                       \
            0,                       \
            &Cloud_VisionIMUYAWOPID, \
    }

#define VisionYawSpeed_PIDInit       \
    {                                \
        0,                           \
            0,                       \
            0,                       \
            0,                       \
            0,                       \
            60.0f,                   \
            0.5f,                    \
            0.0f,                    \
            0,                       \
            0,                       \
            0,                       \
            0,                       \
            M6020_MaxOutput,         \
            1000,                    \
            15000,                   \
            &Cloud_VisionIMUYAWIPID, \
    }

#define VisionPitchAttitude_PIDInit    \
    {                                  \
        0,                             \
            0,                         \
            0,                         \
            0,                         \
            0,                         \
            12.0f,                     \
            0.0f,                      \
            0.0f,                      \
            0,                         \
            0,                         \
            0,                         \
            0,                         \
            10000,                     \
            0,                         \
            3000,                      \
            &Cloud_VisionIMUPITCHOPID, \
    }

#define VisionPitchSpeed_PIDInit       \
    {                                  \
        0,                             \
            0,                         \
            0,                         \
            0,                         \
            0,                         \
            9.1f,                      \
            0.09f,                     \
            0.0f,                      \
            0,                         \
            0,                         \
            0,                         \
            0,                         \
            M6020_MaxOutput,           \
            4500,                      \
            9000,                      \
            &Cloud_VisionIMUPITCHIPID, \
    }
/***************������+�Ӿ�PID END***************/

/******************�Ӿ�˫��PID��ʼ��*****************/
#define Vision_AnglePid_Yaw_PIDInit \
    {                               \
        0,                          \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            10,                     \
            12,                     \
            13,                     \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            28888,                  \
            0,                      \
            8000,                   \
            &Position_PID,          \
    }
#define Vision_SpeedPid_Yaw_PIDInit \
    {                               \
        0,                          \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            0,                      \
            28888,                  \
            0,                      \
            8000,                   \
            &Position_PID,          \
    }
#define Vision_AnglePid_Pitch_PIDInit \
    {                                 \
        0,                            \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            28888,                    \
            0,                        \
            8000,                     \
            &Position_PID,            \
    }
#define Vision_SpeedPid_Pitch_PIDInit \
    {                                 \
        0,                            \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            0,                        \
            28888,                    \
            0,                        \
            8000,                     \
            &Position_PID,            \
    }
/******************�Ӿ�˫��PID��ʼ�� END*****************/

#define TestSpeed_PIDInit    \
    {                        \
        0,                   \
            0,               \
            0,               \
            0,               \
            0,               \
            50.0f,           \
            0.2f,            \
            0.0f,            \
            0,               \
            0,               \
            0,               \
            0,               \
            M6020_MaxOutput, \
            0,               \
            3000,            \
            &Position_PID,   \
    }

#define Cloud_FUNGroundInit               \
    {                                     \
        &Cloud_Init,                      \
            &Cloud_processing_WithIMU,    \
            &Cloud_processing_WithoutIMU, \
            &Cloud_getYawAngleWithCenter, \
            &IMUData_chance,              \
    }
/*********************���������ݳ�ʼ��******************/
#define Kalman_YawDataInit      \
    {                           \
        0,                      \
            0,                  \
            0,                  \
            0,                  \
            {850, 0.1, 10, 50}, \
    }

#define Kalman_PitchDataInit   \
    {                          \
        0,                     \
            0,                 \
            0,                 \
            0,                 \
            {0, 0.03, 5, 250}, \
    }

#define KalmanRampInit           \
    {                            \
        0,                       \
            3,                   \
            -PredictAngle_limit, \
            PredictAngle_limit,  \
    }

/*********************���������ݳ�ʼ��END******************/
typedef enum
{
    A_IMU,
    C_IMU
} IMUState_t;

typedef struct
{
    float *IMUAngle_Yaw;
    float *IMUAngle_Pitch;
    float *IMUSpeed_Yaw;
    float *IMUSpeed_Pitch;
    IMUState_t IMUState;
} RealData_t;

/* ��̨�˶� */
typedef struct
{
    float YawLpfAttFactor;   //yaw����̨�˲�ϵ��
    float PitchLpfAttFactor; //pitch���˲�ϵ����

    //���λ��angle��������
    float targetYawRaw; //��̨Ŀ��yaw��ԭʼ����
    float last_targetYawRaw;
    float targetPitchRaw;   //��̨Ŀ��pitch��ԭʼ����
    float targetYawLPF;     //��̨yaw���˲�������
    float targetPitchLPF;   //��̨pitch���˲�������
    int Reversefollow_Date; //���̸������Ľǵ���ֵ

    //��������������
    float IMUtargetYawRaw;      //��̨Ŀ��yaw��ԭʼ����
    float IMUtargetPitchRaw;    //��̨Ŀ��pitch��ԭʼ����
    float IMUtargetYawLPF;      //��̨yaw���˲�������
    float last_IMUtargetYawLPF; //��̨yaw���˲�������
    float IMUtargetPitchLPF;    //��̨pitch���˲�������

    float IMUYawAngleMax;   //��̨IMU���Ƕ�(��)
    float IMUYawAngleMin;   //��̨IMU��С�Ƕ�(��)
    float IMUPitchAngleMax; //��̨IMU���Ƕ�(��)
    float IMUPitchAngleMin; //��̨IMU��С�Ƕ�(��)

    RealData_t RealData;

    uint8_t Mode; //��̨����ģʽ
} Cloud_t;

typedef struct
{
    float Speed_Gain; //�ٶ�����
    positionpid_t *Vision_AnglePID;
    positionpid_t *Vision_SpeedPID;
} VisionFourth_Ring_t;

typedef struct //�Ӿ�Ŀ���ٶȲ���
{
    int delay_cnt; //����������֡Ŀ�겻�����ʱ��,�����ж��ٶ��Ƿ�Ϊ0
    int freq;
    int last_time;         //�ϴ��ܵ�Ŀ��Ƕȵ�ʱ��
    float last_position;   //�ϸ�Ŀ��Ƕ�
    float speed;           //�ٶ�
    float last_speed;      //�ϴ��ٶ�
    float processed_speed; //�ٶȼ�����
} speed_calc_data_t;

typedef struct
{
    void (*Cloud_Init)(void);
    void (*Cloud_processing_WithIMU)(float delta_yaw, float delta_pitch);
    void (*Cloud_processing_WithoutIMU)(float delta_yaw, float delta_pitch);
    float (*Cloud_getYawAngleWithCenter)(void);
    void (*IMUData_chance)(void);
} Cloud_FUN_t;

typedef struct
{
    float Test_Speed;
    float Test_angle;
} Cloud_ParameterDeBug_t;

typedef struct
{
    Kalman_Data_t *Kalman_Data;
    VisionFourth_Ring_t *VisionFourth_Ring;
} All_VisionCompensate_t;

extern Cloud_t Cloud;
extern GY_IMUExport_t IMUExport_Cloud;
extern float Cloud_Chassis_SpeedOffset_current_;
extern Cloud_FUN_t Cloud_FUN;
extern float Kalman_VisionAngle;

extern VisionFourth_Ring_t VisionFourth_Ring_Yaw;
extern VisionFourth_Ring_t VisionFourth_Ring_Pitch;
extern Cloud_ParameterDeBug_t Cloud_ParameterDeBug[2];
extern positionpid_t YawAttitude_PID;   //��̨yaw����̬pid
extern positionpid_t YawSpeed_PID;      //��̨yaw���ٶ�pid
extern positionpid_t PitchAttitude_PID; //��̨roll����̬pid
extern positionpid_t PitchSpeed_PID;    //��̨roll����̬pid
extern positionpid_t M6020s_YawOPID;    //YAW����̨����⻷
extern positionpid_t M6020s_YawIPID;    //YAW����̨����ڻ�
extern positionpid_t M6020s_PitchOPID;  //PITCH����̨����⻷
extern positionpid_t M6020s_PitchIPID;  //PITCH����̨����ڻ�
extern positionpid_t VisionYawAttitude_PID;
extern positionpid_t VisionYawSpeed_PID;
extern All_VisionCompensate_t All_VisionComYaw;
extern extKalman_t Cloud_PitchGyroAngle_Error_Kalman;
extern kalman_filter_t yaw_kalman_filter;
extern kalman_filter_t pitch_kalman_filter;

#endif /* __CLOUD_CONTROL_H */
