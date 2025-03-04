#ifndef __ANO_SEDD_DATA_H
#define __ANO_SEDD_DATA_H
#include <main.h>

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))     				//取出int型变量的低字节
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))      //取存储在此变量下一内存字节的内容，高字节
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

void Send_data_to_ANO(uint8_t ID,int16_t data1,uint16_t data2,uint16_t data3,uint16_t data4);




#endif








