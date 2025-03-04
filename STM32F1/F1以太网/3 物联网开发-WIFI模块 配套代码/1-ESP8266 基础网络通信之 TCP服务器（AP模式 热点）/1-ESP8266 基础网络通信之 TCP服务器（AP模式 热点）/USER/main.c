/*-----------------------------------------------------*/
/*              ModuleStar STM32系列开发板                */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*CORE     ：包含STM32的核心程序，官方提供，我们不修改 */
/*STLIB    ：官方提供的库文件，我们不修改              */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"       //包含需要的头文件
#include "delay.h"      //包含需要的头文件
#include "usart1.h"     //包含需要的头文件
#include "usart2.h"     //包含需要的头文件
#include "wifi.h"	    //包含需要的头文件

char  Data_buff[2048];  //数据缓冲区
char  Data_len[5];      //数据长度缓冲区
char  Data_id[5];       //数据发送者的id缓冲区

int main(void) 
{	
	Delay_Init();                   //延时功能初始化              
	Usart1_Init(115200);              //串口1功能初始化，波特率9600
	Usart2_Init(115200);            //串口2功能初始化，波特率115200	
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
    while(WiFi_InitServer_AP()){    //循环，初始化，建立服务器，直到成功
		Delay_Ms(200);              //延时
	}      
	WiFi_RxCounter=0;                           //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //清空WiFi接收缓冲区 
	
	while(1)                                    //主循环
	{		
		WiFi_Get_LinkSta();                               //检测有无客户端连接或是断开
		if(WiFi_Get_Data(Data_buff,Data_len,Data_id)){    //接收数据		
			u1_printf("连接id:%s有%s字节数据到来\r\n",Data_id,Data_len);   //串口显示信息
			u1_printf("数据:%s\r\n",Data_buff);                            //串口显示信息
			WiFi_SendData(Char_to_Hex(Data_id,strlen(Data_id)),Data_buff,strlen(Data_buff),500);//5s的发送超时时间，发送数据
		}
	}
}
