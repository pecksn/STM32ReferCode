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
  * @brief  获取网络调试助手和串口调试助手发来的信息
  * @param  无
  * @retval 无
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
  * @brief  ESP8266 StaTcpClient Unvarnish 配置测试函数
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void)
{
    printf( "\r\n正在配置 ESP8266 ......\r\n" );
    printf( "\r\n使能 ESP8266 ......\r\n" );
    macESP8266_CH_ENABLE();
    while( ! ESP8266_AT_Test() );
    
//    ESP8266_Cmd ( "AT+CIPSERVER=0", "OK", NULL, 5000 );    
    printf( "\r\n禁止多连接 ......\r\n" );
    while( ! ESP8266_Enable_MultipleId ( DISABLE ) );
    
    printf( "\r\n正在配置工作模式 STA ......\r\n" );
    while( ! ESP8266_Net_Mode_Choose ( STA ) );
    while( ! ESP8266_DHCP_CUR () );
    
    
    printf( "\r\n正在连接 WiFi ......\r\n" );
    while( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
    
    //以下使用MQTT AT指令实现连接
    printf( "\r\n正在配置MQTT ......\r\n" );
    while( !ESP8266_MQTT_USERCFG ());
    
    printf( "\r\n正在连接MQTT ......\r\n" );
    while( !ESP8266_MQTT_CONN ());
    
    printf( "\r\n正在订阅MQTT ......\r\n" );
    while( !ESP8266_MQTT_SUB ());
    
    mqtt_flag = 1;//mqtt已连接
    printf( "\r\n配置 ESP8266 完毕\r\n" );
    printf("等待接收MQTT LED控制数据\r\n");
    
}


/**
  * @brief  ESP8266 发送DHT11数据测试函数
  * @param  无
  * @retval 无
  */
void ESP8266_SendDHT11DataTest(void)
{
    //读取温湿度
    DHT11_Read_TempAndHumidity(&DHT11_Data);
    printf("\r\n\r\n读取DHT11成功!\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\nLED灯状态为 %d\r\n", 
          DHT11_Data.humi_int, DHT11_Data.humi_deci, DHT11_Data.temp_int, DHT11_Data.temp_deci ,led_value);
    
    printf( "\r\n正在发布消息 ......\r\n" );
    
    //发布数据
    ESP8266_MQTT_PUB(DHT11_Data.temp_int,DHT11_Data.humi_int,led_value);
  
}


