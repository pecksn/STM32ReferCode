/**
 * @file PowerLimit.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "PowerLimit.h"
#include "RM_JudgeSystem.h"
#include "Wolf_GyIMU.h"

PowerLimit_t chassis_powerLimit;
int16_t power_buffer = 0;
void PowerLimit_Calculate(PowerLimit_t *powerlimit)
{

    power_buffer = ext_power_heat_data.data.chassis_power_buffer; //����������С ������Դ��

    if (RM_Judge.OffLineFlag == 1)
    {
        power_buffer = 120;
    }

    int16_t powerBuffErr = 60 - power_buffer; //�õ��Ļ���������

    powerlimit->powerBuffRatio = 0;
    powerlimit->powerBuffRatio = (float)power_buffer / 76.0f; //�õ��Ļ���������

    powerlimit->powerBuffRatio *= powerlimit->powerBuffRatio; //�����Ĺ�ϵ��
    //powerlimit->powerBuffRatio *= powerlimit->powerBuffRatio;//�����Ĺ�ϵ��

    if (powerBuffErr > 0)
    {
        powerlimit->SumCurrent_Out = powerlimit->SumCurrent_IN * powerlimit->powerBuffRatio;
    }
}

// void

void PowerLimit(PowerLimit_t *powerlimit, int16_t *wheelCurrent, int16_t amount)
{
    //float *coe = calloc(4, sizeof(float));//ϵ��	��̬���������ڴ档

    float coe[4] = {0.0}; //ϵ��	��̬���������ڴ档
//    float Wheelcoe[2][2] = {{0, 0}, {0.0}};
    int i = 0;
//    int32_t Wheel_IN[2] = {0};
    powerlimit->SumCurrent_IN = powerlimit->SumCurrent_Out = 0;

    // if (Wolf_GyIMU_Fun.GY6050_PitErr() > -20 && Wolf_GyIMU_Fun.GY6050_PitErr() < -10)
    // {
    //     //����ǰ�ֵ���
    //     for (i = 0; i < amount / 2; i++)
    //     {
    //         Wheel_IN[0] += abs(wheelCurrent[i]);
    //     }
    //     //����ǰ�ְٷֱ�
    //     for (i = 0; i < amount / 2; i++)
    //     {
    //         Wheelcoe[0][i] = ((float)(wheelCurrent[i])) / ((float)(Wheel_IN[0]));
    //     }
    //     //��Ҫ������ֵĹ���
    //     int32_t temp_FWheel = (int32_t)Wheel_IN[0] * 0.3f;
    //     Wheel_IN[0] -= temp_FWheel;
    //     //�����������Ӹ��еĹ���
    //     for (i = 0; i < amount / 2; i++)
    //     {
    //         wheelCurrent[i] = (int16_t)(Wheel_IN[0] * Wheelcoe[0][i]);
    //     }

    //     //������ֵ���
    //     for (i = amount / 2; i < amount; i++)
    //     {
    //         Wheel_IN[1] += abs(wheelCurrent[i]);
    //     }
    //     //������ְٷֱ�
    //     for (i = amount / 2; i < amount; i++)
    //     {
    //         Wheelcoe[1][i] = ((float)(wheelCurrent[i])) / ((float)(Wheel_IN[1]));
    //     }
    //     //����ǰ�ֵĹ���
    //     Wheel_IN[1] += temp_FWheel;
    //     //�����������Ӹ��еĹ���
    //     for (i = amount / 2; i < amount; i++)
    //     {
    //         wheelCurrent[i] = (int16_t)(Wheel_IN[1] * Wheelcoe[1][i]);
    //     }
    // }

    //�����ܵ���
    for (i = 0; i < amount; i++)
    {
        powerlimit->SumCurrent_IN += abs(wheelCurrent[i]);
    }
    powerlimit->SumCurrent_Out = powerlimit->SumCurrent_IN; //������ʱΪԭ����ֵ��

    //����ÿ������ռ�ܵ����İٷֱ�
    for (i = 0; i < amount; i++)
    {
        coe[i] = ((float)(wheelCurrent[i])) / ((float)(powerlimit->SumCurrent_IN));
    }

    //�����ʱ���ƹ��ʺ��������
    PowerLimit_Calculate(powerlimit); //�ڲ�ָ��ķ�ʽ�޸� SumCurrent_Out

    //���հٷֱȷ���������
    for (i = 0; i < amount; i++)
    {
        wheelCurrent[i] = (int16_t)(powerlimit->SumCurrent_Out * coe[i]);
    }
}
