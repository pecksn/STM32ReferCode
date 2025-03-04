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

#define USART2_DMA_REC_SIZE 50
#define USART2_REC_SIZE 100

#define USART3_DMA_REC_SIZE 50
#define USART3_REC_SIZE 100


typedef struct
{
        uint8_t UsartRecFlag; // ��־λ
        uint16_t UsartRecLen; // �������ݳ���
        uint16_t UsartDMARecLEN; // DMA ���ճ���
        uint8_t  Usart1DMARecBuffer[USART1_DMA_REC_SIZE]; // DMA ��������
        uint8_t  Usart1RecBuffer[USART1_REC_SIZE]; // ������
}teUsart1type;

typedef struct
{
        uint8_t UsartRecFlag; // ��־λ
        uint16_t UsartRecLen; // �������ݳ���
        uint16_t UsartDMARecLEN; // DMA ���ճ���
        uint8_t  Usart2DMARecBuffer[USART2_DMA_REC_SIZE]; // DMA ��������
        uint8_t  Usart2RecBuffer[USART2_REC_SIZE]; // ������
}teUsart2type;

typedef struct
{
        uint8_t UsartRecFlag; // ��־λ
        uint16_t UsartRecLen; // �������ݳ���
        uint16_t UsartDMARecLEN; // DMA ���ճ���
        uint8_t  Usart3DMARecBuffer[USART1_DMA_REC_SIZE]; // DMA ��������
        uint8_t  Usart3RecBuffer[USART1_REC_SIZE]; // ������
}teUsart3type;

extern teUsart1type Usart1type;
extern teUsart2type Usart2type;
extern teUsart3type Usart3type;

void EnableUsart1_It(void);
void EnableUsart2_It(void);
void EnableUsart3_It(void);

#endif /* USART6DMA_USART6DMA_H_ */










