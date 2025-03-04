/*************************************************************************************
文件名称：main.c
版    本：V1.0
编    著：Eric Xie
说    明：本例程只为实现485通讯  例程主要实现逻和方法，至于更多应用，可在此基础上自行增加
		  这里是从机例程

**************************************************************************************/

/* 所需头文件 */
#include "led.h"		
#include "delay.h"		
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "rs485.h"
#include "demo.h"



int main(void)
{ 
	delay_init();	    							//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//设置系统中断优先级分组4	 
	uart_init(115200);								//初始化串口1 用于打印调试 波特率为115200
	LED_Init();		  								//初始化LED
	RS485_Init(9600);								//初始化485串口 波物率为9600
	TIMER3_Init(999,71);							//定时器3初始化 周期1ms
	
	while(1)
	{
		UartDriver();								//串口数据处理
		TestOpr();									//测试函数
	}
}
