/**
 * @file Control_Vision.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Control_Vision.h"
#include "DJI_IMU.h"
#include "M6020_Motor.h"
#include "RM_JudgeSystem.h"
#include "cmsis_os.h"

uint8_t Vision_DataBuff[Vision_BuffSIZE];
VisionData_t VisionData;
VisionSend_IMU_t VisionSend_IMU;
WorldTime_RxTypedef Vision_WorldTime;
WorldTime_RxTypedef VisionKF_TIME;

extKalman_t Vision_depthKalman;

void Vision_Init(void);
void Update_VisionTarget(void);
void Vision_processing(void);
void Vision_ID_Init(void);
void Vision_Handler(UART_HandleTypeDef *huart);
void Vision_USART_Receive_DMA(UART_HandleTypeDef *huartx);
void Vision_SendBufFunction(float *angle, float *Gyro);
uint8_t GetVisionDiscMode(void);
uint8_t GetVisionHitMode(void);
void Check_Vision(void);
void Vision_CP(float CPData);
VisionExportData_t VisionExportData = VisionExportDataGroundInit;
#undef VisionExportDataGroundInit
Control_Vision_FUN_t Control_Vision_FUN = Control_Vision_FUNGroundInit;
#undef Control_Vision_FUNGroundInit

/*
	sprintf(g_buf_temp, "%c %1d %1d %03d %03d %03d", 'S', mode, mode_select,x, y, depth);
	uint8_t CRC = Checksum_CRC8(g_buf_temp, sizeof(g_buf_temp));
	sprintf(g_buf, "%c %1d%1d%03d%03d%03d%03d%c", 'S',mode, mode_select, x, y, depth, CRC, 'E');
*/

//�Լ���ɫ��0 ���� 1��	2 ��
//ģʽ��0Ĭ�ϣ�1���飬2�������3�����ڱ���4С���ݣ�5¼��6�ɻ���7�ڱ����飬8�״�
//������ID��1Ӣ�ۣ�2���̣�3������6���˻���7�ڱ�
//ͨ��һ����ά����Ƕ�ױ��治ͬĿ��Ĵ��������ݣ�������Ӧö��ShootTarget_e
uint8_t Vision_SendBuf[9][16] = {'S', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '2', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '5', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E',
                                 'S', '0', '8', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'E'};

static void Vision_I_T_Set(uint8_t ID, uint8_t Type)
{
    for (uint8_t n = 0; n < 9; n++)
    {
        Vision_SendBuf[n][1] = ID;
        Vision_SendBuf[n][3] = Type;
    }
}

void Vision_ID_Init(void)
{
    switch (ext_game_robot_state.data.robot_id)
    {
    case 1:
        Robot.TeamColor = TeamColor_Red;
        Robot.Types = Types_Hero;
        Vision_I_T_Set('1', '1');
        break;

    case 2:
        Robot.TeamColor = TeamColor_Red;
        Robot.Types = Types_Engineer;
        Vision_I_T_Set('1', '2');
        break;

    case 3:
        Robot.TeamColor = TeamColor_Red;
        Robot.Types = Types_Standard;
        Vision_I_T_Set('1', '3');
        break;

    case 4:
        Robot.TeamColor = TeamColor_Red;
        Robot.Types = Types_Standard;
        Vision_I_T_Set('1', '3');
        break;

    case 5:
        Robot.TeamColor = TeamColor_Red;
        Robot.Types = Types_Standard;
        Vision_I_T_Set('1', '3');
        break;

    case 6:
        Robot.TeamColor = TeamColor_Red;
        Robot.Types = Types_Aerial;
        Vision_I_T_Set('1', '6');
        break;

    case 7:
        Robot.TeamColor = TeamColor_Red;
        *Vision_SendBuf[1] = '1';
        Robot.Types = Types_Sentry;
        Vision_I_T_Set('1', '7');
        break;

    case 101:
        Robot.TeamColor = TeamColor_Blue;
        Robot.Types = Types_Hero;
        Vision_I_T_Set('2', '1');
        break;

    case 102:
        Robot.TeamColor = TeamColor_Blue;
        Robot.Types = Types_Engineer;
        Vision_I_T_Set('2', '2');
        break;

    case 103:
        Robot.TeamColor = TeamColor_Blue;
        Robot.Types = Types_Standard;
        Vision_I_T_Set('2', '3');
        break;

    case 104:
        Robot.TeamColor = TeamColor_Blue;
        Robot.Types = Types_Standard;
        Vision_I_T_Set('2', '3');
        break;

    case 105:
        Robot.TeamColor = TeamColor_Blue;
        Robot.Types = Types_Standard;
        Vision_I_T_Set('2', '3');
        break;

    case 106:
        Robot.TeamColor = TeamColor_Blue;
        Robot.Types = Types_Aerial;
        Vision_I_T_Set('2', '6');
        break;

    case 107:
        Robot.TeamColor = TeamColor_Blue;
        Robot.Types = Types_Sentry;
        Vision_I_T_Set('2', '7');
        break;
    }
}

