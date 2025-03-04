/**
 * @file Robot_Config.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __Robot_CONFIG_H
#define __Robot_CONFIG_H

/************************************************************************/
/*          ������Ϊ�˱���Ӧ���������ǰ׼���ķ�����
			 Ŀ�ģ�ĳӲ����Ҫ����ʱ��ͨ���޸ĺ궨��ʹ�����˿��ٸ����豸��
*/
/************************************************************************/



#define Device_BoardType 2 //0=���İ��� ��1=2017�ɹٷ���	�� 2=2018�¹ٷ���
//���µ���ö��������ʹ�á�
#define Device_BoardType_Wolf 0
#define Device_BoardType_2017 1
#define Device_BoardType_2018 2

#define Infantry_Year 1
#define Infantry_2020 0
#define Infantry_2021 1

#define Friction__Wheel 0
#define M3508_Wheel 0 //M3508Ħ����
#define Snail_Wheel 1 //SnailĦ����

#define Cloud_DeBug 0 //��̨PID����
#define Cloud_NoDeBug 0
#define Cloud_YesDeBug 1


#define Vision_Version 1
#define Vision_Oldversion 0 //���Ӿ��汾
#define Vision_Newversion 1 //���Ӿ��汾

#define Cloud_IMU 0
#define Cloud_DJI_IMU 0
#define Cloud_Wolf_IMU 1

#define Who_Infantry 0
#define ZJ_Infantry 0
#define ZF_Infantry 1

//ע�͵���ʾ��ǰ�������ø�ģ��
#define Enable_DeviceMonitor //�������ߡ����󱨾���
#define Enable_CloudControl
#define Enable_ShootSystem //�������ϵͳ
#define Enable_RMJudgeSystem
// #define Temporary_Function
//#define Enable_Buzzer //���÷�����
// #define Enable_Vision_Test
// #define Enable_shootUnit2  //�Ƿ���˫����
#endif
