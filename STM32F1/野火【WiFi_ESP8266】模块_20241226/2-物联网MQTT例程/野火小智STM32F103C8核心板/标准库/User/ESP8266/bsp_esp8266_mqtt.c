#include "./ESP8266/bsp_esp8266_mqtt.h"
#include "./ESP8266/bsp_esp8266_test.h"

int led_value = 0;//led����ֵ
uint8_t mqtt_flag = 0;//mqtt���ӱ�־

/*
 * ��������ESP8266_MQTT_USERCFG
 * ����  ��WF-ESP8266ģ�����MQTT���û�����
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_MQTT_USERCFG ( void )
{
	char cStr [300];
	
	sprintf ( cStr, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"", MQTT_CLIENT_ID,MQTT_USER_NAME,MQTT_PASSWD );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * ��������ESP8266_MQTT_CONN
 * ����  ��WF-ESP8266ģ�����MQTT����������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_MQTT_CONN ( void )
{
	char cStr [200];
	
	sprintf ( cStr, "AT+MQTTCONN=0,\"%s\",1883,0", MQTT_BROKERADDRESS );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * ��������ESP8266_MQTT_SUB
 * ����  ��WF-ESP8266ģ�����MQTT����Ϣ����
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_MQTT_SUB ( void )
{
	char cStr [200];
	
	sprintf ( cStr, "AT+MQTTSUB=0,\"%s\",0", MQTT_SUBSCRIBE_TOPIC );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * ��������ESP8266_MQTT_PUB
 * ����  ��WF-ESP8266ģ�����MQTT����Ϣ����
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_MQTT_PUB ( uint8_t temp_set,uint8_t humi_set,uint8_t led_value )
{
	char cStr [200];

	sprintf ( cStr, "AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\,\\\"led\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}\",0,0",\
    MQTT_PUBLISH_TOPIC,temp_set,humi_set,led_value );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*
 * ��������ESP8266_MQTT_RECV
 * ����  ��ESP8266ģ�����MQTT���������ݣ����ҽ���
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_MQTT_RECV( void )
{
	char pRecStr[20] = {0};
    char* found_str = 0;
	
    if ( ucTcpClosedFlag )                     //����Ƿ�ʧȥ���ӣ����˳�͸��ģʽ���ܽ��յ�
    {
        mqtt_flag = 0;//mqtt�Ͽ�
        printf("�������Ͽ�\r\n"); 
        while(1);  
    } 

    if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "value"))//����esp8266ʵ�ʷ�������LED�������ݽ����ж�
    {
        strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
        
        sprintf(pRecStr,"\"%s\":","value");//����esp8266ʵ�ʷ�������LED�������ݽ����ж�
        found_str = strstr(strEsp8266_Fram_Record .Data_RX_BUF, pRecStr);
        
        if (found_str != NULL) 
        {
            sscanf(found_str + strlen(pRecStr), "%d", &led_value);//��ȡLED������
            printf("\r\n���յ�LED�������ݣ��ȴ���Ϣ����\r\n");
            printf("LED��%d\n",led_value);
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