/**
	* @brief  ���Ӿ��������ݺ���
 * @param	����
 * @retval None
 */
static void SendVisionData(uint8_t *data)
{
    if (data == NULL)
    {
        return;
    }

    for (int i = 0; i < 16; i++)
    {
        //		USART_sendChar(UART7, data[i]);
        HAL_UART_Transmit(&huart7, &data[i], 1, 0xff);
    }
}

/**
 * @brief �����ǽǶȷ���
 * 
 * @param angle 
 * @param Gyro 
 */
void Vision_SendBufFunction(float *angle, float *Gyro)
{
    VisionSend_IMU.VisionSend_t.YawAngle_Error = angle[0] - 180.0f;
    VisionSend_IMU.VisionSend_t.PitchAngle_Error = Gyro[0] / 16.384f / 57.3f; //��ʱ����YAW�������ٶ�
    VisionSend_IMU.Gyro_z = 0;
    VisionSend_IMU.Gyro_z_Hight = 0;
    VisionSend_IMU.Gyro_z_low = 0;
}

void Update_VisionTarget(void)
{ //�����Ӿ�ʶ��Ŀ�ꡣ

    for (uint8_t n = 0; n < 9; n++)
    {
        Vision_SendBuf[n][4] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[0];
        Vision_SendBuf[n][5] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[1];
        Vision_SendBuf[n][6] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[2];
        Vision_SendBuf[n][7] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[3];
        Vision_SendBuf[n][8] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[4];
        Vision_SendBuf[n][9] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[5];
        Vision_SendBuf[n][10] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[6];
        Vision_SendBuf[n][11] = VisionSend_IMU.VisionSend_t.Angle_Error_Data[7];
        Vision_SendBuf[n][12] = VisionSend_IMU.Gyro_z_Hight;
        Vision_SendBuf[n][13] = VisionSend_IMU.Gyro_z_low;
        Vision_SendBuf[n][14] = ext_game_robot_state.data.shooter_id1_17mm_speed_limit;
    }

    switch (Robot.Attack_ShootTarget)
    {
    case ShootTarget_default:
        SendVisionData(Vision_SendBuf[0]);
        break;
    case ShootTarget_Self_aiming:
        SendVisionData(Vision_SendBuf[1]);
        break;
    case ShootTarget_BIG_WHEEL:
        SendVisionData(Vision_SendBuf[2]);
        break;
    case ShootTarget_Sentry:
        SendVisionData(Vision_SendBuf[3]);
        break;
    case ShootTarget_Spin:
        SendVisionData(Vision_SendBuf[4]);
        break;
    case ShootTarget_Video:
        SendVisionData(Vision_SendBuf[5]);
        break;
    case ShootTarget_UAV:
        SendVisionData(Vision_SendBuf[6]);
        break;
    case ShootTarget_OurSentry:
        SendVisionData(Vision_SendBuf[7]);
        break;
    case ShootTarget_Radar:
        SendVisionData(Vision_SendBuf[8]);
        break;
    }
}

