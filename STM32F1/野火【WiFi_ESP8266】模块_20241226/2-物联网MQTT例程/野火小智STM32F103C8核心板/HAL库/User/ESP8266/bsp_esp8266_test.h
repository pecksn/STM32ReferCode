#ifndef  __BSP_ESP8266_TEST_H
#define	 __BSP_ESP8266_TEST_H


#include "stm32f1xx.h"


/********************************** �û���Ҫ���õĲ���**********************************/
#define      macUser_ESP8266_ApSsid                       "wifiname"         //Ҫ���ӵ��ȵ������
#define      macUser_ESP8266_ApPwd                        "123456"           //Ҫ���ӵ��ȵ����Կ


#define MQTT_BROKERADDRESS "k1acxDP4SBI.iot-as-mqtt.cn-shanghai.aliyuncs.com"   //����������

#define MQTT_CLIENT_ID  "mqtt_stm32|securemode=2\\,signmethod=hmacsha1\\,timestamp=1732852282145|"  //�ͻ����
#define MQTT_USER_NAME  "mqtt_stm32&k1acxDP4SBI"                                //�û���
#define MQTT_PASSWD     "46100E6E696C21DF6336E4D060A9C1B0477FB5FE"              //�û�����
#define MQTT_SUBSCRIBE_TOPIC  "/sys/k1acxDP4SBI/mqtt_stm32/thing/service/property/set"    //��������
#define MQTT_PUBLISH_TOPIC    "/sys/k1acxDP4SBI/mqtt_stm32/thing/event/property/post"     //��Ϣ����

/********************************** �ⲿȫ�ֱ��� ***************************************/
extern volatile uint8_t ucTcpClosedFlag;


/********************************** ���Ժ������� ***************************************/
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void);
void ESP8266_SendDHT11DataTest(void);

#endif /* __BSP_ESP8266_TEST_H */

