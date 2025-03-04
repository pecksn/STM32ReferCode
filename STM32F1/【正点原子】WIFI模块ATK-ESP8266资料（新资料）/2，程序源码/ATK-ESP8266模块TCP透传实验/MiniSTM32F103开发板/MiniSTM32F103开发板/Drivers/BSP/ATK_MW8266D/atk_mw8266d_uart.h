/**
 ****************************************************************************************************
 * @file        atk_mw8266d_uart.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ��UART�ӿ���������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� MiniSTM32 V4������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MW8266D_UART_H
#define __ATK_MW8266D_UART_H

#include "./SYSTEM/sys/sys.h"

/* ���Ŷ��� */
#define ATK_MW8266D_UART_TX_GPIO_PORT           GPIOC
#define ATK_MW8266D_UART_TX_GPIO_PIN            GPIO_PIN_12
#define ATK_MW8266D_UART_TX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC��ʱ��ʹ�� */

#define ATK_MW8266D_UART_RX_GPIO_PORT           GPIOD
#define ATK_MW8266D_UART_RX_GPIO_PIN            GPIO_PIN_2
#define ATK_MW8266D_UART_RX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0) /* PD��ʱ��ʹ�� */

#define ATK_MW8266D_UART_INTERFACE              UART5
#define ATK_MW8266D_UART_IRQn                   UART5_IRQn
#define ATK_MW8266D_UART_IRQHandler             UART5_IRQHandler
#define ATK_MW8266D_UART_CLK_ENABLE()           do{ __HAL_RCC_UART5_CLK_ENABLE(); }while(0) /* UART5ʱ��ʹ�� */

/* UART�շ������С */
#define ATK_MW8266D_UART_RX_BUF_SIZE            128
#define ATK_MW8266D_UART_TX_BUF_SIZE            64

/* �������� */
void atk_mw8266d_uart_printf(char *fmt, ...);       /* ATK-MW8266D UART printf */
void atk_mw8266d_uart_rx_restart(void);             /* ATK-MW8266D UART���¿�ʼ�������� */
uint8_t *atk_mw8266d_uart_rx_get_frame(void);       /* ��ȡATK-MW8266D UART���յ���һ֡���� */
uint16_t atk_mw8266d_uart_rx_get_frame_len(void);   /* ��ȡATK-MW8266D UART���յ���һ֡���ݵĳ��� */
void atk_mw8266d_uart_init(uint32_t baudrate);      /* ATK-MW8266D UART��ʼ�� */

#endif
