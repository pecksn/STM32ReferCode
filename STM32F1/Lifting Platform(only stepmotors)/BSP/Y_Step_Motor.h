#ifndef __Y_STEP_MOTOR_H
#define	__Y_STEP_MOTOR_H


/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"
/* 类型定义 ------------------------------------------------------------------*/
typedef struct {
  __IO uint8_t  run_state ;  	// 电机旋转状态
  __IO int8_t   dir ;        	// 电机旋转方向
  __IO uint32_t decel_start; 	// 启动减速位置
  __IO int32_t  step_delay;  	// 下个脉冲周期（时间间隔），启动时为加速度
  __IO int32_t  decel_val;   	// 减速阶段步数
  __IO int32_t  min_delay;   	// 最小脉冲周期(最大速度，即匀速段速度)
  __IO int32_t  accel_count; 	// 加减速阶段计数值
  __IO int32_t  medle_delay;
}speedRampData;
 
typedef enum{
  NORMAL,
  INDOG,
  NEGLIM,
}BeginState_Typedef;
/* 宏定义 --------------------------------------------------------------------*/
#define 	AXIS_1  0   //各轴标号
#define 	AXIS_2  1
#define 	AXIS_3  2
#define 	AXIS_4  3
#define 	AXIS_5  4  
#define 	AXIS_6  5
#define 	AXIS_7  6
#define 	AXIS_8  7
#define 	AXIS_9  8  
#define 	AXIS_10  9
#define 	AXIS_11  10
#define 	AXIS_12  11

#define 	RISE_DECLINE_CYCLE  11.5

//TIM2
// 电机1相关引脚定义
#define STEPMOTOR_TIM2_CHANNEL1                TIM_CHANNEL_1			  
#define STEPMOTOR_TIM2_IT_CC1                  TIM_IT_CC1			    
#define STEPMOTOR_TIM2_FLAG_CC1                TIM_FLAG_CC1			  
#define STEPMOTOR_TIM2_PULSE_PIN_1             m1p_Pin          

#define STEPMOTOR_1_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()  
#define STEPMOTOR_1_DIR_PORT                  GPIOD                         
#define STEPMOTOR_1_DIR_PIN                   m1d_Pin                     

// 电机2相关引脚定义
#define STEPMOTOR_TIM2_CHANNEL2                TIM_CHANNEL_2			            
#define STEPMOTOR_TIM2_IT_CC2                  TIM_IT_CC2			            
#define STEPMOTOR_TIM2_FLAG_CC2                TIM_FLAG_CC2                 
#define STEPMOTOR_TIM2_PULSE_PIN_2             m2p_Pin                

#define STEPMOTOR_2_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()    
#define STEPMOTOR_2_DIR_PORT                  GPIOD                           
#define STEPMOTOR_2_DIR_PIN                   m2d_Pin        

// 电机3相关引脚定义
#define STEPMOTOR_TIM2_CHANNEL3                TIM_CHANNEL_3			         
#define STEPMOTOR_TIM2_IT_CC3                  TIM_IT_CC3			
#define STEPMOTOR_TIM2_FLAG_CC3                TIM_FLAG_CC3
#define STEPMOTOR_TIM2_PULSE_PIN_3             m3p_Pin                   

#define STEPMOTOR_3_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()     
#define STEPMOTOR_3_DIR_PORT                  GPIOD                            
#define STEPMOTOR_3_DIR_PIN                   m3d_Pin                      

// 电机4相关定义
#define STEPMOTOR_TIM2_CHANNEL4                TIM_CHANNEL_4			            
#define STEPMOTOR_TIM2_IT_CC4                  TIM_IT_CC4	
#define STEPMOTOR_TIM2_FLAG_CC4                TIM_FLAG_CC4
#define STEPMOTOR_TIM2_PULSE_PIN_4             m4p_Pin                   

#define STEPMOTOR_4_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()     
#define STEPMOTOR_4_DIR_PORT                  GPIOD                           
#define STEPMOTOR_4_DIR_PIN                   m4d_Pin                      

// 电机5相关定义
#define STEPMOTOR_TIM3_CHANNEL1                TIM_CHANNEL_1			            
#define STEPMOTOR_TIM3_IT_CC1                  TIM_IT_CC1	
#define STEPMOTOR_TIM3_FLAG_CC1                TIM_FLAG_CC1
#define STEPMOTOR_TIM3_PULSE_PIN_5             m5p_Pin                   

#define STEPMOTOR_5_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()     
#define STEPMOTOR_5_DIR_PORT                  GPIOC                           
#define STEPMOTOR_5_DIR_PIN                   m5d_Pin      

