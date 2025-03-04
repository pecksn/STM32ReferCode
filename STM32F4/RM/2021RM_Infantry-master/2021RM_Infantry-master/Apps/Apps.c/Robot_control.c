/**
 * @file Robot_control.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Robot_control.h"
#include "BSP_CAN.h"
#include "Chassis_control.h"
#include "Cloud_control.h"
#include "Control_Vision.h"
#include "DJI_IMU.h"
#include "DR16_Remote.h"
#include "Laser.h"
#include "M6020_Motor.h"
#include "Monitor_RM_CAN.h"
#include "RMClient_UI.h"
#include "RM_JudgeSystem.h"
#include "Shoot.h"
#include "SuperCapacitor.h"
#include "typedef.h"

void Robot_init(void);
void CAN_SendControl(void);
void Robot_control(void);
void Robot_ChangeControlSource(ControlSource_e controlSource);
void Robot_Reset(void);
void Robot_Enable(void);
void Robot_Disable(void);
void Robot_setChassisWorkMode(ChassisWorkMode_e mode);
void Robot_setAttackMode(AttackMode_e mode);
Robot_control_FUN_t Robot_control_FUN = Robot_control_FUNGroundInit;
#undef Robot_control_FUNGroundInit

Robot_t Robot; //ȫ�ֻ����˶���
uint8_t RM_UITime;

/**
 * @brief ������״̬��ʼ��
 * 
 */
void Robot_init(void)
{
    Control_Vision_FUN.Vision_ID_Init();
    Robot.Attack_ShootTarget = ShootTarget_Self_aiming;
    Robot.Magazine = Magazine_Close;
    SuperCapacitorFUN.SuperCapacitor_Switch(false);
    UI_DeleteAll();
    for (int i = 0; i < 9; i++)	
    {
        Dispaly_Init[i]();
    }
    if (Robot.ControlSource == ControlSource_PC)
    {
        Robot.Sport_ChassisWorkMode = ChassisWorkMode_Follow;
        Robot.Sport_CloudWorkMode = CloudWorkMode_Normal;
        Robot.Device_FrictMode = FrictWorkMode_Disable;
        Robot.Attack_AttackMode = AttackMode_ManualMode;
    }

    LASER_Set(GPIO_PIN_RESET);
#ifdef Enable_Vision_Test
    Vision_I_T_Set('1', '2');
#endif
}

static void Chassis_control()
{
    //��������ͨ�ż�����
    //	Cap_Tick_5MS++;
    //	if (Cap_Tick_5MS >= 2)
    //	{
    //		ChassisCapControl();
    //		Cap_Tick_5MS = 0;
    //	}

    Chassis_FUN.ChassisCapControl();
    Chassis_FUN.Chassis_processing(DR16_Export_Data.Robot_TargetValue.Left_Right_Value, DR16_Export_Data.Robot_TargetValue.Forward_Back_Value, DR16_Export_Data.Robot_TargetValue.Omega_Value);
}

/**
 * @brief ��̨����
 * 
 */
static void Cloud_control(void)
{
    Cloud_FUN.IMUData_chance();
    if (Robot.Sport_ChassisWorkMode == ChassisWorkMode_NewFollow || ReversefollowFlag.NewFollowFlag == 1)
    {
        Cloud_FUN.Cloud_processing_WithoutIMU(DR16_Export_Data.Robot_TargetValue.Yaw_Value, DR16_Export_Data.Robot_TargetValue.Pitch_Value);
    }
    else
    {
        Cloud_FUN.Cloud_processing_WithIMU(DR16_Export_Data.Robot_TargetValue.Yaw_Value, DR16_Export_Data.Robot_TargetValue.Pitch_Value);
    }
}

/**
 * @brief Can�ܷ���
 * 
 */
void CAN_SendControl(void)
{
    //���һ�����ķ�����ͬ��������⡣
    //CAN1 ��yaw = 1 �� ���� = 7
    //CAN2 �� pitch = 2

    // -------  CAN1
    Monitor_CAN_FUN.CAN_0x200_SendData(&hcan1, M3508s[0].outCurrent, M3508s[1].outCurrent, M3508s[2].outCurrent, M3508s[3].outCurrent); //���̵��(3508)

    Monitor_CAN_FUN.CAN_0x1FF_SendData(&hcan1, M6020s_Yaw.outCurrent, 0, 0, 0); //pitch����(6020)��yaw����(6020)��

    // Monitor_CAN_FUN.CAN_0x2FF_SendData(&hcan1, 5000, 0, 0, 0); //pitch����(6020)��yaw����(6020)��

    Monitor_CAN_FUN.CAN_0x601_SendData(&hcan1, supercapacitor.SendData.data);
    // -------  CAN2
    Monitor_CAN_FUN.CAN_0x1FF_SendData(&hcan2, M3508_PowerL.outCurrent, M6020s_Pitch.outCurrent, M2006_Reload.outCurrent, M3508_PowerR.outCurrent);
}