void Vision_Init(void)
{

    VisionData.Gravity_Offset.x = 0;
    VisionData.Gravity_Offset.y = 0;
    //VisionData.Gravity_Offset.y = 60;

    VisionData.LpfAttFactor.y = 0.1;
    VisionData.LpfAttFactor.x = 0.9;

    VisionData.SpeedGain.y = 0.0; //1.2
    VisionData.SpeedGain.x = 0.0; //-2.2

    VisionExportData.FinalOffset_Last.x = 0;
    VisionExportData.FinalOffset_Last.y = 0;

    KalmanCreate(&Vision_depthKalman, 1, 40);
}

/**
 * @brief �Ӿ����ݲ���
 * 
 */
void Vision_processing(void)
{

    //�Ե�ǰ���ݽ����жϣ��Ƿ���Խ������
    if (VisionData.RawData.whether_Fire)
    {
        VisionExportData.AbleToFire = true;
    }

    if (VisionData.RawData.mode == 0 || VisionData.RawData.depth == 0 || Robot.VisionEnabled == false || VisionData.OffLineFlag == 1) //�Ӿ�ģ��ʧ�ܻ��Ҳ���Ŀ��
    {
        VisionExportData.FinalOffset_Last.x = 0;
        VisionExportData.FinalOffset_Last.y = 0;
        VisionExportData.FinalOffset.x = 0;
        VisionExportData.FinalOffset.y = 0;
        VisionExportData.FinalOffset_depth = 0;

        VisionExportData.FinalOffset_depth = 1000.0f;

        VisionExportData.AbleToFire = false;
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);
        /*	VisionData.Final_Offset.x = 0;
			VisionData.Final_Offset.y = 0;

			VisionData.ErrorChange_Rate.x = 0;
			VisionData.ErrorChange_Rate.y = 0;*/
        //ע�⣺���ﲻ������VisionData.Target_Offset ��������ecֵ����

        return;
    }

    //�Ե�ǰ���������㣬���������˶���
    // --- �����Ƿ����
    if (VisionData.InfoUpdateFlag != 0)
    {
        VisionExportData.FinalOffset.x = VisionData.Final_Offset.x * M6020_mAngleRatio;
        if (Robot.Attack_ShootTarget == ShootTarget_BIG_WHEEL)
        {
            VisionExportData.FinalOffset.y = VisionData.Final_Offset.y * M6020_mAngleRatio;
        }
        else
        {
            VisionExportData.FinalOffset.y = (VisionData.Final_Offset.y + VisionData.Gravity_Offset.y) * M6020_mAngleRatio;
        }

        VisionKF_TIME.WorldTime = xTaskGetTickCount();

        VisionData.InfoUpdateFlag = 0; // �����־λ
    }

    VisionExportData.FinalOffset_depth = KalmanFilter(&Vision_depthKalman, VisionData.RawData.depth);

    VisionFourth_Ring_Yaw.Speed_Gain = VisionData.SpeedGain.x * imu_Export.SpeedLPF[0];
    VisionFourth_Ring_Pitch.Speed_Gain = VisionData.SpeedGain.y * M6020s_Pitch.realSpeed;
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);

    //if (VisionData.RawData.whether_Fire == 1)
    //{
    //	Buzzer_On(true);
    //}
    //else
    //{
    //	Buzzer_On(false);
    //}
}

/**
 * @brief �Ӿ�����
 * 
 * @param data 
 */
