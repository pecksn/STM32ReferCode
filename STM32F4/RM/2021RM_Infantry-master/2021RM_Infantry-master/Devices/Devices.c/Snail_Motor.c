/**
 * @file Snail_Motor.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Snail_Motor.h"

Snail_t Snail_PowerL; //��ߵ�Ħ���֡�
Snail_t Snail_PowerR; //��ߵ�Ħ���֡�

//�������ݲ���������ȡ�ٶȡ�
uint8_t TIM4CH1_CAPTURE_STA;	//���벶��״̬
uint16_t TIM4CH1_CAPTURE_Date1; //���벶��ֵ
uint16_t TIM4CH1_CAPTURE_Date2; //���벶��ֵ

uint8_t TIM4CH2_CAPTURE_STA;	//���벶��״̬
uint16_t TIM4CH2_CAPTURE_Date1; //���벶��ֵ
uint16_t TIM4CH2_CAPTURE_Date2; //���벶��ֵ
long long temp1 = 0;
long long temp2 = 0;

TIMxCHx_CAPTURE_t TIM4CH1_CAPTURE;
TIMxCHx_CAPTURE_t TIM4CH2_CAPTURE;
TIMxCHx_CAPTURE_t TIM4CH3_CAPTURE;
TIMxCHx_CAPTURE_t TIM4CH4_CAPTURE;

void C615_CaptureSpeed(TIM_TypeDef *TIMx, uint8_t One_Two);
void C615_ObtainSpeed(TIMxCHx_CAPTURE_t *TIMxCHx_CAPTURE, Snail_t *Snail_Power);

Snail_Speed_Fun_t Snail_Speed_Fun = Snail_Speed_FunGroundInit;
#undef Snail_Speed_FunGroundInit

/**
  * @brief  ��ȡC615�ٶȣ�����Ҫ��ȡ�ٶȵ�λ����ʹ��
  * @param  TIMxCHx_CAPTURE_t *TIMxCHx_CAPTURE, Snail_t *Snail_Power
  * @retval None
  */
void C615_ObtainSpeed(TIMxCHx_CAPTURE_t *TIMxCHx_CAPTURE, Snail_t *Snail_Power)
{
	if (TIMxCHx_CAPTURE->STA & 0X80) //�ɹ�������һ�θߵ�ƽ
	{
		TIMxCHx_CAPTURE->temp = TIMxCHx_CAPTURE->STA & 0X3F;
		TIMxCHx_CAPTURE->temp *= 65536; //���ʱ���ܺ�
		TIMxCHx_CAPTURE->temp += TIMxCHx_CAPTURE->Date2;
		TIMxCHx_CAPTURE->temp -= TIMxCHx_CAPTURE->Date1; //�õ��ܵĸߵ�ƽʱ��
		if (abs(TIMxCHx_CAPTURE->temp) > 1800)
		{
			Snail_Power->realSpeed = Snail_Power->realSpeed;
		}
		else
		{
			Snail_Power->realSpeed = TIMxCHx_CAPTURE->temp;
		}
		TIMxCHx_CAPTURE->Date1 = 0;
		TIMxCHx_CAPTURE->STA = 0; //������һ�β���
	}
}

/**
* @brief �����һ��C615�ٶ�
* @param argument: Not used
* @retval None
*/
static void C615_CaptureSpeed_CH1CH2(TIM_TypeDef *TIM, TIMxCHx_CAPTURE_t *TIMxCH1_CAPTURE, TIMxCHx_CAPTURE_t *TIMxCH2_CAPTURE)
{
	uint16_t tsr;
	tsr = TIM->SR;
	//CH1�жϴ���
	if ((TIMxCH1_CAPTURE->STA & 0X80) == 0) //��δ�ɹ�����
	{
		if (tsr & 0X01) //���
		{
			if (TIMxCH1_CAPTURE->STA & 0X40) //�Ѿ����񵽸ߵ�ƽ��
			{
				if ((TIMxCH1_CAPTURE->STA & 0X3F) == 0X3F) //�ߵ�ƽ̫����
				{
					TIMxCH1_CAPTURE->STA |= 0X80; //��ǳɹ�������һ��
					TIMxCH1_CAPTURE->Date2 = 0XFFFF;
				}
				else
					TIMxCH1_CAPTURE->STA++;
			}
		}
		if (tsr & 0x02) //����1���������¼�
		{
			if (TIMxCH1_CAPTURE->STA & 0X40) //����һ���½���
			{
				TIMxCH1_CAPTURE->STA |= 0X80;		//��ǳɹ�����һ�θߵ�ƽ����
				TIMxCH1_CAPTURE->Date2 = TIM->CCR1; //��ȡ��ǰ�Ĳ���ֵ.
				TIM->CCER &= ~(1 << 1);				//CC1P=0 ����Ϊ�����ز���
			}
			else //��δ��ʼ,��һ�β���������
			{
				TIMxCH1_CAPTURE->Date2 = 0;
				TIMxCH1_CAPTURE->STA = 0X40; //��ǲ�����������
				TIMxCH1_CAPTURE->Date1 = TIM->CCR1;
				TIM->CCER |= 1 << 1; //CC1P=1 ����Ϊ�½��ز���
			}
		}
	}
	//CH2�жϴ���
	if ((TIMxCH2_CAPTURE->STA & 0X80) == 0) //��δ�ɹ�����
	{
		if (tsr & 0X01) //���
		{
			if (TIMxCH2_CAPTURE->STA & 0X40) //�Ѿ����񵽸ߵ�ƽ��
			{
				if ((TIMxCH2_CAPTURE->STA & 0X3F) == 0X3F) //�ߵ�ƽ̫����
				{
					TIMxCH2_CAPTURE->STA |= 0X80; //��ǳɹ�������һ��
					TIMxCH2_CAPTURE->Date2 = 0XFFFF;
				}
				else
					TIMxCH2_CAPTURE->STA++;
			}
		}
		if (tsr & 0x04) //����1���������¼�
		{
			if (TIMxCH2_CAPTURE->STA & 0X40) //����һ���½���
			{
				TIMxCH2_CAPTURE->STA |= 0X80;		//��ǳɹ�����һ�θߵ�ƽ����
				TIMxCH2_CAPTURE->Date2 = TIM->CCR2; //��ȡ��ǰ�Ĳ���ֵ.
				TIM->CCER &= ~(1 << 5);				//CC1P=0 ����Ϊ�����ز���
			}
			else //��δ��ʼ,��һ�β���������
			{
				TIMxCH2_CAPTURE->Date2 = 0;
				TIMxCH2_CAPTURE->STA = 0X40; //��ǲ�����������
				TIMxCH2_CAPTURE->Date1 = TIM->CCR2;
				TIM->CCER |= 1 << 5; //CC1P=1 ����Ϊ�½��ز���
			}
		}
	}
}

