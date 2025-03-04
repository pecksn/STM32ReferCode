#include "DJI_C_IMU.h"

//���ж�������� ��Ҫ��ͬ��ID����ID��ͻ�����Ͱ����Ϊ����

//��������������
DJI_C_Euler_u DJI_C_Euler_Receive /* [] */;
DJI_C_Gyro_u DJI_C_Gyro_Receive; /* [] */
//���ݴ���ṹ��
DJI_C_IMU_t DJI_C_IMU /*DJI_C_IMU[]*/;
//֡�ʽṹ��
WorldTime_RxTypedef gyro_FPS;
uint32_t Gyro_FPS;
WorldTime_RxTypedef euler_FPS;
uint32_t Euler_FPS;

void Updata_Hand_Euler_Gyro_Data(void);
void DJI_C_Euler_getInfo(Can_Export_Data_t CAN_Rx_Structure);
void DJI_C_Gyro_getInfo(Can_Export_Data_t CAN_Rx_Structure);
void Check_DJI_C_IMU(void);
void CIMU_Reset(DJI_C_IMU_t *DJI_C_IMU);

DJI_C_IMUFUN_t DJI_C_IMUFUN = DJI_C_IMUFUNGroundInit;
#undef DJI_C_IMUFUNGroundInit

//�����������
void Updata_Hand_Euler_Gyro_Data(void)
{
    //�Ƕ�
    DJI_C_IMU.yaw = -((float)DJI_C_Euler_Receive.yaw * Angle_turn_Radian + 180.0f);    //������תΪ��
    DJI_C_IMU.pitch = (float)DJI_C_Euler_Receive.pitch * Angle_turn_Radian * 22.7527f; //(-90�� ~ 90��)
    //���ٶ�
    DJI_C_IMU.Gyro_z = DJI_C_Gyro_Receive.Gyro_z * Angle_turn_Radian;
    DJI_C_IMU.Gyro_y = DJI_C_Gyro_Receive.Gyro_y * Angle_turn_Radian;

    //yaw��Ĺ��㴦��
    if (DJI_C_IMU.yaw - DJI_C_IMU.last_yaw < -300.0f)
    {
        DJI_C_IMU.turnCounts++;
    }
    if (DJI_C_IMU.last_yaw - DJI_C_IMU.yaw < -300.0f)
    {
        DJI_C_IMU.turnCounts--;
    }
    DJI_C_IMU.total_yaw = DJI_C_IMU.yaw + DJI_C_IMU.turnCounts * 360.0f;

    DJI_C_IMU.last_yaw = DJI_C_IMU.yaw;
}

//���DJI_C_IMU��Euler
void DJI_C_Euler_getInfo(Can_Export_Data_t CAN_Rx_Structure)
{
    if (CAN_Rx_Structure.CAN_RxHeader.StdId != DJI_C_Angle)
    {
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        DJI_C_Euler_Receive.BraodData[i] = CAN_Rx_Structure.CANx_Export_RxMessage[i];
    }

    DJI_C_Euler_Receive.InfoUpdateFrame++;
    //��ȡ��֡��
    Get_FPS(&euler_FPS, &Euler_FPS);
}

//���DJI_C_IMU��Gyro
void DJI_C_Gyro_getInfo(Can_Export_Data_t CAN_Rx_Structure)
{
    if (CAN_Rx_Structure.CAN_RxHeader.StdId != DJI_C_Gyro)
    {
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        DJI_C_Gyro_Receive.BraodData[i] = CAN_Rx_Structure.CANx_Export_RxMessage[i];
    }
    //��ȡ��֡��
    Get_FPS(&gyro_FPS, &Gyro_FPS);
}

void CIMU_Reset(DJI_C_IMU_t *DJI_C_IMU)
{
    DJI_C_IMU->last_yaw = DJI_C_IMU->Gyro_z;
    DJI_C_IMU->total_yaw = DJI_C_IMU->Gyro_z;
    DJI_C_IMU->turnCounts = 0;
}

void Check_DJI_C_IMU(void)
{
    //C�������� ---------------------------
    if (DJI_C_Euler_Receive.InfoUpdateFrame < 1)
    {
        DJI_C_IMU.OffLineFlag = 1;
    }
    else
    {
        DJI_C_IMU.OffLineFlag = 0;
    }
    DJI_C_Euler_Receive.InfoUpdateFrame = 0;
}
