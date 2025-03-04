/*
 * usart1Dma.c
 
 */

#include "usart1Dma.h"

#include "usart.h"
#include "main.h"

teUsart1type Usart1type;
teUsart2type Usart2type;
teUsart3type Usart3type;

// 打开相关中断
void EnableUsart1_It(void)
{
        __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	//	__HAL_UART_ENABLE_IT(&huart1,UART_IT_TXE);
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        HAL_UART_Receive_DMA(&huart1,Usart1type.Usart1DMARecBuffer,USART1_DMA_REC_SIZE);
}

void EnableUsart2_It(void)
{
        __HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	//	__HAL_UART_ENABLE_IT(&huart2,UART_IT_TXE);
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        HAL_UART_Receive_DMA(&huart2,Usart2type.Usart2DMARecBuffer,USART2_DMA_REC_SIZE);
}

void EnableUsart3_It(void)
{	
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
        __HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
	//	__HAL_UART_ENABLE_IT(&huart3,UART_IT_TXE);
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);
        HAL_UART_Receive_DMA(&huart3,Usart3type.Usart3DMARecBuffer,USART3_DMA_REC_SIZE);
}
