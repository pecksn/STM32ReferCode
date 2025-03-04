/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 学习小组
  * 功    能: 四轴步进电机导轨控制
  ******************************************************************************
  * 说明：
	      利用STM32定时器编程控制四路步进电机，利用定时器的4个通道分别控制四路步进
				电机的开始与停止等运行状态，独立控制每一通道的速度、启动时间等等要素，驱
				动步进电机T型加减速启动、停止、等动作。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "STEPMOTOR.h" 
#include "key.h"
/* 私有类型定义 --------------------------------------------------------------*/
#define  FASTSEEK_SPEED   300		//原点回归速度
#define  SLOWSEEK_SPEED   100		//原点回归爬行速度
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
// 速度最大值由驱动器和电机决定，有些最大是1800，有些可以达到4000
uint32_t set_speed  = 500;         // 速度 单位为0.05rad/sec
// 加速度和减速度选取一般根据实际需要，值越大速度变化越快，加减速阶段比较抖动
// 所以加速度和减速度值一般是在实际应用中多尝试出来的结果
uint32_t step_accel = 150;         // 加速度 单位为0.1rad/sec^2
uint32_t step_decel = 50;          // 减速度 单位为0.1rad/sec^2

uint32_t fastseek_Speed = FASTSEEK_SPEED;
uint32_t slowseek_Speed = SLOWSEEK_SPEED;

/* 扩展变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     // 使能PWR时钟

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  // 设置调压器输出电压级别1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // 外部晶振，8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        // 打开HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    // 打开PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            // PLL时钟源选择HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 // 8分频MHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               // 336倍频
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     // 2分频，得到168MHz主时钟
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 // USB/SDIO/随机数产生器等的主PLL分频系数
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟：168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟： 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1时钟：42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2时钟：84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // 使能CSS功能，优先使用外部晶振，内部时钟源为备用
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                 // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{
  /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
  HAL_Init();
  /* 配置系统时钟 */
  SystemClock_Config();
  /* 配置GPIO作为按键使用 */
  KEY_GPIO_Init();
  /* 配置定时器输出脉冲 */
  STEPMOTOR_TIMx_Init();
  
  /* 无限循环 */
  while (1)
	{
//		STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索X轴原点
//    STEPMOTOR_AxisHome(AXIS_Y,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索Y轴原点
//    STEPMOTOR_AxisHome(AXIS_Z,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
//    STEPMOTOR_AxisHome(AXIS_R,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
    
		if(KEY1_StateRead() == KEY_DOWN)//导轨运行状态控制，形成控制闭环，由开始点、结束点、原点部位的传感器返回信息实现闭环控制。
		{
      STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索X轴原点
      STEPMOTOR_AxisHome(AXIS_Y,2* FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索Y轴原点
      STEPMOTOR_AxisHome(AXIS_Z,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
      STEPMOTOR_AxisHome(AXIS_R,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
		}
		if(KEY2_StateRead() == KEY_DOWN)
		{ //控制脉冲计数方式控制步进电机运行到指定位置（步数控制）实现电机开环控制
      STEPMOTOR_DisMoveAbs(AXIS_X,100,step_accel,step_decel,set_speed);//X轴移动到400mm的位置
      STEPMOTOR_DisMoveAbs(AXIS_Y,200,step_accel,step_decel,set_speed);//Y轴移动到400mm的位置
      STEPMOTOR_DisMoveAbs(AXIS_Z,100,step_accel,step_decel,set_speed);//Z轴移动到400mm的位置
      STEPMOTOR_DisMoveAbs(AXIS_R,100,step_accel,step_decel,set_speed);//R轴移动到400mm的位置
		}
    if(KEY3_StateRead() == KEY_DOWN)
    { //由GPIO点编程控制驱动器的DIR引脚实现运行方向控制
      STEPMOTOR_AxisMoveRel(AXIS_X,30*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // X轴反向移动30圈
      STEPMOTOR_AxisMoveRel(AXIS_Y,60*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // Y轴反向移动30圈
      STEPMOTOR_AxisMoveRel(AXIS_Z,30*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // Z轴反向移动30圈
      STEPMOTOR_AxisMoveRel(AXIS_R,30*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // R轴反向移动30圈
    }
		
    if(KEY4_StateRead() == KEY_DOWN)
    {
      STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索X轴原点
    }		
		
    if(KEY5_StateRead() == KEY_DOWN)
    {
      STEPMOTOR_AxisHome(AXIS_Y,2* FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索Y轴原点
    }		
		
		
  }
}


/*******************************END OF FILE*****************************************/

