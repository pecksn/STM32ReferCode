/**
 * @file Control_Vision.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __Control_Vision
#define __Control_Vision

#include "CRC.h"
#include "Cloud_control.h"
#include "PID.h"
#include "RM_JudgeSystem.h"
#include "Robot_Config.h"
#include "Robot_control.h"
#include "dma.h"
#include "gpio.h"
#include "typedef.h"
#include "usart.h"
#include <Math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h> //�����˵�Ĭ�������ļ���

typedef struct
{
    float x;
    float y;
} XY_t;

#if Vision_Version == Vision_Oldversion
//�Ӿ���ʶ������ϵ��С��
#define VisionPage_Width 1280
#define VisionPage_Height 800

typedef struct
{
    struct
    {
        char Start_Tag;
        int8_t mode; //�����Ƿ����(�Ƿ�ʶ��װ�װ�)
        int8_t mode_select;
        int8_t whether_Fire; //�Ƿ�������(�����Զ����)
        int8_t _yaw;         //���� 0��1��
        int16_t x;           //yaw�ᣬΪ��Ļ�����
        int8_t _pitch;       //���� 0��1��
        int16_t y;           //pitch ��
        int16_t depth;       //���
        int16_t crc;
        char End_Tag;
    } RawData; // �Ӿ���Э�� ����һ֡�����ݽṹ��

    XY_t Target_Offset;  //�����ջص��������ݣ����㵱ǰ����Ŀ��ƫ��ֵ
    XY_t Gravity_Offset; //����ǹ�ڶ�׼Ŀ���һ���ܴ��У�����Ӱ�죩����������ǹ����Ҫ΢�����ٲ��ܸպû��С�==������ƫ��ֵ
    XY_t Final_Offset;   //���պϳɸ�PID�����ƫ��ֵ��

    XY_t Target_LastOffset; //���� �Ӿ����ص���һ�ε�Ŀ��ƫ��ֵ
    XY_t ErrorChange_Rate;  //�����Ӿ������������仯��(��)

    XY_t SpeedGain;    //ʹ���˶��ٶ������档
    XY_t LpfAttFactor; //�˲�ϵ��

    uint8_t InfoUpdateFlag;      //��Ϣ��ȡ���±�־
    uint16_t InfoUpdateFrame;    //֡��
    uint8_t OffLineFlag;         //�豸���߱�־
    uint32_t FPS;                //�Ӿ�������֡��
#define Vision_BuffSIZE (20 + 2) //�Ӿ����ݻ���������
} VisionData_t;

#elif Vision_Version == Vision_Newversion
typedef struct
{
    struct
    {
        union
        {
            struct
            {
                char Start_Tag;
                uint8_t mode; //�����Ƿ����(�Ƿ�ʶ��װ�װ�)
                // uint8_t mode_select;
                uint8_t whether_Fire; //�Ƿ�������(�����Զ����)
                uint8_t _yaw;         //���� 0��1��
                uint8_t x_L;          //yaw�߰�λ
                uint8_t x_H;          //yaw�Ͱ�λ
                uint8_t _pitch;       //���� 0��1��
                uint8_t y_L;          //pitch�߰�λ
                uint8_t y_H;          //pitch�߰�λ
                uint8_t depth_L;      //��ȵͰ�λ
                uint8_t depth_H;      //��ȸ߰�λ
                uint8_t crc;
                char End_Tag;
            };
            uint8_t VisionRawData[13];
        };

        float x;
        float y;
        float depth;
        float yaw_;
    } RawData; // �Ӿ���Э�� ����һ֡�����ݽṹ��

    XY_t Gravity_Offset; //����ǹ�ڶ�׼Ŀ���һ���ܴ��У�����Ӱ�죩����������ǹ����Ҫ΢�����ٲ��ܸպû��С�==������ƫ��ֵ
    XY_t Final_Offset;   //���պϳɸ�PID�����ƫ��ֵ��

    XY_t ErrorChange_Rate; //�����Ӿ������������仯��(��)

    XY_t SpeedGain;    //ʹ���˶��ٶ������档
    XY_t LpfAttFactor; //�˲�ϵ��

    uint8_t InfoUpdateFlag;   //��Ϣ��ȡ���±�־
    uint16_t InfoUpdateFrame; //֡��
    uint8_t OffLineFlag;      //�豸���߱�־
    uint32_t FPS;             //�Ӿ�������֡��
} VisionData_t;

#define Vision_BuffSIZE (13 + 2) //�Ӿ����ݻ���������
#endif

#define VisionExportDataGroundInit \
    {                              \
        {0},                       \
            {0},                   \
            {0},                   \
            0,                     \
            0,                     \
            &VisionSend_IMU,       \
    }

#define Control_Vision_FUNGroundInit   \
    {                                  \
        &Vision_Init,                  \
            &Update_VisionTarget,      \
            &Vision_processing,        \
            &Vision_ID_Init,           \
            &Vision_Handler,           \
            &Vision_USART_Receive_DMA, \
            &Vision_SendBufFunction,   \
            &GetVisionDiscMode,        \
            &GetVisionHitMode,         \
            &Check_Vision,             \
            &Vision_CP,                \
    }

typedef struct
{
    union
    {
        struct
        {
            float YawAngle_Error;   //������YAW�ǶȲ�
            float PitchAngle_Error; //������Pitch�ǶȲ�
        };
        uint8_t Angle_Error_Data[8];
    } VisionSend_t;

    int Gyro_z;           //�����Ǽ��ٶ�С�������λ
    uint8_t Gyro_z_Hight; //�����Ǽ��ٶ�С�������λ�߰�λ
    uint8_t Gyro_z_low;   //�����Ǽ��ٶ�С�������λ�Ͱ�λ

} VisionSend_IMU_t;

typedef struct
{
    XY_t FinalOffset;        //���ո�����̨pid �����ƫ��ֵ��
    XY_t FinalOffset_Last;   //���ո�����̨pid �����ƫ��ֵ��
    XY_t FinalSpeed;         //���ո�����̨pid �����ƫ��ֵ������ٶȡ�
    bool AbleToFire;         //��ʾĿ���Ѿ���׼������ֱ�����
    float FinalOffset_depth; //�����Ӿ����
    VisionSend_IMU_t *VisionSend_IMU;

} VisionExportData_t;

typedef struct
{
    void (*Vision_Init)(void);
    void (*Update_VisionTarget)(void);
    void (*Vision_processing)(void);
    void (*Vision_ID_Init)(void);
    void (*Vision_Handler)(UART_HandleTypeDef *huart);
    void (*Vision_USART_Receive_DMA)(UART_HandleTypeDef *huartx);
    void (*Vision_SendBufFunction)(float *angle, float *Gyro);
    uint8_t (*GetVisionDiscMode)(void);
    uint8_t (*GetVisionHitMode)(void);
    void (*Check_Vision)(void);
    void (*Vision_CP)(float CPData);
} Control_Vision_FUN_t;

extern uint8_t Vision_DataBuff[Vision_BuffSIZE];
extern VisionExportData_t VisionExportData;
extern VisionData_t VisionData;
extern Control_Vision_FUN_t Control_Vision_FUN;
extern VisionSend_IMU_t VisionSend_IMU;
extern WorldTime_RxTypedef VisionKF_TIME;

#ifdef Enable_Vision_Test
extern uint8_t Vision_SendBuf[5][5];
void Vision_I_T_Set(uint8_t ID, uint8_t Type);
#endif

#endif