WorldTime_RxTypedef Control_WorldTime;
uint32_t Robot_FPS;
void Robot_control(void)
{

    Chassis_control();
    Cloud_control();
    Shoot_Fun.Shoot_processing();

    CAN_SendControl();
    Get_FPS(&Control_WorldTime, &Robot_FPS);
}

/**
	* @brief  ���Ļ����˿�����Դ
 * @param	void
 * @retval None
 */
void Robot_ChangeControlSource(ControlSource_e controlSource)
{
    if (Robot.ControlSource != controlSource) //����ģʽ���䣬���á�
    {
        Robot_Reset();
    }
    Robot.ControlSource = controlSource;
}
/**
 * @brief  ״̬��λ
 * @param	void
 * @retval None
 */
void Robot_Reset(void)
{
    //���³�ʼ�������ˡ�
    Robot.Sport_ChassisWorkMode = ChassisWorkMode_Follow;
    Robot.Sport_CloudWorkMode = CloudWorkMode_Normal;
    Robot.Device_FrictMode = FrictWorkMode_Disable;
    Robot.Sport_AttachWorkMode = AttachWorkMode_Disable;
    Robot.Attack_AttackMode = AttackMode_ManualMode;
    Robot.Attack_ShootTarget = ShootTarget_Self_aiming;
    Robot.VisionEnabled = false;
    LASER_Set(GPIO_PIN_RESET);
}

/**
 * @brief ������ȫ��ʹ��
 * 
 */
void Robot_Enable(void)
{
    Robot.WorkStatus = WorkStatus_Normal;
}

/**
 * @brief ������ȫ��ʧ��
 * 
 */
void Robot_Disable(void)
{
    DR16_Export_Data.Robot_TargetValue.Forward_Back_Value = 0;
    DR16_Export_Data.Robot_TargetValue.Left_Right_Value = 0;
    DR16_Export_Data.Robot_TargetValue.Omega_Value = 0;
    DR16_Export_Data.Robot_TargetValue.Pitch_Value = 0;
    DR16_Export_Data.Robot_TargetValue.Yaw_Value = 0;
    Robot.Sport_ChassisWorkMode = ChassisWorkMode_Disable;
    Robot.Sport_AttachWorkMode = AttachWorkMode_Disable;
    Robot.Sport_CloudWorkMode = CloudWorkMode_Disable;
    Robot.Attack_AttackMode = AttackMode_Disable;
    Robot.Device_FrictMode = FrictWorkMode_Disable;
    Robot.Attack_ShootTarget = ShootTarget_Self_aiming;
    Robot.VisionEnabled = false;

    /***************��̨������̽ṹ������*************/
    ReversefollowFlag.NewFollowFlag = 0;
    ReversefollowFlag.ReversefollowTime = 0;
    ReversefollowFlag.transition = 0;

    /***************�糵�ṹ������*************/
    BIGWHEEL_EndRunFlag.BIG_WHEELEndFlag = 0;
    BIGWHEEL_EndRunFlag.BIG_WHEELEndTime = 0;
    BIGWHEEL_EndRunFlag.Lsat_Angle = 0;
    LASER_Set(GPIO_PIN_RESET);
}

void Robot_setAttackMode(AttackMode_e mode)
{
    // if (Robot.Attack_AttackMode == AttackMode_AutoMode && mode != AttackMode_AutoMode)
    // {
    //     shootUnit1.NeedLaunchCount = 0; //���Զ����ģʽ��ȡ��ʱ�������Զ����������
    // }
    // Robot.Attack_AttackMode = mode;

    switch (Robot.Attack_AttackMode)
    {
    case AttackMode_ManualMode:
        Robot.Device_FrictMode = FrictWorkMode_HighSpeed;
        break;
    case AttackMode_Disable:
        Robot.Device_FrictMode = FrictWorkMode_Disable;
        break;
    case AttackMode_AutoMode:
        Robot.Device_FrictMode = FrictWorkMode_HighSpeed;
        break;
    default:
        Robot.Device_FrictMode = FrictWorkMode_Disable;
        break;
    }
}

void Robot_setChassisWorkMode(ChassisWorkMode_e mode)
{

    if (Robot.Sport_ChassisWorkMode == ChassisWorkMode_Spin && mode != ChassisWorkMode_Spin)
    {
        //��Ҫ�����������totalAngle
        M6020_Fun.M6020_Reset(&M6020s_Yaw); //���������۵�total���㡣
    }

    Robot.Sport_ChassisWorkMode = mode;
}