// 电机6相关定义
#define STEPMOTOR_TIM3_CHANNEL2                TIM_CHANNEL_2			            
#define STEPMOTOR_TIM3_IT_CC2                  TIM_IT_CC2
#define STEPMOTOR_TIM3_FLAG_CC2                TIM_FLAG_CC2
#define STEPMOTOR_TIM3_PULSE_PIN_6             m6p_Pin                   

#define STEPMOTOR_6_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()     
#define STEPMOTOR_6_DIR_PORT                  GPIOC                           
#define STEPMOTOR_6_DIR_PIN                   m6d_Pin      

// 电机7相关定义
#define STEPMOTOR_TIM3_CHANNEL3                TIM_CHANNEL_3			            
#define STEPMOTOR_TIM3_IT_CC3                  TIM_IT_CC3	
#define STEPMOTOR_TIM3_FLAG_CC3                TIM_FLAG_CC3
#define STEPMOTOR_TIM3_PULSE_PIN_7             m7p_Pin                   

#define STEPMOTOR_7_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()     
#define STEPMOTOR_7_DIR_PORT                  GPIOC                           
#define STEPMOTOR_7_DIR_PIN                   m7d_Pin      

// 电机8相关定义
#define STEPMOTOR_TIM3_CHANNEL4                TIM_CHANNEL_4			            
#define STEPMOTOR_TIM3_IT_CC4                  TIM_IT_CC4	
#define STEPMOTOR_TIM3_FLAG_CC4                TIM_FLAG_CC4
#define STEPMOTOR_TIM3_PULSE_PIN_8             m8p_Pin                   

#define STEPMOTOR_8_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()     
#define STEPMOTOR_8_DIR_PORT                  GPIOC                           
#define STEPMOTOR_8_DIR_PIN                   m8d_Pin   

// 电机9相关定义
#define STEPMOTOR_TIM4_CHANNEL1                TIM_CHANNEL_1			            
#define STEPMOTOR_TIM4_IT_CC1                  TIM_IT_CC1	
#define STEPMOTOR_TIM4_FLAG_CC1                TIM_FLAG_CC1
#define STEPMOTOR_TIM4_PULSE_PIN_9             m9p_Pin                   

#define STEPMOTOR_9_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()     
#define STEPMOTOR_9_DIR_PORT                  GPIOD                           
#define STEPMOTOR_9_DIR_PIN                   m9d_Pin      

// 电机10相关定义
#define STEPMOTOR_TIM4_CHANNEL2                TIM_CHANNEL_2			            
#define STEPMOTOR_TIM4_IT_CC2                  TIM_IT_CC2
#define STEPMOTOR_TIM4_FLAG_CC2                TIM_FLAG_CC2
#define STEPMOTOR_TIM4_PULSE_PIN_10             m10p_Pin                   

#define STEPMOTOR_10_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()     
#define STEPMOTOR_10_DIR_PORT                  GPIOD                           
#define STEPMOTOR_10_DIR_PIN                   m10d_Pin      

// 电机11相关定义
#define STEPMOTOR_TIM4_CHANNEL3                TIM_CHANNEL_3			            
#define STEPMOTOR_TIM4_IT_CC3                  TIM_IT_CC3	
#define STEPMOTOR_TIM4_FLAG_CC3                TIM_FLAG_CC3
#define STEPMOTOR_TIM4_PULSE_PIN_11             m11p_Pin                   

#define STEPMOTOR_11_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()     
#define STEPMOTOR_11_DIR_PORT                  GPIOD                           
#define STEPMOTOR_11_DIR_PIN                   m11d_Pin      

// 电机12相关定义
#define STEPMOTOR_TIM4_CHANNEL4                TIM_CHANNEL_4			            
#define STEPMOTOR_TIM4_IT_CC4                  TIM_IT_CC4	
#define STEPMOTOR_TIM4_FLAG_CC4                TIM_FLAG_CC4
#define STEPMOTOR_TIM4_PULSE_PIN_12             m12p_Pin                   

#define STEPMOTOR_12_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()     
#define STEPMOTOR_12_DIR_PORT                  GPIOD                           
#define STEPMOTOR_12_DIR_PIN                   m12d_Pin   

