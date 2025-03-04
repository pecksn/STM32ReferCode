/**
 * @file M6020_Motor.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __M6020_MOTOR_H
#define __M6020_MOTOR_H

#include "can.h"
#include "typedef.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define M6020_READID_START 0x205 //��IDΪ1ʱ�ı���ID
#define M6020_READID_END 0x206
#define M6020_SENDID 0x1FF //1~4�ĵ����0x2FFΪ5~7

#define M6020_CurrentRatio 0f //���ⶨ
#define M6020_MaxOutput 30000 //���͸������������ֵ
#define M6020_mAngle 8191     //6020�Ļ�е�Ƕ����ֵ0~8191��MachineAngle

#define M6020_mAngleRatio 22.7527f //��е�Ƕ�����ʵ�Ƕȵı���

#define M6020_getRoundAngle(rAngle) rAngle / M6020_mAngleRatio //��е�Ƕ�����ʵ�Ƕȵı���

#define M6020_FunGroundInit        \
    {                              \
        &M6020_getInfo,            \
            &M6020_setTargetAngle, \
            &M6020_Reset,          \
            &Check_M6020,          \
    }

typedef struct
{
    uint16_t realAngle;  //�������Ļ�е�Ƕ�
    int16_t realSpeed;   //���������ٶ�
    int16_t realCurrent; //��������ʵ��ת�ص���
    uint8_t temperture;  //�������ĵ���¶�

    int16_t targetSpeed; //Ŀ���ٶ�
    int32_t targetAngle; //Ŀ��Ƕ�
    uint16_t lastAngle;  //�ϴεĽǶ�
    float totalAngle;  //�ۻ��ܹ��Ƕ�
    int16_t turnCount;   //ת����Ȧ��

    int16_t outCurrent; //�������

    uint8_t InfoUpdateFlag;   //��Ϣ��ȡ���±�־
    uint16_t InfoUpdateFrame; //֡��
    uint8_t OffLineFlag;      //�豸���߱�־
} M6020s_t;

typedef enum
{
    //��Ҫע���뱨�Ľ��ܺ�������Ӧ����
    //M6020_PITCH_Right = 0,
    M6020_PITCH = 0,
    M6020_YAW,
} M6020Name_e;

typedef struct
{
    void (*M6020_getInfo)(Can_Export_Data_t RxMessage);
    void (*M6020_setTargetAngle)(M6020s_t *M6020, int32_t angle);
    void (*M6020_Reset)(M6020s_t *m6020);
    void (*Check_M6020)(void);
} M6020_Fun_t;

extern M6020s_t M6020s_Yaw;   //IDΪ1
extern M6020s_t M6020s_Pitch; //2
extern M6020_Fun_t M6020_Fun;

#endif /* __M3508_MOTOR_H */
