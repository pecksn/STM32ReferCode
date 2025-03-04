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
  * ʵ��ƽ̨:Ұ��С�� STM32F103C8 ���İ�
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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


uint8_t publish_flag =0;//���������־

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main ( void )
{
    /* ��ʼ����શ�ʱ�� */
    SysTick_Init ();
    /* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
    USART1_Config ();
    printf("ʹ�ø��������Ƚ�esp8266��¼1MB MQTT͸��AT�̼�");
	printf("���е�bsp_esp8266_test.hͷ�ļ��޸��ȵ��������롢MQTT����������ز���");
    /* ��ʼ��DWT��������������ʱ���� */
    CPU_TS_TmrInit();
    /* ��ʼ��LED */
    LED_GPIO_Config();
    /* ��ʼ��DTT11������ */
    DHT11_Init ();
    /* ��ʼ��WiFiģ��ʹ�õĽӿں����� */
    ESP8266_Init ();
    /* ����������ƽ̨���� */
    ESP8266_StaTcpClient_Unvarnish_ConfigTest();
    
    while ( 1 )
    {
        if(publish_flag == 1 && mqtt_flag == 1 )//��������ʱ�䵽
        {
            ESP8266_SendDHT11DataTest();//����DHT11����
            publish_flag = 0;
        }
    }
	
}


/*********************************************END OF FILE**********************/
