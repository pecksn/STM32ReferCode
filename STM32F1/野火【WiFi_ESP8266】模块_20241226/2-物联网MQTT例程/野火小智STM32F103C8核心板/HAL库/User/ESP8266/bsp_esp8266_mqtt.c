#include "./ESP8266/bsp_esp8266_mqtt.h"
#include "./ESP8266/bsp_esp8266_test.h"

int led_value = 0;//led开关值
uint8_t mqtt_flag = 0;//mqtt连接标志

/*
 * 函数名：ESP8266_MQTT_USERCFG
 * 描述  ：WF-ESP8266模块进行MQTT的用户配置
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_MQTT_USERCFG ( void )
{
	char cStr [300];
	
	sprintf ( cStr, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"", MQTT_CLIENT_ID,MQTT_USER_NAME,MQTT_PASSWD );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * 函数名：ESP8266_MQTT_CONN
 * 描述  ：WF-ESP8266模块进行MQTT的连接配置
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_MQTT_CONN ( void )
{
	char cStr [200];
	
	sprintf ( cStr, "AT+MQTTCONN=0,\"%s\",1883,0", MQTT_BROKERADDRESS );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * 函数名：ESP8266_MQTT_SUB
 * 描述  ：WF-ESP8266模块进行MQTT的信息订阅
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_MQTT_SUB ( void )
{
	char cStr [200];
	
	sprintf ( cStr, "AT+MQTTSUB=0,\"%s\",0", MQTT_SUBSCRIBE_TOPIC );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * 函数名：ESP8266_MQTT_PUB
 * 描述  ：WF-ESP8266模块进行MQTT的信息推送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_MQTT_PUB ( uint8_t temp_set,uint8_t humi_set,uint8_t led_value )
{
	char cStr [200];

	sprintf ( cStr, "AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\,\\\"led\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}\",0,0",\
    MQTT_PUBLISH_TOPIC,temp_set,humi_set,led_value );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * 函数名：ESP8266_MQTT_RECV
 * 描述  ：ESP8266模块接收MQTT传来的数据，并且解析
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_MQTT_RECV( void )
{
	char pRecStr[20] = {0};
    char* found_str = 0;
	
    if ( ucTcpClosedFlag )                     //检测是否失去连接，需退出透传模式才能接收到
    {
        mqtt_flag = 0;//mqtt断开
        printf("服务器断开\r\n"); 
        while(1);  
    } 

    if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "value"))//按照esp8266实际发回来的LED控制数据进行判断
    {
        strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
        
        sprintf(pRecStr,"\"%s\":","value");//按照esp8266实际发回来的LED控制数据进行判断
        found_str = strstr(strEsp8266_Fram_Record .Data_RX_BUF, pRecStr);
        
        if (found_str != NULL) 
        {
            sscanf(found_str + strlen(pRecStr), "%d", &led_value);//获取LED的数据
            printf("\r\n接收到LED控制数据，等待消息发布\r\n");
            printf("LED：%d\n",led_value);
            if(led_value == 1)
            {
                LED2_ON;
            }                
            else  
            {
                LED2_OFF;
            }
            return true;
        } 
    }
    else 
    {
        return false;
    }
    return false;
}


