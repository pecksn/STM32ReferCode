
/* 头文件 */
#include "led.h"


/****************************************************************************************************
 * 函数名称： void LED_Init(void)
 * 入口参数：无
 * 返回  值：无
 * 功能说明：初始化LED灯对应的GPIO引脚
 * 			 设置为推挽输出，IO翻转速度为50MHz
 *			 使用库函数实现，结合硬件原理
 *			 在led.h中有具体的定义
 *			 如硬件连接不同，可在led.h中更改即可
 ***************************************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_RCC | RCC_APB2Periph_AFIO, ENABLE);	//打开LED端口时钟 和复用时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 		//把PB.3.4 PA15 用作普通IO口 这里要注意

	GPIO_InitStructure.GPIO_Pin = LED_PIN;				//设置LED引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//LED引脚模式->推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//LED引脚翻转速度->50MHz
	GPIO_Init(LED_PROT, &GPIO_InitStructure);			//根据设定参数初始化LED对应的GPIO
	GPIO_SetBits(LED_PROT,LED_PIN);						//黙认输出为低 关闭LED灯
}

/****************************************************************************************************
 * 函数名称： void LedPoint(u16 uiCycle)
 * 入口参数：u16 uiCycle
 * 返回  值：无
 * 功能说明：led周期闪烁处理函数，在定时器中调用，uiCycle的值 = 定时器中断的周期
 ***************************************************************************************************/
void LedPoint(u16 uiCycle)
{
	static u16 uiCnt = 0;
	
	uiCnt ++;
	if (uiCnt == uiCycle)
	{
		uiCnt = 0;
		LED ^= 1;
	}
}
/* END */
 
