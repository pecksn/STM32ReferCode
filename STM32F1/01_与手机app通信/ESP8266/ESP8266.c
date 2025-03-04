#include "ESP8266.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
 UART_HandleTypeDef *uart8266;
 uint8_t ESP8266Rxbuf[LENGTH];
 uint8_t CIPMODE=0;
 uint8_t TESTMODE=0;
	int replyFlag=0;
void ESP8266_Init(UART_HandleTypeDef *uart){
	uart8266=uart;
	HAL_Delay(2000);
	HAL_UART_Transmit(uart8266,"AT+RESTORE\r\n",12,0xff);
	HAL_Delay(5000);
	HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));
}
uint8_t checkAT(uint8_t *str,int len){
	int i=0;
	for(i=0;i<len;i++){
		if(*(str+i)=='\r'||*(str+i)=='\n'){
			if(i<len-2){
				return 0;
			}
			else if(*(str+i)=='\r'&&*(str+i+1)=='\n'){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t sendATTo8266(uint8_t *str){
	int len=strlen((char *)str);
	if(0==checkAT(str,len)){
		printf("AT error\n");
		return 0;
	}
	else{
		printf("AT right\n");
		if(HAL_OK==HAL_UART_Transmit(uart8266,str,len,0xff)){
			printf("%.*s send to 8266 ok!\n",len-2,str);
			return 1;
		}
		else{
			printf("%.*s send to 8266 error!\n",len-2,str);
			return 0;
		}
	}
}

void recATBy8266(uint16_t Size){
	HAL_UART_DMAStop(uart8266);
//		printf("wifibuf:%s\n",WifiRxbuf);
	int i=0;
	do{
		if(ESP8266Rxbuf[i]=='O'&&ESP8266Rxbuf[i+1]=='K'){
			memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));
			HAL_Delay(2000);
			printf("8266 say OK\r\n");
			replyFlag=1;
//			printf("OK replyFlag:%d\n",replyFlag);
			HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));
			return;
			}
		else if(ESP8266Rxbuf[i]=='E'&&ESP8266Rxbuf[i+1]=='R'&&ESP8266Rxbuf[i+2]=='R'&&ESP8266Rxbuf[i+3]=='O'&&ESP8266Rxbuf[i+4]=='R'){
			HAL_UART_Transmit(uart8266,"AT\r\n",4,0xff);
			HAL_Delay(4000);
			printf("\n8266 say ERROR\r\n");
			replyFlag=2;
//			printf("ERROR replyFlag:%d\n",replyFlag);
			memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));
			HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));
			return;
			}
		}while(++i<Size);
	memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));
	HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));
}
uint8_t sendStringTo8266(uint8_t *str)
{
	uint16_t Size = strlen((char *)str);
	if(CIPMODE==1){
		HAL_UART_Transmit(uart8266,str,Size,0xff);
		if(!strcmp((char *)str,"+++")){
			printf("\nclose CIPSEND\n");
			CIPMODE=0;
		}
		return 1;
	}
	else if(!strcmp((char *)str,"AT+TESTMODE\r\n")){
		if(TESTMODE==0){
			printf("open testmode\n");
			TESTMODE=1;
		}
		else{
			printf("close testmode\n");
			TESTMODE=0;
		}
		return 0;
	}
	if(HAL_OK==HAL_UART_Transmit(uart8266,str,Size,0xff)){
		if(!strcmp((char *)str,"AT+CIPSEND\r\n")){
			printf("open CIPSEND\n");
			uint32_t t=0;
			while(replyFlag==0){
				HAL_Delay(1000);
				t++;
				if(t>40){
				printf("open CIPSEND error\n");
				return 0;
				}
			}
			CIPMODE=1;
		}
		return 1;
	}
	else{
		return 0;
	}
	
}
uint8_t sendTo8266(uint8_t *str){
	if(sendStringTo8266(str)){
		uint32_t t=0;
		
//		uint32_t t=4000*3127;//臭狗屎
//		while(t--){
//			printf("t=%d flag=%d\n",t,replyFlag);
//		}
		while(replyFlag==0){
			HAL_Delay(1000);
			t++;
			if(t>40){
				printf("time out error\n");
				return 0;
			}
		}
		if(replyFlag==1){
			replyFlag=0;
			return 1;
		}
		else{
			printf("error\n");
			replyFlag=0;
			return 0;
		}

	}
	else{
		replyFlag=0;
		return 0;
	}
}
void recStringBy8266(uint16_t Size)
{
	if(CIPMODE!=0||TESTMODE!=0){
		printf("%s\n",ESP8266Rxbuf);
		memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));
		HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));
	}
	else{
		recATBy8266(Size);
	}
}
void linkWifi(char *ssid,char *pwd){
	printf("Start link wifi:\n");
	printf("Set CWMODE=1\n");
	if(sendTo8266("AT+CWMODE=1\r\n")){
		printf("Set CWMODE success!\n");
	}
	else{
		printf("Set CWMODE fail!\n");
	}
	printf("8266 RESTART\n");
	if(sendTo8266("AT+RST\r\n")){
		printf("8266 RESTART success!\n");
	}
	else{
		printf("8266 RESTART fail!\n");
	}
	printf("link wifi:%s %s\n",ssid,pwd);
	char str[50];
	sprintf(str,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);
	if(sendTo8266(str)){
		printf("link wifi success!\n");
	}
	else{
		printf("link wifi fail!\n");
	}
	
}

void linkTCP(char *addr,char *port){
	printf("link TCP: %s %s\n",addr,port);
	char str[50];
	sprintf(str,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",addr,port);
	if(sendTo8266(str)){
		printf("link tcp success!\n");
	}
	else{
		printf("link tcp fail!\n");
	}
}
void StartCipsend(char *addr,char *port){
	linkTCP(addr,port);
	printf("Set Cipmode=1\n");
	if(sendTo8266("AT+CIPMODE=1\r\n")){
		printf("Set Cipmode=1 success!\n");
	}
	else{
		printf("Set Cipmode=1 fail!\n");
	}
	printf("Start cipsend\n");
	if(sendTo8266("AT+CIPSEND\r\n")){
		printf("Start cipsend success!\n");
	}
	else{
		printf("Start cipsend fail!\n");
	}
}




