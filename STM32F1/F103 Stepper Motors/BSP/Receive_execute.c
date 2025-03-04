#include "usart.h"
#include "main.h"
#include "MotorControl.h"
#include "usart1Dma.h"
#include "Receive_execute.h"
#include<string.h>

//void clear_zero()
//{
//	while (Usart1type.UsartRecLen > 0 && Usart1type.Usart1RecBuffer[Usart1type.UsartRecLen-1] == 0xFF) 
//	{
//        // 如果数据包末尾是FF，就将其去掉
//        Usart1type.UsartRecLen--;
//    }

//}

void judge_execute()
{			
	if(Usart1type.UsartRecFlag == 1)
    {	  if(Usart1type.Usart1RecBuffer[0]==0x01&& Usart1type.Usart1RecBuffer[1]==0x01 && Usart1type.Usart1RecBuffer[3]==0x01)
		  {
			  switch(Usart1type.Usart1RecBuffer[2])
			  {
				  case 1: 
					  motor1_descent(); //要转多少圈在回调函数里面修改
					  break;
				  case 2: 
					  motor2_descent();
					  break;
				  case 3: 
					  motor3_descent();
					  break;
				  case 4: 
					  motor4_descent();
					  break;
				  case 5: 
					  motor5_descent();
					  break;
				  case 6: 
					  motor6_descent();
					  break;
				  case 7: 
					  motor7_descent();
					  break;
				  case 8: 
				 	  motor8_descent();
					  break;
				  default:
					  break;
			  }
			//    uint8_t a[4]={0x01,0x01,0x01,0x01};
         //	HAL_UART_Transmit(&huart1,a,4,1);
			  HAL_UART_Transmit(&huart1, Usart1type.Usart1RecBuffer,4 , 1);
		  }
		  
      //    printf("%s",Usart1type.Usart1RecBuffer);
		  memset(Usart1type.Usart1RecBuffer,0,100);//清除串口数组

          Usart1type.UsartRecLen = 0;
          Usart1type.UsartRecFlag =0;   
     }
	 HAL_Delay(10);
}

//工作状态反馈
void door_closed()
{
	if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1)==RESET)
	{
		uint8_t a[6]={0x01,0x02,0x01,0x01,0xFF};
		HAL_UART_Transmit_IT(&huart1,a,100);
	}
	
}
