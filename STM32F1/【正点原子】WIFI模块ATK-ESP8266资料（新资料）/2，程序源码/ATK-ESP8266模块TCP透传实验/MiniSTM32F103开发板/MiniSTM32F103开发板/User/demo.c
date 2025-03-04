/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ��TCP͸��ʵ��
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

#include "demo.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"

#define DEMO_WIFI_SSID          "ALIENTEK-YF"
#define DEMO_WIFI_PWD           "15902020353"
#define DEMO_TCP_SERVER_IP      "192.168.1.3"
#define DEMO_TCP_SERVER_PORT    "8080"

/**
 * @brief       ��ʾIP��ַ
 * @param       ��
 * @retval      ��
 */
static void demo_show_ip(char *buf)
{
    printf("IP: %s\r\n", buf);
    lcd_show_string(60, 151, 128, 16, 16, buf, BLUE);
}

/**
 * @brief       ����0���ܣ����ܲ���
 * @param       is_unvarnished: 0��δ����͸��
 *                              1���ѽ���͸��
 * @retval      ��
 */
static void demo_key0_fun(uint8_t is_unvarnished)
{
    uint8_t ret;
    
    if (is_unvarnished == 0)
    {
        /* ����ATָ����� */
        ret = atk_mw8266d_at_test();
        if (ret == 0)
        {
            printf("AT test success!\r\n");
        }
        else
        {
            printf("AT test failed!\r\n");
        }
    }
    else
    {
        /* ͨ��͸����������Ϣ��TCP Server */
        atk_mw8266d_uart_printf("This ATK-MW8266D TCP Connect Test.\r\n");
    }
}

/**
 * @brief       ����1���ܣ��л�͸��ģʽ
 * @param       is_unvarnished: 0��δ����͸��
 *                              1���ѽ���͸��
 * @retval      ��
 */
static void demo_key1_fun(uint8_t *is_unvarnished)
{
    uint8_t ret;
    
    if (*is_unvarnished == 0)
    {
        /* ����͸�� */
        ret = atk_mw8266d_enter_unvarnished();
        if (ret == 0)
        {
            *is_unvarnished = 1;
            printf("Enter unvarnished!\r\n");
        }
        else
        {
            printf("Enter unvarnished failed!\r\n");
        }
    }
    else
    {
        /* �˳�͸�� */
        atk_mw8266d_exit_unvarnished();
        *is_unvarnished = 0;
        printf("Exit unvarnished!\r\n");
    }
}

/**
 * @brief       ����͸��ʱ����������TCP Server�����ݷ��͵����ڵ�������
 * @param       is_unvarnished: 0��δ����͸��
 *                              1���ѽ���͸��
 * @retval      ��
 */
static void demo_upload_data(uint8_t is_unvarnished)
{
    uint8_t *buf;
    
    if (is_unvarnished == 1)
    {
        /* ��������ATK-MW8266D UART��һ֡���� */
        buf = atk_mw8266d_uart_rx_get_frame();
        if (buf != NULL)
        {
            printf("%s", buf);
            /* �ؿ���ʼ��������ATK-MW8266D UART������ */
            atk_mw8266d_uart_rx_restart();
        }
    }
}

/**
 * @brief       ������ʾ��ں���
 * @param       ��
 * @retval      ��
 */
void demo_run(void)
{
    uint8_t ret;
    char ip_buf[16];
    uint8_t key;
    uint8_t is_unvarnished = 0;
    
    /* ��ʼ��ATK-MW8266D */
    ret = atk_mw8266d_init(115200);
    if (ret != 0)
    {
        printf("ATK-MW8266D init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
    printf("Joining to AP...\r\n");
    ret  = atk_mw8266d_restore();                               /* �ָ��������� */
    ret += atk_mw8266d_at_test();                               /* AT���� */
    ret += atk_mw8266d_set_mode(1);                             /* Stationģʽ */
    ret += atk_mw8266d_sw_reset();                              /* �����λ */
    ret += atk_mw8266d_ate_config(0);                           /* �رջ��Թ��� */
    ret += atk_mw8266d_join_ap(DEMO_WIFI_SSID, DEMO_WIFI_PWD);  /* ����WIFI */
    ret += atk_mw8266d_get_ip(ip_buf);                          /* ��ȡIP��ַ */
    if (ret != 0)
    {
        printf("Error to join ap!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    demo_show_ip(ip_buf);
    
    /* ����TCP������ */
    ret = atk_mw8266d_connect_tcp_server(DEMO_TCP_SERVER_IP, DEMO_TCP_SERVER_PORT);
    if (ret != 0)
    {
        printf("Error to connect tcp server!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
    /* ���¿�ʼ�����µ�һ֡���� */
    atk_mw8266d_uart_rx_restart();
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                /* ���ܲ��� */
                demo_key0_fun(is_unvarnished);
                break;
            }
            case KEY1_PRES:
            {
                /* ͸��ģʽ�л� */
                demo_key1_fun(&is_unvarnished);
                break;
            }
            default:
            {
                break;
            }
        }
        
        /* ����͸��������TCP Server�����ݵ����ڵ������� */
        demo_upload_data(is_unvarnished);
        
        delay_ms(10);
    }
}
