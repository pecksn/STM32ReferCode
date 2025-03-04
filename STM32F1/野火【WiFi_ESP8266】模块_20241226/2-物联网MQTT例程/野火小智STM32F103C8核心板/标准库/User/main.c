/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2024-xx-xx
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火小智 STM32F103C8 核心板
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
#include "bsp_esp8266_mqtt.h"


uint8_t publish_flag =0;//发布任务标志

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
    /* 初始化嘀嗒定时器 */
    SysTick_Init ();
    /* 初始化USART1 配置模式为 115200 8-N-1 */
    USART1_Config ();
    printf("使用该例程请先将esp8266烧录1MB MQTT透传AT固件");
	printf("自行到bsp_esp8266_test.h头文件修改热点名称密码、MQTT服务器等相关参数");
    /* 初始化DWT计数器，用于延时函数 */
    CPU_TS_TmrInit();
    /* 初始化LED */
    LED_GPIO_Config();
    /* 初始化DTT11的引脚 */
    DHT11_Init ();
    /* 初始化WiFi模块使用的接口和外设 */
    ESP8266_Init ();
    /* 连接物联网平台测试 */
    ESP8266_StaTcpClient_Unvarnish_ConfigTest();
    
    while ( 1 )
    {
        if(publish_flag == 1 && mqtt_flag == 1 )//发布任务时间到
        {
            ESP8266_SendDHT11DataTest();//发布DHT11数据
            publish_flag = 0;
        }
    }
	
}


/*********************************************END OF FILE**********************/
