#include "usart.h"
#include "main.h"
#include "MotorControl.h"
#include "usart1Dma.h"
#include "Receive_execute.h"
#include "Y_Step_motor.h"
#include <string.h>

// 速度最大值由驱动器和电机决定，有些最大是1800，有些可以达到4000
uint32_t set_speed  = 200;         // 速度 单位为0.05rad/sec
// 加速度和减速度选取一般根据实际需要，值越大速度变化越快，加减速阶段比较抖动
// 所以加速度和减速度值一般是在实际应用中多尝试出来的结果
uint32_t step_accel = 15;         // 加速度 单位为0.1rad/sec^2
uint32_t step_decel = 15;          // 减速度 单位为0.1rad/sec^2

uint32_t fastseek_Speed = FASTSEEK_SPEED;
uint32_t slowseek_Speed = SLOWSEEK_SPEED;

void judge_execute()
{			
	if(Usart3type.UsartRecFlag == 1)
    {	if(Usart3type.Usart3RecBuffer[0]==0x01&& Usart3type.Usart3RecBuffer[3]==0x01)
		{
			if(Usart3type.Usart3RecBuffer[1]==0x00&&Usart3type.Usart3RecBuffer[2]==0x01)//握手初始化，将电机全部升起来
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
				HAL_UART_Transmit(&huart3, Usart3type.Usart3RecBuffer,4 , 2);
				
				STEPMOTOR_AxisMoveRel(AXIS_1,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
					step_accel,step_decel,set_speed); 
				HAL_Delay(200);

				STEPMOTOR_AxisMoveRel(AXIS_2,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
					    step_accel,step_decel,set_speed);
				HAL_Delay(200);

			    STEPMOTOR_AxisMoveRel(AXIS_3,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
				step_accel,step_decel,set_speed);
				HAL_Delay(200);
						
				STEPMOTOR_AxisMoveRel(AXIS_4,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
						step_accel,step_decel,set_speed);
				HAL_Delay(200);

				STEPMOTOR_AxisMoveRel(AXIS_5,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
						step_accel,step_decel,set_speed);
				HAL_Delay(200);

//				STEPMOTOR_AxisMoveRel(AXIS_6,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
//					  	step_accel,step_decel,set_speed); 
//				HAL_Delay(200);

//				STEPMOTOR_AxisMoveRel(AXIS_7,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
//						step_accel,step_decel,set_speed);
//				HAL_Delay(200);

//				STEPMOTOR_AxisMoveRel(AXIS_8,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
//						step_accel,step_decel,set_speed);
//				HAL_Delay(200);	

			}
			else if (Usart3type.Usart3RecBuffer[1]==0x01)//下降指令
			{
				switch(Usart3type.Usart3RecBuffer[2])
			    {
				 	case 0x01: 
					   STEPMOTOR_AxisMoveRel(AXIS_1,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
					   step_accel,step_decel,set_speed); 
					  break;
				  case 0x02: 
					  STEPMOTOR_AxisMoveRel(AXIS_2,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
					  step_accel,step_decel,set_speed);
					  break;
				  case 0x03: 
					  STEPMOTOR_AxisMoveRel(AXIS_3,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
					  step_accel,step_decel,set_speed);
					  break;
				  case 0x04: 
					  STEPMOTOR_AxisMoveRel(AXIS_4,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
				      step_accel,step_decel,set_speed);
					  break;
				  case 0x05: 
					  STEPMOTOR_AxisMoveRel(AXIS_5,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
					  step_accel,step_decel,set_speed);
					  break;
//				  case 0x06: 
//					  STEPMOTOR_AxisMoveRel(AXIS_6,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
//					  step_accel,step_decel,set_speed); 
//					  break;
//				  case 0x07: 
//					  STEPMOTOR_AxisMoveRel(AXIS_7,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
//				      step_accel,step_decel,set_speed);
//					  break;
//				  case 0x08: 
//				 	  STEPMOTOR_AxisMoveRel(AXIS_8,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CW,\
//				      step_accel,step_decel,set_speed);
//					  break;
				  default:
					  break;
				}
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
				HAL_UART_Transmit(&huart3, Usart3type.Usart3RecBuffer,4 , 2);
			}
			else if(Usart3type.Usart3RecBuffer[1]==0x03)//上升指令
			{
				switch(Usart3type.Usart3RecBuffer[2])
			    {
					case 0x01: 
					   STEPMOTOR_AxisMoveRel(AXIS_1,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
					   step_accel,step_decel,set_speed); 
					  break;
				  case 0x02: 
					  STEPMOTOR_AxisMoveRel(AXIS_2,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
					  step_accel,step_decel,set_speed);
					  break;
				  case 0x03: 
					  STEPMOTOR_AxisMoveRel(AXIS_3,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
					  step_accel,step_decel,set_speed);
					  break;
				  case 0x04: 
					  STEPMOTOR_AxisMoveRel(AXIS_4,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
				      step_accel,step_decel,set_speed);
					  break;
				  case 0x05: 
					  STEPMOTOR_AxisMoveRel(AXIS_5,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
					  step_accel,step_decel,set_speed);
					  break;
//				  case 0x06: 
//					  STEPMOTOR_AxisMoveRel(AXIS_6,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
//					  step_accel,step_decel,set_speed); 
//					  break;
//				  case 0x07: 
//					  STEPMOTOR_AxisMoveRel(AXIS_7,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
//				      step_accel,step_decel,set_speed);
//					  break;
//				  case 0x08: 
//				 	  STEPMOTOR_AxisMoveRel(AXIS_8,RISE_DECLINE_CYCLE*SPR*MOTOR_DIR_CCW,\
//				      step_accel,step_decel,set_speed);
//					  break;
				  default:
					  break;
				}
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
				HAL_UART_Transmit(&huart3, Usart3type.Usart3RecBuffer,4 , 2);
			}
		    else
		  	{
			  
		 	}

		}
       // printf("%s",Usart2type.Usart2RecBuffer);
		  memset(Usart3type.Usart3RecBuffer,0,100);//清除串口数组
          Usart3type.UsartRecLen = 0;
          Usart3type.UsartRecFlag =0;   
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);	  
    }
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