void Vision_DataReceive(uint8_t *data)
{

    uint8_t CRCBuffer = Checksum_CRC8(data, 13 - 2);

#if Vision_Version == Vision_Oldversion
    //	sscanf(data, "%c%1d%1d%04d%03d%04d%03d%c", &VisionData.RawData.Start_Tag, &VisionData.RawData.mode, &VisionData.RawData.mode_select, &VisionData.RawData.x, &VisionData.RawData.y, &VisionData.RawData.depth, &VisionData.RawData.crc, &VisionData.RawData.End_Tag);
    sscanf(data, "%c%1d%1d%1d%04d%1d%03d%04d%03d%c", &VisionData.RawData.Start_Tag, &VisionData.RawData.mode, &VisionData.RawData.whether_Fire, &VisionData.RawData._yaw,
           &VisionData.RawData.x, &VisionData.RawData._pitch, &VisionData.RawData.y, &VisionData.RawData.depth, &VisionData.RawData.crc, &VisionData.RawData.End_Tag);

    if (/*CRCBuffer != VisionData.RawData.crc ||*/ VisionData.RawData.Start_Tag != 'S' || VisionData.RawData.End_Tag != 'E') //CRCУ��ʧ�ܡ�
    {
        return;
    }

    VisionData.InfoUpdateFrame++;

    //Get_FPS(&Vision_WorldTime, &VisionData.FPS);

    if (VisionData.RawData.mode == 0 || VisionData.RawData.depth == 0 || VisionData.OffLineFlag == 1) //�Ӿ�ģ��ʧ�ܻ��Ҳ���Ŀ��
    {
        VisionExportData.FinalOffset_Last.x = 0;
        VisionExportData.FinalOffset_Last.y = 0;
        VisionExportData.FinalOffset.x = 0;
        VisionExportData.FinalOffset.y = 0;
        VisionData.Final_Offset.x = 0;
        VisionData.Final_Offset.y = 0;

        VisionData.ErrorChange_Rate.x = 0;
        VisionData.ErrorChange_Rate.y = 0;
        //ע�⣺���ﲻ������VisionData.Target_Offset ��������ecֵ����
        VisionData.Target_Offset.x = VisionPage_Width / 2;
        VisionData.Target_Offset.y = VisionPage_Height / 2;
        VisionExportData.AbleToFire = false;
        return;
    }

    //����ת�ɱ��ε�ƫ�
    VisionData.Target_Offset.x = Vision_getOffset(VisionPage_Width, VisionData.RawData.x);
    VisionData.Target_Offset.y = Vision_getOffset(VisionPage_Height, VisionData.RawData.y);

    if (VisionData.RawData.depth != 0) //��ֹ�� ��ʧ �� �ҵ� ����ec ���󣡣���
    {
        //����ƫ��仯�ʡ�
        VisionData.ErrorChange_Rate.x = VisionData.Target_Offset.x - VisionData.Target_LastOffset.x;
        VisionData.ErrorChange_Rate.y = VisionData.Target_Offset.y - VisionData.Target_LastOffset.y;

        //������һ�ε�ƫ�
        VisionData.Target_LastOffset.x = VisionData.Target_Offset.x;
        VisionData.Target_LastOffset.y = VisionData.Target_Offset.y;
    }
    else
    {
        VisionData.ErrorChange_Rate.x = 0;
        VisionData.ErrorChange_Rate.y = 0;
        VisionData.Target_LastOffset.x = VisionData.Target_Offset.x;
        VisionData.Target_LastOffset.y = VisionData.Target_Offset.y;
    }

    VisionData.Gravity_Offset.y =
        /*(pow(VisionData.RawData.depth, 3) *0.0004088 -
			0.2795 * pow(VisionData.RawData.depth, 2) +
			74.2*VisionData.RawData.depth - 1316) / (VisionData.RawData.depth - 30.77)+*/
        150;

    //ȷ���������ڼ����ƫ��ֵ
    VisionData.Final_Offset.x = VisionData.Target_Offset.x - VisionData.Gravity_Offset.x;
    VisionData.Final_Offset.y = VisionData.Target_Offset.y - VisionData.Gravity_Offset.y;

    /*
f(x) = (p1*x^3 + p2*x^2 + p3*x + p4) / (x + q1)
Coefficients:
	   p1 =   0.0004088
	   p2 =     -0.2795
	   p3 =        74.2
	   p4 = - 1316
	   q1 =      -30.77
	*/

#elif Vision_Version == Vision_Newversion

    for (int i = 0; i < 13; i++)
    {
        VisionData.RawData.VisionRawData[i] = data[i];
    }

    VisionData.RawData.x = ((VisionData.RawData.x_H << 8) | VisionData.RawData.x_L) / 100.0f;
    VisionData.RawData.y = ((VisionData.RawData.y_H << 8) | VisionData.RawData.y_L) / 100.0f;
    VisionData.RawData.depth = (VisionData.RawData.depth_H << 8) | VisionData.RawData.depth_L;

    if (VisionData.RawData._yaw == 0)
    {
        VisionData.RawData.x *= -1.0f;
    }

    if (VisionData.RawData._pitch == 0)
    {
        VisionData.RawData.y *= -1.0f;
    }

    if (CRCBuffer != VisionData.RawData.crc || VisionData.RawData.Start_Tag != 'S' || VisionData.RawData.End_Tag != 'E') //CRCУ��ʧ�ܡ�
    {
        return;
    }

    VisionData.InfoUpdateFrame++;
    VisionData.InfoUpdateFlag = 1;

    Get_FPS(&Vision_WorldTime, &VisionData.FPS);

    if (VisionData.RawData.mode == 0 || VisionData.RawData.depth == 0 || VisionData.OffLineFlag == 1) //�Ӿ�ģ��ʧ�ܻ��Ҳ���Ŀ��
    {
        VisionExportData.FinalOffset_Last.x = 0;
        VisionExportData.FinalOffset_Last.y = 0;
        VisionExportData.FinalOffset.x = 0;
        VisionExportData.FinalOffset.y = 0;
        VisionData.Final_Offset.x = 0;
        VisionData.Final_Offset.y = 0;
        VisionData.RawData._pitch = 0;
        VisionData.RawData.depth = 1000.0f;
        return;
    }

    //VisionData.Gravity_Offset.y =
    //	/*(pow(VisionData.RawData.depth, 3) *0.0004088 -
    //		0.2795 * pow(VisionData.RawData.depth, 2) +
    //		74.2*VisionData.RawData.depth - 1316) / (VisionData.RawData.depth - 30.77)+*/;

    //ȷ���������ڼ����ƫ��ֵ
    VisionData.Final_Offset.x = VisionData.RawData.x - VisionData.Gravity_Offset.x;
    VisionData.Final_Offset.y = VisionData.RawData.y - VisionData.Gravity_Offset.y;

    /*
f(x) = (p1*x^3 + p2*x^2 + p3*x + p4) / (x + q1)
Coefficients:
	   p1 =   0.0004088
	   p2 =     -0.2795
	   p3 =        74.2
	   p4 = - 1316
	   q1 =      -30.77
	*/
#endif
}

