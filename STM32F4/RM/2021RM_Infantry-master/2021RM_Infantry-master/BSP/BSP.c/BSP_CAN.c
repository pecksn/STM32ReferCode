/**
 * @file BSP_CAN.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "BSP_CAN.h"

/*******************************�û����ݶ���************************************/
void CAN_IT_Init(CAN_HandleTypeDef *hcanx, uint8_t Can_type);
void CAN_RxMessage_Export_Date(CAN_HandleTypeDef *hcanx, osMessageQId CANx_Handle, uint8_t Can_type);
void CAN_SendData(osMessageQId CANx_Handle, CAN_HandleTypeDef *CANx, uint8_t id_type, uint32_t id, uint8_t data[8]);
void Check_CAN(void);
Can_Fun_t Can_Fun = Can_FunGroundInit;
#undef Can_FunGroundInit

/***********************************�ӿڸ�ֵ************************************/
Can_Data_t Can_Data[2] = Can_DataGroundInit;
#undef Can_DataGroundInit
CAN_Devices_t *Monitor_CAN[] = {&Monitor_CAN1, &Monitor_CAN2};
CAN_Devices_t Monitor_CAN1, Monitor_CAN2;
/*******************************************************************************/

/**
  * @Data   2021-03-24
  * @brief  CANɸѡ����ʼ��
  * @param  CAN_FilterTypeDef *CAN_Filter, CAN_HandleTypeDef *hcanx
  * @retval void
  */
static void CAN_FILTER_Init(CAN_FilterTypeDef *CAN_Filter, CAN_HandleTypeDef *hcanx)
{
    CAN_Filter->FilterFIFOAssignment = CAN_FILTER_FIFO0; //ɸѡ����������FIFO0
    CAN_Filter->FilterBank = 0;                          //ɸѡ����0
    CAN_Filter->SlaveStartFilterBank = 0;
    CAN_Filter->FilterMode = CAN_FILTERMODE_IDMASK;   //������ID����ģʽ
    CAN_Filter->FilterScale = CAN_FILTERSCALE_32BIT;  //ɸѡ��λ��Ϊ����32λ��
    CAN_Filter->FilterActivation = CAN_FILTER_ENABLE; //ʹ��ɸѡ��
                                                      /* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID�������µľ����������ǵĻ��������FIFO0�� */
    CAN_Filter->FilterIdHigh = 0x0000;                //Ҫɸѡ��ID��λ
    CAN_Filter->FilterIdLow = 0x0000;                 //Ҫɸѡ��ID��λ
    CAN_Filter->FilterMaskIdHigh = 0x0000;            //ɸѡ����16λÿλ����ƥ��
    CAN_Filter->FilterMaskIdLow = 0x0000;             //ɸѡ����16λÿλ����ƥ��
    HAL_CAN_ConfigFilter(hcanx, CAN_Filter);
}

/**
  * @Data   2021-03-24
  * @brief   canx�ж�����
  * @param   CAN_HandleTypeDef *hcanx, uint8_t Can_type
  * @retval  void
  */
void CAN_IT_Init(CAN_HandleTypeDef *hcanx, uint8_t Can_type)
{
    uint8_t Canx_type = Can_type - 1;
    /*ʹ���˲���*/

    CAN_FILTER_Init(&Can_Data[Canx_type].CAN_FilterTypedef.CAN_Filter, hcanx);
    /*����CAN*/
    HAL_CAN_Start(hcanx);
    /*ʹ��CAN��IT�ж�*/
    __HAL_CAN_ENABLE_IT(hcanx, CAN_IT_RX_FIFO0_MSG_PENDING); //  CAN_IT_FMP0
}

/**
  * @Data   2021-03-27
  * @brief  canx�жϽ���
  * @param  CAN_HandleTypeDef *hcanx, osMessageQId CANx_Handle, uint8_t Can_type
  * @retval void
  */
void CAN_RxMessage_Export_Date(CAN_HandleTypeDef *hcanx, osMessageQId CANx_Handle, uint8_t Can_type)
{
    Can_Export_Data_t Can_Export_Data[2];
    uint8_t Canx_type = Can_type - 1;
    HAL_CAN_GetRxMessage(hcanx, CAN_RX_FIFO0,
                         &Can_Data[Canx_type].CAN_RxTypedef.CANx_RxHeader,
                         Can_Data[Canx_type].CAN_RxTypedef.CAN_RxMessage);

    Can_Export_Data[Canx_type].CAN_RxHeader = Can_Data[Canx_type].CAN_RxTypedef.CANx_RxHeader;
    memcpy(&Can_Export_Data[Canx_type].CANx_Export_RxMessage,
           Can_Data[Canx_type].CAN_RxTypedef.CAN_RxMessage,
           sizeof(uint8_t[8]));

    xQueueSendToBackFromISR(CANx_Handle, &Can_Export_Data[Canx_type], 0); //�ѽ������ݷ������ն���
}

/**
  * @brief  CAN��������
  * @param  CANx 		CAN���
  * 		id_type ��	id���� CAN_ID_STD�� CAN_ID_EXT
  *			id			id��
  * 		data[8]		8������
  * @retval None
  */
void CAN_SendData(osMessageQId CANx_Handle, CAN_HandleTypeDef *CANx, uint8_t id_type, uint32_t id, uint8_t data[8])
{
    Can_Send_Data_t Can_Send_Data;

    Can_Send_Data.Canx = CANx;
    if (id_type == CAN_ID_STD)
    {
        Can_Send_Data.CAN_TxHeader.StdId = id;
    }
    else
    {
        Can_Send_Data.CAN_TxHeader.ExtId = id; //ID��
    }

    Can_Send_Data.CAN_TxHeader.IDE = id_type;      //ID����
    Can_Send_Data.CAN_TxHeader.RTR = CAN_RTR_DATA; //���͵�Ϊ����
    Can_Send_Data.CAN_TxHeader.DLC = 0x08;         //���ݳ���Ϊ8�ֽ�
    Can_Send_Data.CAN_TxHeader.TransmitGlobalTime = DISABLE;

    memcpy(Can_Send_Data.CANx_Send_RxMessage,
           data,
           sizeof(uint8_t[8]));

    xQueueSend(CANx_Handle, &Can_Send_Data, 0);
}

void Check_CAN(void)
{
    for (uint8_t i = 0; i < 2; i++)
    {
        if (Monitor_CAN[i]->InfoUpdateFrame < 1)
        {
            Monitor_CAN[i]->OffLineFlag = 1;
        }
        else
        {
            Monitor_CAN[i]->OffLineFlag = 0;
        }
        Monitor_CAN[i]->InfoUpdateFrame = 0;
    }
}
