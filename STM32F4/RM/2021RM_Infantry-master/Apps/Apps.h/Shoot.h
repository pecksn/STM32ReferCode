/**
 * @file Shoot.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __SHOOT_H
#define __SHOOT_H

#include "Flash_Store.h"
#include "M2006_Motor.h"
#include "PID.h"
#include "Parabola.h"
#include <SpeedRamp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**********************************����ϵͳ��ܽṹ****************************/
/*
	����4�ţ�
			����װ�� shootUnit_1
					������� FricOutput��
					����װ��Reloader

			�ҷ���װ�� shootUnit_2
					������� FricOutput��
					����װ��Reloader

	Ӣ�ۣ�
			����װ�� shootUnit
				������� FricOutput��
				С����װ��Reloader

				�󲦵�װ�ò����ǣ�������ǲ��������ƣ�Ū��pid����һֱת�ͺ��ˡ�

	����Ĭ�϶���˫�������д��������ʱֻ��ֵ��һ���������ɣ�
	����װ��Ĭ��ʹ��2006
	Ĭ����1��2�� ��1��2��ǰ1��2�����������װ�ö����������
*/
/************************************************************************/

#define Reloader1Direction 1;      //����װ��1��ת������������Լ���Ҫ�ķ����˾�ȡ�� -1
#define Angle_Pill (8191 * 36 / 9) //����װ����Ҫת�����ٻ�е�Ƕ�������һ�ŵ��� = 8191*36/9
#define Fixed_BulletHeat 10        //�̶�����ֵ
#define FricOffNum 4               //Ħ����ƫ��������
#define EraseWrite                 //�Ƿ��������ƫ����������д������

#define M2006s_YawOPIDInit   \
    {                        \
        0,                   \
            0,               \
            0,               \
            0,               \
            0,               \
            0.34f,           \
            0.0f,            \
            0.0f,            \
            0,               \
            0,               \
            0,               \
            0,               \
            M2006_MaxOutput, \
            0,               \
            3000,            \
            &Position_PID,   \
    }

#define M2006s_YawIPIDInit   \
    {                        \
        0,                   \
            0,               \
            0,               \
            0,               \
            0,               \
            3.4f,            \
            0.0f,            \
            0.0f,            \
            0,               \
            0,               \
            0,               \
            0,               \
            M2006_MaxOutput, \
            0,               \
            3000,            \
            &Position_PID,   \
    }

#define M3508_PowerLPIDInit   \
    {                         \
        0,                    \
            0,                \
            0,                \
            0,                \
            0,                \
            20.0f,            \
            0.6f,             \
            0.0f,             \
            0,                \
            0,                \
            0,                \
            0,                \
            M2006_MaxOutput,  \
            3000,             \
            &Incremental_PID, \
    }

#define M3508_PowerRPIDInit   \
    {                         \
        0,                    \
            0,                \
            0,                \
            0,                \
            0,                \
            17.0f,            \
            1.2f,             \
            0.0f,             \
            0,                \
            0,                \
            0,                \
            0,                \
            M2006_MaxOutput,  \
            3000,             \
            &Incremental_PID, \
    }

#define Shoot_FunGroundInit            \
    {                                  \
        &Shoot_Init,                   \
            &Shoot_processing,         \
            &Shoot_openCartridge,      \
            &Shoot_setNeedLaunchCount, \
            &FricRead,                 \
            &FricWrite,                \
            false,                     \
    }

extern uint32_t auto_shoottime;
extern int need_shoottime;
extern int bullet1;
extern int need_shoottime1;
extern int needShoot;
extern int last_need_shoottime;

typedef enum
{
    Reloader_allow = 0,
    Reloader_ban = 1
} Reloader_license_t;

typedef struct
{
    int32_t totalAngle_start;         //��¼����ʼʱ���̵�λ��
    uint8_t alreadyPush;              // �Ѿ���ʼִ�в�������
    uint8_t completePush;             //ִ�гɹ�
    uint8_t PillObstruct;             //����������־
    uint8_t PillObstruct_Direction;   //�����������ִ�з���1��ת��0����
    int16_t PillObstruct_Time;        //��������ʱ��
    int32_t PillObstruct_targetAngle; //����������������õ�תĿ��ֵ��
    int32_t amount_hadLaunch;         //��ǰ�����Ѳ���������
    uint16_t ObstructSpeed;           //�����жϵ���������ٶ�ֵ
    Reloader_license_t Reloader_license;
} Reloader_t; //һ������װ�á�

typedef struct
{

    double shootSpeed_Judge; //��ǰĦ���ֵ��ٶȴ�С��Ӧ������ϵͳ��⵽���ٶ�ֵ��

    uint16_t NeedLaunchCount; //����Ҫ����ĵ���������

    //���̵�ǰ�Ĳ�������ṹ�塣
    Reloader_t Reloader;
    M2006s_t *Reloader_Motor; //����װ�ö�Ӧ�ĵ������Ҫ��ʼ��ʱָ���Ӧ�Ľṹ��

    uint16_t shootInterval; //����������λ���롣
    uint16_t bullet;        //�����ӵ�
    uint16_t FricSpeed;     //Ħ�����ٶ�
    uint16_t FricOutput;    //���������С����󸳸�����Ħ���ֵ������С
    bool FricUpdata;

} ShootUnit_t; //����װ��

typedef struct
{
    int shoot_LimitBullet;
    float shoot_OverallBullet;
    float shoot_JudgeOverallBullet;
    int shoot_RealBullet;
} Shoot_JudgeData_t; //ֱ�ӵ��ò���ϵͳģ������ݱ���̫������Ϊͳһ�ṹ�塣

typedef struct
{
    void (*Shoot_Init)(void);
    void (*Shoot_processing)(void);
    bool (*Shoot_openCartridge)(bool open);
    void (*Shoot_setNeedLaunchCount)(ShootUnit_t *shootUnit, uint16_t amount);
    void (*FricRead)(void);
    void (*FricWrite)(void);
    bool Cartridge_openFlag;
} Shoot_Fun_t;

extern Shoot_Fun_t Shoot_Fun;

extern ShootUnit_t shootUnit1;
extern Shoot_JudgeData_t Shoot_Judge;

extern int needShoot; //��Ҫ����Ŀ���
extern uint32_t FricOffset[FricOffNum];

#ifdef Enable_shootUnit2
extern ShootUnit_t shootUnit2; //ʹ��˫����
#endif

#endif /* __SHOOT_H */
