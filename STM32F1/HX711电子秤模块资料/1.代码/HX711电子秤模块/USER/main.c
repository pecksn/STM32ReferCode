#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "hx711.h"

/*****************辰哥单片机设计******************
											STM32
 * 项目			:	HX711电子秤模块实验                     
 * 版本			: V1.0
 * 日期			: 2024.9.11
 * MCU			:	STM32F103C8T6
 * 接口			:	参看HX711.h							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥 

**********************BEGIN***********************/

int value;
float weight;
int32_t reset;
u8 buff[30];

float Weights=100.0;  //100g
int32_t Weights_100=8493860;  //100g

int main(void)
{ 
	
  SystemInit();//配置系统时钟为72M	
	delay_init(72);
	LED_Init();
	LED_On();
	HX711_Init();
	USART1_Config();//串口初始化
	
	OLED_Init();
	printf("Start \n");
	delay_ms(1000);
	//显示“重量:”
	OLED_ShowChinese(0,0,0,16,1);
	OLED_ShowChinese(16,0,1,16,1);
	OLED_ShowChar(32,0,':',16,1);
	
	reset = HX711_GetData();

  while (1)
  {
		LED_Toggle();
		value = HX711_GetData();
		
		weight=(float)(value-reset)*Weights/(float)(Weights_100-reset);
		
		sprintf((char*)buff,"%.1f   ",weight);
		
		printf("重量: %f\r\n",weight);
		
		OLED_ShowString(50,30,buff,16,1);

		delay_ms(500);
  }
}




