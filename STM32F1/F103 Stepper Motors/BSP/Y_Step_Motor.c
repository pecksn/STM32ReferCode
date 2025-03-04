#include "Y_Step_motor.h"
#include "tim.h"

/***************************************************************************************************************
  * �������ƣ� GetTData
  * ����������  ����y=a*x+b���Լ���,б��a=(SpeedMax-SpeedMin)/nAccTime,b=SpeedMin,nAccTime��λΪms,speed��/��
  * ���� pTData ���ɵļ�������
  * ����ֵ������ĳ���
  * ����˵���� 
  * 
****************************************************************************************************************/

//void Y_motor_init(){
//	unsigned short pTData[500]={0};
//}
unsigned short pTData[500]={0};

int CurrentSpeed=0;

pSmCtrl_typedef ppSmCtrl={0};
pSmCtrl_typedef *pSmCtrl=&ppSmCtrl;

short GetTData(unsigned short* pTData,short SpeedMin,short SpeedMax,short nAccTime)
{
	short TabLen= 0;	
	double tSum=0;
	short RealSpeed=SpeedMin;//ʵʱ�ٶȳ�ʼֵ
	unsigned int nAccTimer = nAccTime*1000*72;	//����ʱ���Ӧ�Ķ�ʱ������ֵ:1S��ʱ����ֵ�仯72M�Σ���ô1ms�ͱ仯72000��                                     
	short deltaSpeed = (SpeedMax-SpeedMin); 	//Ϊ�˼ӿ�����ٶȣ�����Ԥ�ȼ���б����ֵ��һ����
	
	while(RealSpeed<SpeedMax)
	{
		RealSpeed = deltaSpeed*tSum/nAccTimer+SpeedMin;//deltaSpeed/nAccTimer��б��a��SpeedMin��b
		pTData[TabLen] = 72000000/2*RealSpeed;//���ڷ�һ���ߵ�ƽȻ��һ���͵�ƽ�������һ�������Ե�����ƽ���ٶ���Ҫ��2
		tSum  += 2*pTData[TabLen];		//��һ��                                                                                                 ��Ҫһ���ߵ�ƽ�͵͵�ƽ����*2 
		TabLen++; 
	}

	return TabLen;
}
/***************************************************************************************************************
  * �������ƣ�  ·���滮
  * ���������� speed-�����ٶȣ�step-��Ҫ�ߵĲ���
  *speed �ǲ��������Ŀ���ٶȣ���λ������/�롣���������ʾ����Ҫ���õĲ�������������ٶ�
  * ����˵���� 
  * 
****************************************************************************************************************/
void PlanMovePath(unsigned short speed,unsigned int step,unsigned int TabLen,unsigned int SpeedMax)
{	

	extern unsigned short pTData[500];
	unsigned short nTimerSpeed = 72000000/2*speed;	//speed 1S������� 12M
	unsigned short TabIndex =0;
	for(int i=0;i<TabLen;i++)
	{
		if(pTData[i]<=nTimerSpeed)  
		{
			TabIndex = i;
			break;
		}
	}

	if(speed>SpeedMax)
	{
		TabIndex = TabLen;
	}

	if((TabIndex) <= (step>>1))	//�����߷�,ȡstep��һ��
	{
		pSmCtrl->AccStep = TabIndex;
		pSmCtrl->ConstStep = step - (TabIndex)*2;
		pSmCtrl->DecStep = pSmCtrl->AccStep;
	}
	else//������
	{
		pSmCtrl->AccStep = (step>>1);
		pSmCtrl->ConstStep = 0;
		pSmCtrl->DecStep = step-pSmCtrl->AccStep;
	}
	
	if(step==1)
	{
		pSmCtrl->AccStep = 1;
		pSmCtrl->ConstStep = 0;
		pSmCtrl->DecStep = 0;
	}
}

void ReloadTimer(TIM_HandleTypeDef *htim, uint32_t period)
{
    // ������װ��ֵ
    uint32_t reloadValue = (PWM_FREQUENCE / period) - 1;

    // ֹͣ��ʱ��
    HAL_TIM_Base_Stop(htim);

    // ������װ��ֵ
    htim->Instance->ARR = reloadValue;

    // �����ʱ���жϱ�־λ
    __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);

    // ������ʱ��
    HAL_TIM_Base_Start(htim);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	return;
}
if (pSmCtrl->AccStep != 0)      //���ٽ׶�
{
    pSmCtrl->AccStep--;
    ReloadTimer(&htim2, pTData[CurrentSpeed]);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}
else if (pSmCtrl->ConstStep != 0)    //���ٽ׶�
{
    pSmCtrl->ConstStep--;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}
else if (pSmCtrl->DecStep != 0)  //���ٽ׶�
{
    pSmCtrl->DecStep--;
    if (CurrentSpeed > 0)
    {
        CurrentSpeed--;
    }

    ReloadTimer(htim, pTData[CurrentSpeed]);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}
else
{
    HAL_TIM_Base_Stop(&htim2);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);    
}
}

