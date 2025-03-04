/*
 * usart1Dma.c
 
 */

#include "usart1Dma.h"

#include "usart.h"
#include "main.h"

teUsart1type Usart1type;


// 打开相关中断
void EnableUsart_It(void)
{
        __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	//	__HAL_UART_ENABLE_IT(&huart1,UART_IT_TXE);
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        HAL_UART_Receive_DMA(&huart1,Usart1type.Usart1DMARecBuffer,USART1_DMA_REC_SIZE);
}


