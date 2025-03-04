#include "usart.h"
#include "main.h"
#include "usart1Dma.h"
#include "Receive_execute.h"
#include "Y_Step_motor.h"
#include<string.h>

//void clear_zero()
//{
//	while (Usart1type.UsartRecLen > 0 && Usart1type.Usart1RecBuffer[Usart1type.UsartRecLen-1] == 0xFF) 
//	{
//        // 如果数据包末尾是FF，就将其去掉
//        Usart1type.UsartRecLen--;
//    }

//}
uint32_t set_speed  = 500;         // 速度 单位为0.05rad/sec
// 加速度和减速度选取一般根据实际需要，值越大速度变化越快，加减速阶段比较抖动
// 所以加速度和减速度值一般是在实际应用中多尝试出来的结果
uint32_t step_accel = 150;         // 加速度 单位为0.1rad/sec^2
uint32_t step_decel = 50;          // 减速度 单位为0.1rad/sec^2

uint32_t fastseek_Speed = FASTSEEK_SPEED;
uint32_t slowseek_Speed = SLOWSEEK_SPEED;
void judge_execute()
{			
	if(Usart1type.UsartRecFlag == 1)
    {	  if(Usart1type.Usart1RecBuffer[0]==0x01&& Usart1type.Usart1RecBuffer[1]==0x01 && Usart1type.Usart1RecBuffer[3]==0x01)
		  {
			  switch(Usart1type.Usart1RecBuffer[2])
			  {
				  case 1: 
					  STEPMOTOR_DisMoveAbs(AXIS_X,100,step_accel,step_decel,set_speed);//X轴移动到400mm的位置
					  break;
				  case 2: 
					  STEPMOTOR_DisMoveAbs(AXIS_Y,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
					  break;
				  case 3: 
					  STEPMOTOR_DisMoveAbs(AXIS_Z,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
					  break;
				  case 4: 
					  STEPMOTOR_DisMoveAbs(AXIS_Z,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
					  break;
				  case 5: 
					  STEPMOTOR_DisMoveAbs(AXIS_Z,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
					  break;
				  case 6: 
					  STEPMOTOR_DisMoveAbs(AXIS_Z,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
					  break;
				  case 7: 
					  STEPMOTOR_DisMoveAbs(AXIS_Z,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
					  break;
				  case 8: 
				 	  STEPMOTOR_DisMoveAbs(AXIS_Z,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
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
