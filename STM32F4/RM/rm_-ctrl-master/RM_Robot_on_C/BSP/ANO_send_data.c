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
	//HAL_UART_Transmit_DMA(&huart6, (uint8_t *)Data_buffer, sizeof(Data_buffer));
}



void ANO_send_int16(int16_t data1,int16_t data2,int16_t data3,int16_t data4)
{
    uint8_t Data_buffer[18]={0};

    uint8_t cnt=0;
    uint8_t sumcheck=0,addcheck=0;
    Data_buffer[cnt++] = 0xAA;//头
    Data_buffer[cnt++] = 0xFF;//地址
    Data_buffer[cnt++] = 0xF1;//ID
    Data_buffer[cnt++] = 0x0C;//字节数--12
    Data_buffer[4] = (uint8_t)(data1>>8)&0xFF;
    Data_buffer[5] = (uint8_t)(data1<<8)&0xFF;
    Data_buffer[6] = BYTE0(data2);
    Data_buffer[7] = BYTE1(data2);
    Data_buffer[8] = BYTE0(data3);
    Data_buffer[9] = BYTE1(data3);
    Data_buffer[10] = BYTE0(data4);
    Data_buffer[11] = BYTE1(data4);//8个字节->4个int16型数据
    Data_buffer[12] = 0x00;
    Data_buffer[13] = 0x00;
    Data_buffer[14] = 0x00;
    Data_buffer[15] = 0x00;          //四个空uint8
    for(uint8_t i=0;i<16;i++){
        sumcheck += Data_buffer[i];
        addcheck += sumcheck;
    }
    Data_buffer[16] = sumcheck;
    Data_buffer[17] = addcheck;
    HAL_UART_Transmit(&huart6, (uint8_t *)Data_buffer, sizeof(Data_buffer),0xfff);
}










