/**
 * @file M3508_Motor.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "M3508_Motor.h"
#include <stdio.h>

M3508s_t M3508s[4];
M3508s_t M3508_PowerL; //Ħ���ֵ�� 201
M3508s_t M3508_PowerR; //Ħ���ֵ��; 202
M3508s_t *M3508_Power[] = {&M3508_PowerL, &M3508_PowerR};

void M3508_getInfo(Can_Export_Data_t RxMessage);
void M3508_Friction_getInfo(Can_Export_Data_t RxMessage);
void Check_WheelM3508(void);
void Check_FrictionM3508(void);

M3508_FUN_t M3508_FUN = M3508_FunGroundInit;

#undef M3508_FunGroundInit

/**
  * @brief  ����M3508�������ֵ��id��Ϊ1~4��
  * @param  iqx (x:1~4) ��Ӧid�ŵ���ĵ���ֵ����Χ-16384~0~16384
  * @retval None
  */
// void M3508_setCurrent(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
// {

//   uint8_t data[8];

//   //���ݸ�ʽ���˵����P32
//   data[0] = iq1 >> 8;
//   data[1] = iq1;
//   data[2] = iq2 >> 8;
//   data[3] = iq2;
//   data[4] = iq3 >> 8;
//   data[5] = iq3;
//   data[6] = iq4 >> 8;
//   data[7] = iq4;

//   // CAN_SendData(&hcan1, CAN_ID_STD, M3508_SENDID, data);
// }

/**
  * @brief  ��CAN�����л�ȡM3508�����Ϣ
  * @param  RxMessage 	CAN���Ľ��սṹ��
  * @retval None
  */
void M3508_getInfo(Can_Export_Data_t RxMessage)
{
    uint32_t StdId;
    StdId = (int32_t)(RxMessage.CAN_RxHeader.StdId - M3508_READID_START);
    //������ݣ����ݸ�ʽ���C620���˵����P33
    M3508s[StdId].realAngle = (uint16_t)(RxMessage.CANx_Export_RxMessage[0] << 8 | RxMessage.CANx_Export_RxMessage[1]);
    M3508s[StdId].realSpeed = (int16_t)(RxMessage.CANx_Export_RxMessage[2] << 8 | RxMessage.CANx_Export_RxMessage[3]);
    M3508s[StdId].realCurrent = (int16_t)(RxMessage.CANx_Export_RxMessage[4] << 8 | RxMessage.CANx_Export_RxMessage[5]);
    M3508s[StdId].temperture = RxMessage.CANx_Export_RxMessage[6];

    //֡��ͳ�ƣ����ݸ��±�־λ
    M3508s[StdId].InfoUpdateFrame++;
    M3508s[StdId].InfoUpdateFlag = 1;
}

/**
 * @brief ��CAN�����л�ȡM3508Ħ���ֵ����Ϣ
 * 
 * @param RxMessage 
 * @return  
 */
void M3508_Friction_getInfo(Can_Export_Data_t RxMessage)
{
    M3508s_t *Motor_P = NULL;
    switch (RxMessage.CAN_RxHeader.StdId)
    {
    case 0x205:
        Motor_P = M3508_Power[0];
        break;
    case 0x208:
        Motor_P = M3508_Power[1];
        break;
    }
    if (Motor_P == NULL)
        return;
    //������ݣ����ݸ�ʽ���C620���˵����P33
    Motor_P->realAngle = (uint16_t)(RxMessage.CANx_Export_RxMessage[0] << 8 | RxMessage.CANx_Export_RxMessage[1]);
    Motor_P->realSpeed = (int16_t)(RxMessage.CANx_Export_RxMessage[2] << 8 | RxMessage.CANx_Export_RxMessage[3]);
    Motor_P->realCurrent = (int16_t)(RxMessage.CANx_Export_RxMessage[4] << 8 | RxMessage.CANx_Export_RxMessage[5]);
    Motor_P->temperture = RxMessage.CANx_Export_RxMessage[6];

    //֡��ͳ�ƣ����ݸ��±�־λ
    Motor_P->InfoUpdateFrame++;
    Motor_P->InfoUpdateFlag = 1;
}

/**
 * @brief 3508���Ӽ��
 * 
 */
void Check_WheelM3508(void)
{
    //M3508���
    for (int i = 0; i < 4; i++)
    {
        if (M3508s[i].InfoUpdateFrame < 1)
        {
            M3508s[i].OffLineFlag = 1;
        }
        else
        {
            M3508s[i].OffLineFlag = 0;
        }
        M3508s[i].InfoUpdateFrame = 0;
    }
}

/**
 * @brief 3508Ħ���ּ��
 * 
 */
void Check_FrictionM3508(void)
{
    //M3508���
    for (int i = 0; i < 2; i++)
    {
        if (M3508_Power[i]->InfoUpdateFrame < 1)
        {
            M3508_Power[i]->OffLineFlag = 1;
        }
        else
        {
            M3508_Power[i]->OffLineFlag = 0;
        }
        M3508_Power[i]->InfoUpdateFrame = 0;
    }
}
