/**
  ******************************************************************************
  * @file    Supercapacitor.c
  * @author  
  * @version V1.0
  * @brief    ��������ģ��
  ******************************************************************************
  */

#include "SuperCapacitor.h"

Supercapacitor_t supercapacitor;

void SCCM_MsgProcess(Can_Export_Data_t RxMessage);
void SCCM_SendMsg(float Charge_power, uint8_t Charge_enable, uint8_t Cap_enable);
void SuperCapacitor_Switch(bool state);
void Check_SuperCapacitor(void);
SuperCapacitorFUN_t SuperCapacitorFUN = SuperCapacitorFUN_Init;
#undef SuperCapacitorFUN_Init

uint8_t Cap_Tick_5MS;

/**
	* @brief  �������ݽ��պ���
  * @param	����buff
  * @retval None
  */
void SCCM_MsgProcess(Can_Export_Data_t RxMessage)
{

    memcpy(supercapacitor.ReceiveData.data, RxMessage.CANx_Export_RxMessage, 8);

    //֡��
    supercapacitor.InfoUpdateFrame++;
    supercapacitor.InfoUpdateFlag = 1;
}

/**
	* @brief  �������ݷ��ͺ���
  * @param	��繦�� ���ʹ�� �Ƿ���ݹ���
  * @retval None
  */
void SCCM_SendMsg(float Charge_power, uint8_t Charge_enable, uint8_t Cap_enable)
{
    supercapacitor.SendData.charge_power = Charge_power;
    supercapacitor.SendData.charge_enable = Charge_enable;
    supercapacitor.SendData.is_cap_output = Cap_enable;
}

/**
	* @brief  �������ݿ���
  * @param	void
  * @retval void
  */
void SuperCapacitor_Switch(bool state)
{
    if (state == true)
    {
        supercapacitor.EnableCapacitor = true;
    }
    else
    {
        supercapacitor.EnableCapacitor = false;
    }
}

/**
 * @brief �������ݼ��
 * 
 */
void Check_SuperCapacitor(void)
{
    if (supercapacitor.InfoUpdateFrame < 1)
    {
        supercapacitor.OffLineFlag = 1;
    }
    else
    {
        supercapacitor.OffLineFlag = 0;
    }
    supercapacitor.InfoUpdateFrame = 0;
}
