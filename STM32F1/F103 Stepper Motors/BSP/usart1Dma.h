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
        uint8_t UsartRecFlag; // ��־λ
        uint16_t UsartRecLen; // �������ݳ���
        uint16_t UsartDMARecLEN; // DMA ���ճ���
        uint8_t  Usart1DMARecBuffer[USART1_DMA_REC_SIZE]; // DMA ��������
        uint8_t  Usart1RecBuffer[USART1_REC_SIZE]; // ������
}teUsart1type;


extern teUsart1type Usart1type;


void EnableUsart_It(void);


#endif /* USART6DMA_USART6DMA_H_ */










