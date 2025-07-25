/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-02-14
* @brief   			用3.5.0版本库建的工程模板
* @attention  	实验平台：ModuleStar开发板 + W5500网络适配板
*
*               默认使用开发板的SPI2接口
*							
*               内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突
*               如果是用网线跟PC机直连，请设置PC机本地连接地址IP为静态IP
*
* * 淘宝    :https://shop71600107.taobao.com/
******************************************************************************
*/ 
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_led.h"

#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"
/*app函数头文件*/
#include "tcp_demo.h"

int main(void)
{ 	
	systick_init(72);										/*初始化Systick工作时钟*/
	USART1_Config();										/*初始化串口通信:115200@8-n-1*/
	i2c_CfgGpio();											/*初始化eeprom*/

	printf("  网络初始化 Demo V1.0 \r\n");		

	gpio_for_w5500_config();						        /*初始化MCU相关引脚*/
	reset_w5500();											/*硬复位W5500*/
	set_w5500_mac();										/*配置MAC地址*/
	set_w5500_ip();											/*配置IP地址*/
	
	socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/
	
  printf(" 电脑作为TCP服务器,让W5500作为 TCP客户端去连接 \r\n");
	printf(" 服务器IP:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
	printf(" 监听端口:%d \r\n",remote_port);
	printf(" 连接成功后，服务器发送数据给W5500，W5500将返回对应数据 \r\n");
	printf(" 应用程序执行中……\r\n"); 
	
	while(1)                            /*循环执行的函数*/ 
	{
		do_tcp_client();                  /*TCP_Client 数据回环测试程序*/
	}
} 


