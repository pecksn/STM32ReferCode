#include "./ESP8266/bsp_esp8266_test.h"
#include "./ESP8266/bsp_esp8266.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "./DHT11/bsp_dht11.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./ESP8266/bsp_esp8266_mqtt.h"

#define LED_CMD_NUMBER   8
char *ledCmd[8] = { "LED_RED","LED_GREEN","LED_BLUE","LED_YELLOW","LED_PURPLE","LED_CYAN","LED_WHITE","LED_RGBOFF" };


DHT11_Data_TypeDef DHT11_Data;

volatile uint8_t ucTcpClosedFlag = 0;

/**
  * @brief  ��ȡ����������ֺʹ��ڵ������ַ�������Ϣ
  * @param  ��
  * @retval ��
  */
void Get_ESP82666_Cmd( char * cmd)
{
	uint8_t i;
	for(i = 0;i < LED_CMD_NUMBER; i++)
	{
     if(( bool ) strstr ( cmd, ledCmd[i] ))
		 break;
	}
	switch(i)
    {
      case 0:
        LED_RED;
      break;
      case 1:
        LED_GREEN;
      break;
      case 2:
        LED_BLUE;
      break;
      case 3:
        LED_YELLOW;
      break;
      case 4:
        LED_PURPLE;
      break;
      case 5:
        LED_CYAN;
      break;
      case 6:
        LED_WHITE;
      break;
      case 7:
        LED_RGBOFF;
      break;
      default:
        
        break;      
    }   
}


/**
  * @brief  ESP8266 StaTcpClient Unvarnish ���ò��Ժ���
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void)
{
    printf( "\r\n�������� ESP8266 ......\r\n" );
    printf( "\r\nʹ�� ESP8266 ......\r\n" );
    macESP8266_CH_ENABLE();
    while( ! ESP8266_AT_Test() );
    
//    ESP8266_Cmd ( "AT+CIPSERVER=0", "OK", NULL, 5000 );    
    printf( "\r\n��ֹ������ ......\r\n" );
    while( ! ESP8266_Enable_MultipleId ( DISABLE ) );
    
    printf( "\r\n�������ù���ģʽ STA ......\r\n" );
    while( ! ESP8266_Net_Mode_Choose ( STA ) );
    while( ! ESP8266_DHCP_CUR () );
    
    
    printf( "\r\n�������� WiFi ......\r\n" );
    while( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
    
    //����ʹ��MQTT ATָ��ʵ������
    printf( "\r\n��������MQTT ......\r\n" );
    while( !ESP8266_MQTT_USERCFG ());
    
    printf( "\r\n��������MQTT ......\r\n" );
    while( !ESP8266_MQTT_CONN ());
    
    printf( "\r\n���ڶ���MQTT ......\r\n" );
    while( !ESP8266_MQTT_SUB ());
    
    mqtt_flag = 1;//mqtt������
    printf( "\r\n���� ESP8266 ���\r\n" );
    printf("�ȴ�����MQTT LED��������\r\n");
    
}


/**
  * @brief  ESP8266 ����DHT11���ݲ��Ժ���
  * @param  ��
  * @retval ��
  */
void ESP8266_SendDHT11DataTest(void)
{
    //��ȡ��ʪ��
    DHT11_Read_TempAndHumidity(&DHT11_Data);
    printf("\r\n\r\n��ȡDHT11�ɹ�!\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\nLED��״̬Ϊ %d\r\n", 
          DHT11_Data.humi_int, DHT11_Data.humi_deci, DHT11_Data.temp_int, DHT11_Data.temp_deci ,led_value);
    
    printf( "\r\n���ڷ�����Ϣ ......\r\n" );
    
    //��������
    ESP8266_MQTT_PUB(DHT11_Data.temp_int,DHT11_Data.humi_int,led_value);
  
}


