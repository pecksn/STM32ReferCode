/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   WF-ESP8266 WiFi模块测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "bsp_esp8266_test.h"
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "./dwt_delay/core_delay.h"



/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
    /* 初始化 */
    USART1_Config ();          //初始化串口1
//    CPU_TS_TmrInit();          //初始化DWT计数器，用于延时函数
    LED_GPIO_Config();         //初始化 LED 灯
    ESP8266_Init ();           //初始化WiFi模块使用的接口和外设
    
    
    printf("\r\n欢迎使用野火STM32开发板\r\n");
    printf("这是一个ESP8266AT指令测试实验\r\n");
    printf("请使用串口调试助手发送\"AT+换行回车\"测试ESP8266是否准备好\r\n");
    printf("更多AT指令请参考模块资料\r\n");
    printf("以下是ESP8266上电初始化打印的信息\r\n");
    
    macESP8266_CH_ENABLE();   //使能 ESP8266
//  LED1_ON;
//  LED2_ON;
//  LED3_ON;
  while ( 1 )
  {
    
    if(strUSART_Fram_Record .InfBit .FramFinishFlag == 1)  //如果接收到了串口调试助手的数据
		{
            
			strUSART_Fram_Record .Data_RX_BUF[strUSART_Fram_Record .InfBit .FramLength] = '\0';
			Usart_SendString(macESP8266_USARTx ,strUSART_Fram_Record .Data_RX_BUF);      //数据从串口调试助手转发到ESP8266
			LED2_ON;
            strUSART_Fram_Record .InfBit .FramLength = 0;                                //接收数据长度置零
			strUSART_Fram_Record .InfBit .FramFinishFlag = 0;                            //接收标志置零
	  }
		if(strEsp8266_Fram_Record .InfBit .FramFinishFlag)                             //如果接收到了ESP8266的数据
		{                                                      
			 strEsp8266_Fram_Record .Data_RX_BUF[strEsp8266_Fram_Record .InfBit .FramLength] = '\0';
			 Usart_SendString(DEBUG_USARTx ,strEsp8266_Fram_Record .Data_RX_BUF);        //数据从ESP8266转发到串口调试助手
			 strEsp8266_Fram_Record .InfBit .FramLength = 0;                             //接收数据长度置零
			 strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;                           //接收标志置零
		}
    
  }
	
}


/*********************************************END OF FILE**********************/
