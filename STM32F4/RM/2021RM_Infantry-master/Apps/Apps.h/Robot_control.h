/**
 * @file Robot_control.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __Robot_CONTROL_H
#define __Robot_CONTROL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/************************************************************************/
/*
�ر�˵����
����ö�ٶ�����ǰ׺��ԭ�򣺷�ֹö�����ظ���ö�ٻ��ҵ��²���Ҫ�����⡣
*/
/************************************************************************/

//�����˿��Ƶ���Դ�����ź���Դ��
typedef enum
{
    ControlSource_RC = 0, //ң����ģʽ
    ControlSource_PC = 1, //����ģʽ
    ControlSource_Stop    //�رջ���
} ControlSource_e;

//���̹���ģʽ
typedef enum
{
    ChassisWorkMode_Follow = 0,        //������̨ģʽ
    ChassisWorkMode_NewFollow = 1,     //�������ģʽ
    ChassisWorkMode_Reversefollow = 2, //���������̨ģʽ
    ChassisWorkMode_Spin = 3,          //���߱���ģʽ
    ChassisWorkMode_Twister,           //Ť��ģʽ
    ChassisWorkMode_AutoTrace,         //�Զ�׷��ģʽ
    ChassisWorkMode_Supply,            //����ģʽ
    ChassisWorkMode_Lock,
    ChassisWorkMode_Disable //ʧ��ģʽ
} ChassisWorkMode_e;

//���̹���ģʽ
typedef enum
{
    AttachWorkMode_Slow = 0, //չ�������˶�
    AttachWorkMode_Disable   //����ģʽ�ر�
} AttachWorkMode_e;

//�������ݹ���ģʽ
typedef enum
{
    SuperCapacitor_Off = 0, //�رճ�������
    SuperCapacitor_Open
} SuperCapacitorMode_e;

//��̨����ģʽ
typedef enum
{
    CloudWorkMode_Normal = 0, //����ģʽ��360����̨��
    CloudWorkMode_LimitAngle, //�Ƕ�����ģʽ����360����̨��
    CloudWorkMode_SupplyWork, //����ģʽ
    CloudWorkMode_Disable     //ʧ��
} CloudWorkMode_e;

typedef enum
{
    SpecialMode_Normal = 0,
    SpecialMode_BigSymbol, //���ģʽ
    SpecialMode_UPBridge,  //����ģʽ
    SpecialMode_Supply,    //����վģʽ
} SpecialMode_t;
//�����˹���״̬
typedef enum
{
    WorkStatus_Prepare, //��ʧ�ܽ��뵽����״̬�Ĺ��ɽ׶Σ�����ʱ���ʼ���Ľ׶Σ��������豸��
    WorkStatus_Disable, //ʧ��״̬��Ӧ�Խ�������Ϳ���������
    WorkStatus_Normal   //��������״̬
} WorkStatus_e;

typedef enum
{
    Open,
    Close
} PlayRoomStatus_e;

//������״̬
typedef enum
{
    RammerWorkMode_Disable,  //�����ر�״̬
    RammerWorkMode_Normal,   //��������״̬
    RammerWorkMode_MaxSpeed, //����������
} RammerWorkMode_e;

//Ħ����ģʽ
typedef enum
{
    FrictWorkMode_Disable,   //Ħ���ֹر�ģʽ
    FrictWorkMode_HighSpeed, //Ħ���ָ���ģʽ
    FrictWorkMode_LowSpeed,  //Ħ���ֵ���ģʽ
    FrictWorkMode_AutoSpeed  //Ħ�����Զ�ģʽ

} FrictWorkMode_e;

typedef enum
{
    Magazine_Close, //���ֹر�
    Magazine_Open,  //���ֿ���
} Magazine_e;

//����ģʽ
typedef enum
{
    AttackMode_Disable,        //ʧ�ܹ���
    AttackMode_AutoMode,       //�Զ���׼
    AttackMode_ManualMode,     //�ֶ���׼
    AttackMode_SingleMode,     //����ģʽ
    AttackMode_ContinuousMode, //����ģʽ
    AttackMode_NoneMode,       //������ģʽ

} AttackMode_e;

typedef enum
{
    TeamColor_Blue,
    TeamColor_Red
} TeamColor_e;

typedef enum
{
    Types_Hero,
    Types_Engineer,
    Types_Standard,
    Types_Aerial = 6,
    Types_Sentry = 7
} Types_e;

typedef enum
{
    ShootTarget_default = 0,     //Ĭ��ģʽ
    ShootTarget_Self_aiming = 1, //����ģʽ
    ShootTarget_BIG_WHEEL,       //���ģʽ
    ShootTarget_Sentry,          //����Է��ڱ���ģʽ
    ShootTarget_Spin,            //��С����ģʽ
    ShootTarget_Video,           //¼��ģʽ
    ShootTarget_UAV,             //�ҷ����˻�����ģʽ
    ShootTarget_OurSentry,       //�ҷ��ڱ�����ģʽ
    ShootTarget_Radar,           //�״�ģʽ

} ShootTarget_e;

typedef enum
{
    OpenBurstOut_End = 0,
    OpenBurstOut_Start
} OpenBurstOut_e;

/**************************����Ϊö��**************************/

typedef struct
{
    //�������
    ControlSource_e ControlSource; //�����˵�ǰ��ʲô�豸����
    WorkStatus_e WorkStatus;       //�������Ƿ���׼��״̬��

    //�˶����
    ChassisWorkMode_e Sport_ChassisWorkMode;
    AttachWorkMode_e Sport_AttachWorkMode;
    CloudWorkMode_e Sport_CloudWorkMode;
    SuperCapacitorMode_e SuperCapacitorMode;

    //װ�����
    FrictWorkMode_e Device_FrictMode; //Ħ���ֹ���ģʽ��
    Magazine_e Magazine;              //�����Ƿ���

    //�������
    AttackMode_e Attack_AttackMode;
    ShootTarget_e Attack_ShootTarget;
    OpenBurstOut_e OpenBurstOut;
    bool VisionEnabled; //����ʶ�𿪹�

    //�������
    TeamColor_e TeamColor; //�ҷ����Ŷ���ɫ��
    Types_e Types;         //�ҷ�����

} Robot_t;

#define Robot_control_FUNGroundInit     \
    {                                   \
        &Robot_Enable,                  \
            &Robot_Disable,             \
            &Robot_Reset,               \
            &Robot_control,             \
            &Robot_init,                \
            &Robot_ChangeControlSource, \
            &Robot_setAttackMode,       \
            &Robot_setChassisWorkMode,  \
            &CAN_SendControl,           \
    }

typedef struct
{
    void (*Robot_Enable)(void);
    void (*Robot_Disable)(void);
    void (*Robot_Reset)(void);
    void (*Robot_control)(void);
    void (*Robot_init)(void);
    void (*Robot_ChangeControlSource)(ControlSource_e controlSource);
    void (*Robot_setAttackMode)(AttackMode_e mode);
    void (*Robot_setChassisWorkMode)(ChassisWorkMode_e mode);
    void (*CAN_SendControl)(void);

} Robot_control_FUN_t;

extern Robot_control_FUN_t Robot_control_FUN;
extern Robot_t Robot;

#endif
