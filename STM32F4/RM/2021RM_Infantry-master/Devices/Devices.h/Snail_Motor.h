/**
 * @file Snail_Motor.h
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __Snail_MOTOR_H
#define __Snail_MOTOR_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <Math.h>
#include <string.h>  //�����˵�Ĭ�������ļ���
#include "tim.h"
#include <AddMath.h>


#define Snail_Speed_FunGroundInit               \
	{                                   \
		&C615_CaptureSpeed,                   \
			&C615_ObtainSpeed, \
	}

#define One 0
#define Two 1

typedef struct
{

	long long realSpeed;			//���������ٶ�

	uint16_t  output;//���͸������ֵ��

}Snail_t; //Ħ���ֶ���

typedef struct
{
	uint8_t STA;          //���벶��״̬		  
	uint16_t Date1;  //���벶��ֵ
	uint16_t Date2;  //���벶��ֵ

	long long temp;
}TIMxCHx_CAPTURE_t; //��ʱ�����񻺴�ֵ

typedef struct 
{
	void (*C615_CaptureSpeed)(TIM_TypeDef *TIMx, uint8_t One_Two);
	void (*C615_ObtainSpeed)(TIMxCHx_CAPTURE_t *TIMxCHx_CAPTURE, Snail_t *Snail_Power);
}Snail_Speed_Fun_t;

extern Snail_Speed_Fun_t Snail_Speed_Fun;

extern Snail_t Snail_PowerL;
extern Snail_t Snail_PowerR;
extern TIMxCHx_CAPTURE_t TIM4CH1_CAPTURE;
extern TIMxCHx_CAPTURE_t TIM4CH2_CAPTURE;
extern TIMxCHx_CAPTURE_t TIM4CH3_CAPTURE;
extern TIMxCHx_CAPTURE_t TIM4CH4_CAPTURE;



#endif