/*************************************
* Method:    Vision_getOffset
* Returns:   int16_t
* Parameter: int16_t length
* Parameter: int16_t Value
* ˵��������value �� ����ϵ�е��ƫ��ֵ��
************************************/
int16_t Vision_getOffset(int16_t length, int16_t Value)
{
    return Value - length / 2;
}

/*һ���Ƕ�ȡSR�Ĵ�����һ���Ƕ�ȡ��Ӧ��CR���ƼĴ���*/
/*���������CR����SR���������Ҫ��ȡ��Ӧ�ı�־λ�Ļ����ȿ��Դ�CR��ȡҲ���Դ�SR��ȡ*/
/*__HAL_UART_GET_FLAG�ǻ�ȡSR�Ĵ�������������Ҳ���Ƕ�ȡ��CR����������֮��Ķ�Ӧ״̬*/
/*__HAL_UART_GET_IT_SOURCE��ֱ�Ӷ�ȡ���ƼĴ��������CRx��־λ�����*/
/*�����DMA_GET_COUNTER�ǻ�ȡ��û����ȥ���ַ���������֮ǰ�Ĳ�ͬ*/
/*��������������ĶԱȣ�����ϸ�Ķ�*/
/**
 * @Data    2019-03-23 20:07
 * @brief   �Ӿ�������
 * @param   uint8_t *pData
 * @retval  void
 */
