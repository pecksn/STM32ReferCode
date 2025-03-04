#include "TIM.h"

/**
 * @brief Ħ����pwm
 * @note  TIM3
 */
void PWM_Friction_Init(u16 arr,u16 psc)
{		
	GPIO_InitTypeDef PWM_GPIO;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3ʱ��ʹ��    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM3ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ��PORTAʱ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //ʹ��PORTAʱ��	

	
	PWM_GPIO.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //GPIOA6                     
	PWM_GPIO.GPIO_Mode = GPIO_Mode_AF;//���ù���
	PWM_GPIO.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_Init(GPIOA,&PWM_GPIO); //��ʼ��PA
  
	PWM_GPIO.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13; //GPIOA6                     
	PWM_GPIO.GPIO_Mode = GPIO_Mode_AF;//���ù���
	PWM_GPIO.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_Init(GPIOD,&PWM_GPIO); //��ʼ��PD 
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); //GPIOA6����λ��ʱ��3 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3); //GPIOA7����λ��ʱ��3                
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4); //GPIOD12����λ��ʱ��4 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4); //GPIOD13����λ��ʱ��4                
  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;     //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //0
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//��ʼ����ʱ��3
  TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//��ʼ����ʱ��3
  
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse=0;
  
  //��
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//
  
  TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPEʹ��
  TIM_CtrlPWMOutputs(TIM3,ENABLE);//�����ʹ��
  TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3

  //��
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);//
  
  TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPEʹ�� 	
  TIM_CtrlPWMOutputs(TIM4,ENABLE);//�����ʹ��
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM3
} 



/**
 * @brief ���pwm
 * @note  TIM1
 */
void PWM_Clip_Init(u16 arr,u16 psc)
{		
	GPIO_InitTypeDef PWM_GPIO;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	//TIM1ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //ʹ��PORTDʱ��	
	
	PWM_GPIO.GPIO_Pin = GPIO_Pin_11; //GPIOE11
	PWM_GPIO.GPIO_Mode = GPIO_Mode_AF;//���ù���
	PWM_GPIO.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_Init(GPIOE,&PWM_GPIO); //��ʼ��PE11
 
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1); //GPIOE11����λ��ʱ��1

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;     //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //0
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//��ʼ����ʱ��1
 
	//��ʼ��TIM4 Channel 1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC2

  TIM_CtrlPWMOutputs(TIM1,ENABLE);//�����ʹ��

	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
 
  TIM_ARRPreloadConfig(TIM1,ENABLE);//ARPEʹ�� 
  
	//TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
  
  TIM1->CCR2 = 0;
} 



void Trigger_Cnt_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///ʹ��TIM5ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//��ʼ��TIM5
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //����ʱ��5�����ж�
	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��5
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; // KEY��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB0 B1
} 

//����Ϊ�͵�ƽ
int cnt_LD = 0, cnt_RD = 0,cnt_LU = 0, cnt_RU = 0;
KEY_STATE_t key_state_L = up,key_state_R = up;
void KEY_SCAN(void)
{
  if(KEY_Left == 0)
    cnt_LD++;
  if(KEY_Righ == 0)
    cnt_RD++;
  if(KEY_Left == 1)
    cnt_LU++;
  if(KEY_Righ == 1)
    cnt_RU++;
  
  if(cnt_LD >= 5)
  {
    key_state_L = down;
    cnt_LD = 0;
  }
  if(cnt_RD >= 5)
  {
    key_state_R = down;
    cnt_RD = 0;
  }
  if(cnt_LU >= 5)
  {
    key_state_L = up;
    cnt_LU = 0;
  }
  if(cnt_RU >= 5)
  {
    key_state_R = up;
    cnt_RU = 0;
  }
}

bool Report_IF_TurnLeft_allow(void)
{
  bool res = NO;
  if(key_state_L == up)
    res = YES;
  else
    res = NO;
  res = YES;//��ʹ�ÿ����ж������
  return res;
}
bool Report_IF_TurnRigh_allow(void)
{
  bool res = NO;
  if(key_state_R == up)
    res = YES;
  else
    res = NO;
  res = YES;//��ʹ�ÿ����ж�
  return res;
}



