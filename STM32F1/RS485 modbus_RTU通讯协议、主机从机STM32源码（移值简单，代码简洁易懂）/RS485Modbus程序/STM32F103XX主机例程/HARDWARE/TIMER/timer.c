/****************************************************************************************************************
@文件名称：timer.c
@功能说明：初始化定时器3 及定时器3中断服务函数 用于时基
@版    本:V1.0.0
@编    写:
@修改日志:
****************************************************************************************************************/
#include "timer.h"
#include "rs485.h"
#include "led.h"

/***************************************************************************************************************
@函数名称：定时器3初始化设置函数
@入口参数：arr->自动重装值
		   psc->时钟除数 预分频值
@出口参数：无
@功能说明：初始化定时器3
***************************************************************************************************************/
void TIMER3_Init(unsigned int arr,unsigned int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//--------------使能TIM3时钟 

	TIM_TimeBaseStructure.TIM_Period = arr;//---------------------------自动重装寄存器的值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc;//-------------------------时钟除数预分频的值 	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//----------------------时钟分割 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//-------向上计数模式 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//-------------------初始TIM3时基 
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );//-------------------------使能TIME3中断 充许更新中断 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//-------------------中断选择TIM3 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//---------占先优先级0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//----------------从优先级3 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//-------------------IRQ通道使能 
	NVIC_Init(&NVIC_InitStructure);//-----------------------------------初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);//--------------------------------------------使能TIM3外设
}
/***************************************************************************************************************
@函数名称：定时器3中断处理函数
@入口参数：无
@出口参数：无
@功能说明：每次进入中断置位时基标志 同时翻转LED1状态 用于提示程序工作状态
***************************************************************************************************************/
void TIM3_IRQHandler(void)  
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//----------------检查中断源是否为TIM3
	{
		UartRxMonitor(1);
		LedPoint(500);//-----------------------------------------------LED指示灯周期闪烁
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//------------------清除TIM3中断
	}
}