void Vision_Handler(UART_HandleTypeDef *huart)
{
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_DMA_DISABLE(huart->hdmarx);

    Vision_DataReceive(Vision_DataBuff);

    __HAL_DMA_SET_COUNTER(huart->hdmarx, Vision_BuffSIZE);
    __HAL_DMA_ENABLE(huart->hdmarx);
}

/**
 * @brief USART_DMA���տ������ض���
 * 
 * @param huart 
 * @param pData 
 * @param Size 
 * @return  
 */
static int USART_Receive_DMA_NO_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint32_t Size)
{

    /*��⵱ǰhuart״̬*/
    if (huart->RxState == HAL_UART_STATE_READY)
    {
        /*����ĵ�ַ��������������Ļ�*/
        if ((pData == NULL) || (Size == 0))
        {
            return HAL_ERROR;
        }

        /*huart�����Ӧ��Rx�����ض���*/
        huart->pRxBuffPtr = pData;
        huart->RxXferSize = Size;
        huart->ErrorCode = HAL_UART_ERROR_NONE;

        /*����huart1�ϵ�RX_DMA*/
        HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)pData, Size);

        /*ֻ������ӦDMA�����Rx���ܣ�����ǿ���Tx�Ļ�����USART_CR3_DMAT��*/
        SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    }
    else
    {
        return HAL_BUSY;
    }

    return HAL_OK;
}

/**
 * @brief �Ӿ�ʹ��DMA-USART
 * 
 * @param huartx 
 */
void Vision_USART_Receive_DMA(UART_HandleTypeDef *huartx)
{
    /*��ձ�־λȻ��ʹ��USART���ж�*/
    __HAL_UART_CLEAR_IDLEFLAG(huartx);
    __HAL_UART_ENABLE(huartx);
    __HAL_UART_ENABLE_IT(huartx, UART_IT_IDLE);
    // assert(Vision_BuffSIZE == (13 + 2));
    USART_Receive_DMA_NO_IT(huartx, Vision_DataBuff, Vision_BuffSIZE);
}

/**
 * @brief Get the Vision Disc Mode object
 * 
 * @return  
 */
uint8_t GetVisionDiscMode(void)
{
    return VisionData.RawData.mode;
}

/**
 * @brief Get the Vision Hit Mode object
 * 
 * @return  
 */
uint8_t GetVisionHitMode(void)
{
    uint8_t HitMode = 0;
    if (Robot.Attack_ShootTarget == ShootTarget_Self_aiming || Robot.Attack_ShootTarget == ShootTarget_Sentry)
    {
        HitMode = 1;
    }
    else
    {
        HitMode = 0;
    }

    return HitMode;
}

/**
 * @brief �Ӿ����
 * 
 */
void Check_Vision(void)
{
    //�Ӿ����� ---------------------------
    if (VisionData.InfoUpdateFrame < 1)
    {
        VisionData.OffLineFlag = 1;
    }
    else
    {
        VisionData.OffLineFlag = 0;
    }
    VisionData.InfoUpdateFrame = 0;
}

/**
 * @brief �Ӿ�pitch�����仯
 * 
 * @param CPData 
 */
void Vision_CP(float CPData)
{
    VisionData.Gravity_Offset.y += CPData;
}

/**
 * @brief ��ȡ�Ӿ�pitch�����仯
 * 
 * @return  
 */
float GetVision_CP(void)
{
    return VisionData.Gravity_Offset.y;
}

