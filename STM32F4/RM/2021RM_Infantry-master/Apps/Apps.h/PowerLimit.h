/**
 * @file PowerLimit.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __PowerLimit_H
#define __PowerLimit_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <AddMath.h>

typedef struct
{
	int32_t SumCurrent_IN;//����ĵ����ܺ�
	int32_t SumCurrent_Out;//����������ܺ�

	//float LimitPower; //��ǰ���ƵĹ��ʴ�С ��λW    

	float powerBuffRatio;

	//����ϵͳ������
	uint16_t  *Real_chassis_volt;                   //���������ѹ ��λ ����
	uint16_t *Real_chassis_current;                //����������� ��λ ����
	float *Real_chassis_power;
}PowerLimit_t;

extern PowerLimit_t chassis_powerLimit;
void PowerLimit(PowerLimit_t *powerlimit, int16_t * wheelCurrent, int16_t amount);

#endif
