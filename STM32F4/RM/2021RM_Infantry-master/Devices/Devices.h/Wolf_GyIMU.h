/**
 * @file Wolf_GyIMU.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __GY_IMU_H
#define __GY_IMU_H

#include "typedef.h"
#include <AddMath.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GY_IMU_PACKAGE_LENGTH 18
#define GY_IMU_READID_START 0x413 //can1����̨������
#define GY_IMU_READID_END 0x414   //can2�ĵ���������

#define Wolf_GyIMU_FunGroundInit \
    {                            \
        &GY6050_Init,            \
            &GY6050_getInfo,     \
            &IMU_processing,     \
            &Check_GyIMU,        \
            &GY6050_PitErr,      \
    }

typedef struct
{
    union
    {
        struct
        {
            uint16_t yaw;
            int16_t gyro_z;
            int16_t pitch;
            int16_t gyro_x;
        };
        uint8_t dataBuff[8];
    } bno055_data;

    struct
    {
        float x; //������pitch��ķ�������
        float y; //������Y�᷽������
        float z; //������yaw���ת���ٶ�
    } Gyro;
    struct
    {
        float Roll;  //ROLL�᷽�򣬵�ǰ�ĽǶ�ֵ
        float Pitch; //PITCH�᷽��
        float Yaw;   //YAW�᷽��
    } Eular;

    struct
    {
        float lastPitch; //��һ��YAW������
        float totalPitch;
        float errorPitch;   //Ŀ��Ƕȵ����ֵ
        float Target_pitch; //��������ֵ
        int16_t turnCount;
    } Calculating_Data;

    uint8_t DevStatus;
    uint8_t InfoUpdateFlag;   //��Ϣ��ȡ���±�־
    uint16_t InfoUpdateFrame; //֡��
    uint8_t OffLineFlag;
    uint32_t IMU_FPS;
} GY_IMU_t;

typedef struct
{
    void (*GY6050_Init)(GY_IMUExport_t *IMUExport_Cloud, GY_IMUExport_t *IMUExport_Chassis);
    void (*GY6050_getInfo)(Can_Export_Data_t RxMessage);
    void (*IMU_processing)(GY_IMUExport_t *IMUExport_Cloud, GY_IMUExport_t *IMUExport_Chassis);
    void (*Check_GyIMU)(void);
    float (*GY6050_PitErr)(void);
} Wolf_GyIMU_Fun_t;

extern Wolf_GyIMU_Fun_t Wolf_GyIMU_Fun;

#endif
