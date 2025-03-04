/*
 * usart6Dma.h
 *
 *  Created on: Jan 7, 2021
 *      Author: ASUS
 */

#ifndef USART1DMA_USART6DMA_H_
#define USART1DMA_USART6DMA_H_
#include "main.h"


#define USART1_DMA_REC_SIZE 50
#define USART1_REC_SIZE 100

typedef struct
{
        uint8_t UsartRecFlag; // 标志位
        uint16_t UsartRecLen; // 接收数据长度
        uint16_t UsartDMARecLEN; // DMA 接收长度
        uint8_t  Usart1DMARecBuffer[USART1_DMA_REC_SIZE]; // DMA 接收数组
        uint8_t  Usart1RecBuffer[USART1_REC_SIZE]; // 接收组
}teUsart1type;


extern teUsart1type Usart1type;


void EnableUsart_It(void);


#endif /* USART6DMA_USART6DMA_H_ */