/**
* @brief ����ڶ���C615�ٶ�
* @param argument: Not used
* @retval None
*/
static void C615_CaptureSpeed_CH3CH4(TIM_TypeDef *TIM, TIMxCHx_CAPTURE_t *TIMxCH3_CAPTURE, TIMxCHx_CAPTURE_t *TIMxCH4_CAPTURE)
{
	uint16_t tsr;
	tsr = TIM->SR;
	//CH3�жϴ���
	if ((TIMxCH3_CAPTURE->STA & 0X80) == 0) //��δ�ɹ�����
	{
		if (tsr & 0X01) //���
		{
			if (TIMxCH3_CAPTURE->STA & 0X40) //�Ѿ����񵽸ߵ�ƽ��
			{
				if ((TIMxCH3_CAPTURE->STA & 0X3F) == 0X3F) //�ߵ�ƽ̫����
				{
					TIMxCH3_CAPTURE->STA |= 0X80; //��ǳɹ�������һ��
					TIMxCH3_CAPTURE->Date2 = 0XFFFF;
				}
				else
					TIMxCH3_CAPTURE->STA++;
			}
		}
		if (tsr & 0x08) //����1���������¼�
		{
			if (TIMxCH3_CAPTURE->STA & 0X40) //����һ���½���
			{
				TIMxCH3_CAPTURE->STA |= 0X80;		//��ǳɹ�����һ�θߵ�ƽ����
				TIMxCH3_CAPTURE->Date2 = TIM->CCR3; //��ȡ��ǰ�Ĳ���ֵ.
				TIM->CCER &= ~(1 << 9);				//CC1P=0 ����Ϊ�����ز���
			}
			else //��δ��ʼ,��һ�β���������
			{
				TIMxCH3_CAPTURE->Date2 = 0;
				TIMxCH3_CAPTURE->STA = 0X40; //��ǲ�����������
				TIMxCH3_CAPTURE->Date1 = TIM->CCR3;
				TIM->CCER |= 1 << 9; //CC1P=1 ����Ϊ�½��ز���
			}
		}
	}
	//CH4�жϴ���
	if ((TIMxCH4_CAPTURE->STA & 0X80) == 0) //��δ�ɹ�����
	{
		if (tsr & 0X01) //���
		{
			if (TIMxCH4_CAPTURE->STA & 0X40) //�Ѿ����񵽸ߵ�ƽ��
			{
				if ((TIMxCH4_CAPTURE->STA & 0X3F) == 0X3F) //�ߵ�ƽ̫����
				{
					TIMxCH4_CAPTURE->STA |= 0X80; //��ǳɹ�������һ��
					TIMxCH4_CAPTURE->Date2 = 0XFFFF;
				}
				else
					TIMxCH4_CAPTURE->STA++;
			}
		}
		if (tsr & 0x10) //����1���������¼�
		{
			if (TIMxCH4_CAPTURE->STA & 0X40) //����һ���½���
			{
				TIMxCH4_CAPTURE->STA |= 0X80;		//��ǳɹ�����һ�θߵ�ƽ����
				TIMxCH4_CAPTURE->Date2 = TIM->CCR4; //��ȡ��ǰ�Ĳ���ֵ.
				TIM->CCER &= ~(1 << 13);			//CC1P=0 ����Ϊ�����ز���
			}
			else //��δ��ʼ,��һ�β���������
			{
				TIMxCH4_CAPTURE->Date2 = 0;
				TIMxCH4_CAPTURE->STA = 0X40; //��ǲ�����������
				TIMxCH4_CAPTURE->Date1 = TIM->CCR4;
				TIM->CCER |= 1 << 13; //CC1P=1 ����Ϊ�½��ز���
			}
		}
	}
}

/**
  * @brief  ����ߣ��ͣ���ƽ����TIM�жϴ�ʹ��
  * @param  TIM_TypeDef *TIMx, uint8_t One_Two
  * @retval None
  */
void C615_CaptureSpeed(TIM_TypeDef *TIMx, uint8_t One_Two)
{
	C615_CaptureSpeed_CH1CH2(TIMx, &TIM4CH1_CAPTURE, &TIM4CH2_CAPTURE);
	if (One_Two == Two)
	{
		C615_CaptureSpeed_CH3CH4(TIMx, &TIM4CH3_CAPTURE, &TIM4CH4_CAPTURE);
	}

	TIMx->SR = 0; //����жϱ�־λ
}
