/*
 * File:          typedef.h
 * Date:       2021/3/29
 * Description:   �ⲿ�ӿ�
 * Author:  Miraggio
 * Modifications:
 */
#ifndef __TYPEDEFS_H
/**
 * @file typedef.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#define __TYPEDEFS_H

#include "can.h"
#include <stdint.h>

#pragma anon_unions

/*************CAN�������ݽӿ�************/
typedef struct
{
    CAN_RxHeaderTypeDef CAN_RxHeader;
    uint8_t CANx_Export_RxMessage[8];
} Can_Export_Data_t;

/*************CAN�������ݽӿ�************/
typedef struct
{
    CAN_HandleTypeDef *Canx;
    CAN_TxHeaderTypeDef CAN_TxHeader;
    uint8_t CANx_Send_RxMessage[8];
} Can_Send_Data_t;

/**********�����Ƕ������ݽӿ�************/
typedef struct
{
    struct
    {
        float x; //������pitch��ķ�������
        float y; //������Y�᷽������
        float z; //������yaw���ת���ٶ�
    } Finally_Gyro;
    struct
    {
        float Roll;  //ROLL�᷽�򣬵�ǰ�ĽǶ�ֵ
        float Pitch; //PITCH�᷽��
        float Yaw;   //YAW�᷽��
    } Finally_Eular;

    float Finally_totalPitch;
    float Target_pitch; //��������ֵ
    uint8_t OffLineFlag;
    uint32_t IMUExport_FPS;
} GY_IMUExport_t;

/**********ϵͳʱ��������ݽӿ�************/
typedef struct
{
    uint32_t WorldTime;      //����ʱ��
    uint32_t Last_WorldTime; //��һ������ʱ��
} WorldTime_RxTypedef;
void Get_FPS(WorldTime_RxTypedef *time, uint32_t *FPS);

/************��ʱȫ�ַ��ʱ���***************/
extern uint32_t Robot_FPS;
extern uint32_t GetData_FPS;
extern uint8_t RoboInit_Complete;

#endif /* __TYPEDEFS_H */
