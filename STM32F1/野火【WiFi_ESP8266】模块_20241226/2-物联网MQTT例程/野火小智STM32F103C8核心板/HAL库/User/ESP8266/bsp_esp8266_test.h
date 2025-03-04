#ifndef  __BSP_ESP8266_TEST_H
#define	 __BSP_ESP8266_TEST_H


#include "stm32f1xx.h"


/********************************** 用户需要设置的参数**********************************/
#define      macUser_ESP8266_ApSsid                       "wifiname"         //要连接的热点的名称
#define      macUser_ESP8266_ApPwd                        "123456"           //要连接的热点的密钥


#define MQTT_BROKERADDRESS "k1acxDP4SBI.iot-as-mqtt.cn-shanghai.aliyuncs.com"   //服务器域名

#define MQTT_CLIENT_ID  "mqtt_stm32|securemode=2\\,signmethod=hmacsha1\\,timestamp=1732852282145|"  //客户编号
#define MQTT_USER_NAME  "mqtt_stm32&k1acxDP4SBI"                                //用户名
#define MQTT_PASSWD     "46100E6E696C21DF6336E4D060A9C1B0477FB5FE"              //用户密码
#define MQTT_SUBSCRIBE_TOPIC  "/sys/k1acxDP4SBI/mqtt_stm32/thing/service/property/set"    //订阅主题
#define MQTT_PUBLISH_TOPIC    "/sys/k1acxDP4SBI/mqtt_stm32/thing/event/property/post"     //消息发布

/********************************** 外部全局变量 ***************************************/
extern volatile uint8_t ucTcpClosedFlag;


/********************************** 测试函数声明 ***************************************/
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void);
void ESP8266_SendDHT11DataTest(void);

#endif /* __BSP_ESP8266_TEST_H */

