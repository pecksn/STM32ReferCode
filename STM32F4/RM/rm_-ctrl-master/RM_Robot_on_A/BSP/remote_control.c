#include "remote_control.h"
#include <main.h>
#include "usart.h"

RC_Type remote_control;
uint8_t UART_Buffer[100];


void USART1_IRQHandler(void)
{

	
	HAL_UART_IDLE_IRQHandler(&huart1);

}


/**e
  * @brief  ���ô���û��DMA�жϵĽ���
	*					����ʹ���˿����жϣ���������û�б�Ҫ�ٿ���DMA�жϣ�����HAL����������UART_Receive_DMA����Ĭ�Ͽ�����DMA�жϣ�
	*					����Ҫ�Լ�ʵ��һ��û���жϵ�DMA���պ�����
  *         
  * @param  
  * @retval HAL status
  */
HAL_StatusTypeDef UART_Receive_DMA_NoIT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
 uint32_t *tmp;
  
  /* Check that a Rx process is not already ongoing 	���Rx�����Ƿ��Ѿ��ڽ�����*/
  if(huart->RxState == HAL_UART_STATE_READY) 
  {
    if((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    /* Set the DMA abort callback 	����DMA��ֹ�ص�*/
    huart->hdmarx->XferAbortCallback = NULL;

    /* Enable the DMA channel */
    tmp = (uint32_t*)&pData;											//ʹ��DMAͨ��
    HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, *(uint32_t*)tmp, Size);

    /* Clear the Overrun flag just before enabling the DMA Rx request: can be mandatory for the second transfer */
		//������DMA Rx����֮ǰ��������־:���ڵڶ��δ��������ǿ�Ƶ�
    __HAL_UART_CLEAR_OREFLAG(huart);

    /* Process Unlocked		���̽��� */
    __HAL_UNLOCK(huart);

    /* Enable the UART Parity Error Interrupt		����UART��żУ������ж� */
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);//����UART�����ж�(֡�����������󣬳��޴���)

    /* ͨ����UART CR3�Ĵ���������DMARλ�����ý��������DMA���� */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


HAL_StatusTypeDef HAL_UART_Receive_IT_IDLE(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){

	
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//�������ڿ����ж�
	
		return UART_Receive_DMA_NoIT(huart,pData,Size);//����û��DMA�жϵ�DMA���պ���
}


void HAL_UART_IDLE_IRQHandler(UART_HandleTypeDef *huart)
{
		
	uint32_t DMA_FLAGS,tmp;
	
			
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)){
			
		tmp = huart->Instance->SR;
		tmp = huart->Instance->DR;
		tmp++;  
		DMA_FLAGS = __HAL_DMA_GET_TC_FLAG_INDEX(huart->hdmarx);
		
	  __HAL_DMA_DISABLE(huart->hdmarx);
		__HAL_DMA_CLEAR_FLAG(huart->hdmarx,DMA_FLAGS);
		
		huart->hdmarx->Instance->NDTR = huart->RxXferSize;
		__HAL_DMA_ENABLE(huart->hdmarx);
		
	}
		
	
	Callback_RC_Handle(&remote_control,huart->pRxBuffPtr);
}


void Callback_RC_Handle(RC_Type* rc, uint8_t* buff)
{

	rc->ch1 = (buff[0] | buff[1]<<8) & 0x07FF;
	rc->ch1 -= 1024;
	rc->ch2 = (buff[1]>>3 | buff[2]<<5 ) & 0x07FF;
	rc->ch2 -= 1024;
	rc->ch3 = (buff[2]>>6 | buff[3]<<2 | buff[4]<<10) & 0x07FF;
	rc->ch3 -= 1024;
	rc->ch4 = (buff[4]>>1 | buff[5]<<7) & 0x07FF;		
	rc->ch4 -= 1024;
	rc->sw = (buff[16] | (buff[17] << 8)) & 0x07FF;
	rc->sw -= 1024;
	
	rc->switch_left = ( (buff[5] >> 4)& 0x000C ) >> 2;
	rc->switch_right =  (buff[5] >> 4)& 0x0003 ;
	
	rc->mouse.x = buff[6] | (buff[7] << 8);	
	rc->mouse.y = buff[8] | (buff[9] << 8);
	rc->mouse.z = buff[10]| (buff[11] << 8);
	
	rc->mouse.press_left 	= buff[12];	
	rc->mouse.press_right = buff[13];
	
	rc->keyBoard.key_code = buff[14] | buff[15] << 8; 
	
}















