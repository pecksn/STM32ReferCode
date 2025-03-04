#include "ANO_send_data.h"
#include <main.h>
#include <usart.h>




void Send_data_to_ANO(uint8_t ID,int16_t data1,uint16_t data2,uint16_t data3,uint16_t data4)
{
	uint8_t Data_buffer[30];
	uint8_t cnt=0;
	uint8_t sumcheck=0,addcheck=0;
	Data_buffer[cnt++] = 0xAA;
	Data_buffer[cnt++] = 0xFF;
	Data_buffer[cnt++] = ID;
	Data_buffer[cnt++] = 0x08;
	Data_buffer[cnt++] = BYTE0(data1);
	Data_buffer[cnt++] = BYTE1(data1);
	Data_buffer[cnt++] = BYTE0(data2);
	Data_buffer[cnt++] = BYTE1(data2);
	Data_buffer[cnt++] = BYTE0(data3);
	Data_buffer[cnt++] = BYTE1(data3);
	Data_buffer[cnt++] = BYTE0(data4);
	Data_buffer[cnt++] = BYTE1(data4);
	
	for(uint8_t i=0;i<12;i++){
		sumcheck += Data_buffer[i];
		addcheck += sumcheck;
	}
	Data_buffer[cnt++] = sumcheck;
	Data_buffer[cnt++] = addcheck;
	HAL_UART_Transmit_DMA(&huart6, (uint8_t *)Data_buffer, sizeof(Data_buffer));
}














