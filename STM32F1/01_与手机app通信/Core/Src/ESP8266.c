#include "ESP8266.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
 UART_HandleTypeDef *uart8266;
 uint8_t ESP8266Rxbuf[LENGTH];//存放8266的消息
 uint8_t CIPMODE=0;//透明传输模式标志位
 uint8_t TESTMODE=0;//测试模式标志位
	int replyFlag=0;//8266回复标志位
/*8266初始化*/
void ESP8266_Init(UART_HandleTypeDef *uart){
	uart8266=uart;
	HAL_Delay(2000);//8266上电后要等待一段时间才能正常运行
	HAL_UART_Transmit(uart8266,"AT+RESTORE\r\n",12,0xff);//发送恢复出厂化设置的指令
	HAL_Delay(5000);//等待恢复出厂设置完成
	HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));//开启接收8266的消息
}
/*检查AT指令是否合规*/
/*0 不合规 1 合规*/
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
/*发送AT指令*/
/*0 发送失败，1 发送成功*/
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
/*接收来自8266的AT消息回复*/
void recATBy8266(uint16_t Size){
	HAL_UART_DMAStop(uart8266);//先暂停接收
//		printf("wifibuf:%s\n",WifiRxbuf);
	int i=0;
	do{  //判断消息里有没有OK或者ERROR，8266回复的消息里都会带有OK或者ERROR
		if(ESP8266Rxbuf[i]=='O'&&ESP8266Rxbuf[i+1]=='K'){
			memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));
			HAL_Delay(2000);//收到OK，延时，等后续的消息发送完成
			printf("8266 say OK\r\n");
			replyFlag=1;//收到回复的标志位置1
//			printf("OK replyFlag:%d\n",replyFlag);
			HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));//开启下次接收
			return;
			}
		else if(ESP8266Rxbuf[i]=='E'&&ESP8266Rxbuf[i+1]=='R'&&ESP8266Rxbuf[i+2]=='R'&&ESP8266Rxbuf[i+3]=='O'&&ESP8266Rxbuf[i+4]=='R'){
			HAL_UART_Transmit(uart8266,"AT\r\n",4,0xff);//如果返回是error，那么下一条还是发送失败，所以用个AT指令来抵掉这次发送失败
			HAL_Delay(4000);//延时等抵消成功
			printf("\n8266 say ERROR\r\n");
			replyFlag=2;//收到回复的标志位置2
//			printf("ERROR replyFlag:%d\n",replyFlag);
			memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));
			HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));
			return;
			}
		}while(++i<Size);
	/*8266回复的消息有些是分包发回的，即OK或者ERROR可能在后续的包里面*/
	memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));//清空接收下一数据包
	HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));//开始接收下一数据包
}
/*向8266发送字符串*/
/*0：发送失败，1：发送成功*/
uint8_t sendStringTo8266(uint8_t *str)
{
	uint16_t Size = strlen((char *)str);
	if(CIPMODE==1){//判断是否为透明传输模式，是的话直接进行发送
		HAL_UART_Transmit(uart8266,str,Size,0xff);
		if(!strcmp((char *)str,"+++")){//如果发送的是“+++”(不带\r\n就是关闭透明传输模式)
			printf("\nclose CIPSEND\n");
			CIPMODE=0;
		}
		return 1;
	}
	/*这个是我编写的调试模式，不用发给8266*/
	/*调试模式下，就相当于PC直接与8266通信*/
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
	if(HAL_OK==HAL_UART_Transmit(uart8266,str,Size,0xff)){//发送成功
		if(!strcmp((char *)str,"AT+CIPSEND\r\n")){//判断是否是开启透明传输模式的指令
			printf("open CIPSEND\n");
			uint32_t t=0;
			while(replyFlag==0){//等待8266回复
				HAL_Delay(1000);
				t++;
				if(t>40){
				printf("open CIPSEND error\n");
				return 0;
				}
			}
			CIPMODE=1;//收到回复就把标志位置1
		}
		return 1;
	}
	else{
		return 0;
	}
	
}
/*向8266发送消息并且确认回复*/
/*0：发送失败或者没收到回复或者回复ERROR*/
/*1：发送成功且收到OK回复*/
uint8_t sendTo8266(uint8_t *str){
	if(sendStringTo8266(str)){
		uint32_t t=0;
		
//		uint32_t t=4000*3127;//臭狗屎，慎用，用于调试，看看回复标志位什么时候置1。
//		while(t--){
//			printf("t=%d flag=%d\n",t,replyFlag);
//		}
		while(replyFlag==0){//等待回复
			HAL_Delay(1000);
			t++;
			if(t>40){//超时
				printf("time out error\n");
				return 0;
			}
		}
		if(replyFlag==1){//判断是不是OK回复。
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
/*处理来自8266的消息*/
void recStringBy8266(uint16_t Size)
{
	if(CIPMODE!=0||TESTMODE!=0){//如果是透明传输模式或者调试模式，直接显示
		printf("%s\n",ESP8266Rxbuf);
		memset(ESP8266Rxbuf,0,sizeof(ESP8266Rxbuf));
		HAL_UARTEx_ReceiveToIdle_DMA(uart8266,ESP8266Rxbuf,sizeof(ESP8266Rxbuf));
	}
	else{//否则就是处理AT消息回复
		recATBy8266(Size);
	}
}
/*连接wifi*/
/*过程
1.设置为STA模式：AT+CWMODE=1\r\n
2.重启8266模块执行设置：AT+RST\r\n
3.连接wifi：AT+CWJAP="wifi名","密码"\r\n
*/
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
	char str[50];//一定要是char类型的数组，否则sprintf会报错，不知道怎么解决
	sprintf(str,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);//在stdio.h库中
	if(sendTo8266(str)){
		printf("link wifi success!\n");
	}
	else{
		printf("link wifi fail!\n");
	}
	
}
/*建立TCP连接*/
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
/*开始透明传输*/
/*过程
1.连接TCP
2.设置为透明传输模式：AT+CIPMODE=1\r\n
3.开启透明传输：AT+CIPSEND\r\n
*/
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




