#ifndef _ESP_8266_H
#define _ESP_8266_H
#include "main.h"
#include <stdio.h>
#define LENGTH 1024
extern uint8_t WifiRxbuf[LENGTH];

void ESP8266_Init(UART_HandleTypeDef *uart);
uint8_t sendATTo8266(uint8_t *str);
uint8_t sendStringTo8266(uint8_t *str);
void recStringBy8266(uint16_t Size);
void linkWifi(char *ssid,char *pwd);
void linkTCP(char *addr,char *port);
void StartCipsend(char *addr,char *port);
#endif
