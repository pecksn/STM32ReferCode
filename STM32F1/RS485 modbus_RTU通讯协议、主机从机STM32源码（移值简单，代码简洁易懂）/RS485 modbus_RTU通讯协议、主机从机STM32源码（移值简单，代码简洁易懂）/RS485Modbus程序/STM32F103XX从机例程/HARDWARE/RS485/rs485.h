#ifndef __RS485_H
#define __RS485_H			

#include "sys.h"	 								  


/* 串口相关设置，如需更换串口更改下面这几项即可 */
#define	RS485_PROT		GPIOA					//串口端口
#define RS485_GPIO_RCC	RCC_APB2Periph_GPIOA	//串口端口GPIO时钟
#define RS485_TX_Pin	GPIO_Pin_2				//串口TXD引脚
#define RS485_RX_Pin	GPIO_Pin_3				//串口RXD引脚
#define RS485_USART_RCC	RCC_APB1Periph_USART2	//串口功能时钟
#define RS485_USART_NUM	USART2					//串口通道

/* 485模式控制引脚设置 如需更换更改下面这几项即可 */
#define	RS485_MODE_PROT	GPIOA					//RS485模式控制端口
#define RS485_MODE_Pin	GPIO_Pin_1				//RS485模式控制引脚
#define RS485_MODE_RCC	RCC_APB2Periph_GPIOA	//RS485模式控制端口GPIO时钟
#define RS485_TX_EN		PAout(1)				//485模式控制.0,接收;1,发送

/* 如果想串口中断接收，请不要注释以下宏定义 */
#define EN_USART2_RX 	1						//0,不接收;1,接收.


extern u8 RS485Busy ;
extern u8 regGroup[5];

void RS485_Init(u32 bound);
void UartDriver(void);
void UartRxMonitor(u8 ms);


#endif	   
















