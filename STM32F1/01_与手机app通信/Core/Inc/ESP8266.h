#ifndef _ESP_8266_H
#define _ESP_8266_H
#include "main.h"
#include <stdio.h>
#define LENGTH 1024
extern uint8_t ESP8266Rxbuf[LENGTH];//存放来自8266的回复

void ESP8266_Init(UART_HandleTypeDef *uart);//初始化函数
uint8_t sendATTo8266(uint8_t *str);//向8266发送AT指令
uint8_t sendStringTo8266(uint8_t *str);//向8266发送字符串
void recStringBy8266(uint16_t Size);//接收来自8266的字符串
void linkWifi(char *ssid,char *pwd);//连接wifi，ssid wifi：名字，pwd：wifi密码
void linkTCP(char *addr,char *port);//建立TCP连接，addr：IP地址 port：端口
void StartCipsend(char *addr,char *port);//开始透明传输
#endif