#define STEPMOTOR_DIR_FORWARD(Axis) \
HAL_GPIO_WritePin(Stepmotor[Axis].Dir_Port,Stepmotor[Axis].Dir_Pin,GPIO_PIN_RESET)//设置电机方向,参数Axis:当前活动轴
#define STEPMOTOR_DIR_REVERSAL(Axis)\
HAL_GPIO_WritePin(Stepmotor[Axis].Dir_Port,Stepmotor[Axis].Dir_Pin,GPIO_PIN_SET)


// 定义定时器预分频，定时器实际时钟频率为：72MHz/（STEPMOTOR_TIMx_PRESCALER+1）
#define STEPMOTOR_TIM_PRESCALER                3  // 步进电机驱动器细分设置为：   32  细分
//#define STEPMOTOR_TIM_PRESCALER               7  // 步进电机驱动器细分设置为：   16  细分
//#define STEPMOTOR_TIM_PRESCALER               15  // 步进电机驱动器细分设置为：   8  细分
//#define STEPMOTOR_TIM_PRESCALER               31  // 步进电机驱动器细分设置为：   4  细分
//#define STEPMOTOR_TIM_PRESCALER               63  // 步进电机驱动器细分设置为：   2  细分
//#define STEPMOTOR_TIM_PRESCALER               127  // 步进电机驱动器细分设置为：  1  细分

// 定义定时器周期，输出比较模式周期设置为0xFFFF
#define STEPMOTOR_TIM_PERIOD                  0xFFFF

#define FALSE                                 0
#define TRUE                                  1
#define MOTOR_DIR_CW                          1  // 顺时针:正方向
#define MOTOR_DIR_CCW                         -1 // 逆时针:反方向

#define ORIGIN_POSITION	                      0  // 原点坐标
#define STOP                                  0  // 加减速曲线状态：停止
#define ACCEL                                 1  // 加减速曲线状态：加速阶段
#define DECEL                                 2  // 加减速曲线状态：减速阶段
#define RUN                                   3  // 加减速曲线状态：匀速阶段
#define DECEL_MEDLE                           4  // 减速到爬行速度


#define T1_FREQ                               (SystemCoreClock/(STEPMOTOR_TIM_PRESCALER+1)) // 频率ft值
#define FSPR                                  200// 步进电机单圈步数  步距角:1.8° 360/1.8 = 200 正常情况下需要200步转一圈
#define MICRO_STEP                            32 // 步进电机驱动器细分数
#define SPR                                   (FSPR*MICRO_STEP)   // 旋转一圈需要的脉冲数

// 数学常数
#define ALPHA                                 ((float)(2*3.14159/SPR))       // α= 2*pi/spr步距角
#define A_T_x10                               ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148                           ((float)((T1_FREQ*0.676)/10))  // 0.676为误差修正值
#define A_SQ                                  ((float)(2*100000*ALPHA)) 
#define A_x200                                ((float)(200*ALPHA))
#define MAX_NUM_LAP 						              INT32_MAX
#define MAX_NUM_STEP 						              UINT32_MAX

#define UNIT_STEP_MM                          (SPR/UNITS_DISTANCE)//步进1mm需要的步数
#define MAX_STEP_MM                           (MAX_DISTANCE/UNITS_DISTANCE)*UNIT_STEP_MM //
#define UNITS_DISTANCE                        5   //步进电机转一圈,导轨前进5mm
#define MAX_DISTANCE                          400 //导轨可以移动的最长距离400mm

#define  FASTSEEK_SPEED   300		//原点回归速度
#define  SLOWSEEK_SPEED   100		//原点回归爬行速度

/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim2_STEPMOTOR;
extern TIM_HandleTypeDef htim3_STEPMOTOR;
extern TIM_HandleTypeDef htim4_STEPMOTOR;
/* 函数声明 ------------------------------------------------------------------*/
void HAL_TIM_OC_Callback(uint8_t Axis);
void STEPMOTOR_TIMx_Init( void );
void STEPMOTOR_AxisMoveRel( uint8_t Axis, int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void STEPMOTOR_AxisMoveAbs( uint8_t Axis, int32_t targert_step, uint32_t accel, uint32_t decel, uint32_t speed);

void STEPMOTOR_DisMoveRel( uint8_t Axis, int16_t distance, uint32_t accel, uint32_t decel, uint32_t speed);
void STEPMOTOR_DisMoveAbs( uint8_t Axis, uint16_t Target_Dis, uint32_t accel, uint32_t decel, uint32_t speed);

void STEPMOTOR_AxisHome( uint8_t Axis, int32_t fastseek_speed,  uint32_t slowseek_speed,
                         uint32_t accel, uint32_t decel);
#endif	/* __Y_STEP_MOTOR_H */
/****************************END OF FILE****************************/

