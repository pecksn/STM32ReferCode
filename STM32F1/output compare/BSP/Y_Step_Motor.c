#include "Y_Step_motor.h"
#include "tim.h"
#include <math.h>

/* 私有类型定义 --------------------------------------------------------------*/
//脉冲输出通道
typedef struct {
  uint16_t  Pulse_Pin ; 	// 定时器脉冲输出引脚
  uint32_t  Channel;		  // 定时器通道
  uint32_t  IT_CCx ;  		// 定时器通道中断使能位
  uint32_t  Flag_CCx ;    // 定时器SR中断标记位
}Tim;

typedef struct{\
  uint16_t  Dir_Pin ;     //电机方向引脚编号
  GPIO_TypeDef *Dir_Port; //电机方向引脚端口
}StepMotor;

/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
TIM_HandleTypeDef htimx_STEPMOTOR;
speedRampData srd[4] ={\
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},   // 加减速曲线变量
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},
};
  /* 定时器脉冲输出引脚,需要修改直接在stepmotor.h头文件修改即可*/
const Tim Timer[4]={\
{STEPMOTOR_TIM_PULSE_PIN_X, STEPMOTOR_TIM_CHANNEL1, STEPMOTOR_TIM_IT_CC1, STEPMOTOR_TIM_FLAG_CC1},//X轴
{STEPMOTOR_TIM_PULSE_PIN_Y, STEPMOTOR_TIM_CHANNEL2, STEPMOTOR_TIM_IT_CC2, STEPMOTOR_TIM_FLAG_CC2},//Y轴
{STEPMOTOR_TIM_PULSE_PIN_Z, STEPMOTOR_TIM_CHANNEL3, STEPMOTOR_TIM_IT_CC3, STEPMOTOR_TIM_FLAG_CC3},//Z轴
{STEPMOTOR_TIM_PULSE_PIN_R, STEPMOTOR_TIM_CHANNEL4, STEPMOTOR_TIM_IT_CC4, STEPMOTOR_TIM_FLAG_CC4},//R轴
};


  /* 步进电机控制引脚*/
const StepMotor Stepmotor[4]={\
{STEPMOTOR_X_DIR_PIN,STEPMOTOR_X_DIR_PORT},
{STEPMOTOR_Y_DIR_PIN,STEPMOTOR_Y_DIR_PORT},
{STEPMOTOR_Z_DIR_PIN,STEPMOTOR_Z_DIR_PORT},
{STEPMOTOR_R_DIR_PIN,STEPMOTOR_R_DIR_PORT},
};
//__IO uint8_t  ZeroStep[4] = {IDLE,IDLE,IDLE,IDLE};	        //搜索原点状态机
__IO uint8_t  LimPosi[4]  = {FALSE,FALSE,FALSE,FALSE};      //正转极限标志位  True:到达极限位置  False:未到达极限位置
__IO uint8_t  LimNega[4]  = {FALSE,FALSE,FALSE,FALSE};      //反转极限标志位
__IO uint8_t  MotionStatus[4]   = {STOP,STOP,STOP,STOP};        // 是否在运动？0：停止，1：运动
__IO uint8_t  DOG[4]         		= {FALSE,FALSE,FALSE,FALSE};		// 近点信号
__IO uint8_t  HomeCapture[4]    = {FALSE,FALSE,FALSE,FALSE};		// 原点捕获标志
__IO int8_t   OriginDir[4] = {MOTOR_DIR_CCW,MOTOR_DIR_CCW,MOTOR_DIR_CCW,MOTOR_DIR_CCW};		// 搜索原点方向 默认逆时针
__IO int32_t  step_position[4]  = {0,0,0,0};      // 当前位置  单位:脉冲数
__IO int16_t  location[4]       = {0,0,0,0};      // 当前位置  单位:毫米(mm)
__IO uint8_t  OriginFlag[4] = {0};                // 用于标记启动回原点控制
__IO  BeginState_Typedef  BeginState[4] = {NORMAL,NORMAL,NORMAL,NORMAL};// 滑块起始位置

/* 扩展变量 ------------------------------------------------------------------*/
extern uint32_t set_speed;          // 速度 单位为0.05rad/sec
extern uint32_t step_accel;         // 加速度 单位为0.025rad/sec^2
extern uint32_t step_decel;         // 减速度 单位为0.025rad/sec^2
extern uint32_t fastseek_Speed ;    // 回归速度
extern uint32_t slowseek_Speed ;    // 爬行速度

/* 私有函数原形 --------------------------------------------------------------*/
static void STEPMOTOR_GPIO_Init(void);
void StopMotor(uint8_t Axis);

/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: STEPMOTOR相关GPIO初始化配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void STEPMOTOR_GPIO_Init()
{
	uint8_t i = 0;     
	GPIO_InitTypeDef GPIO_InitStruct; 
  
  /* 电机方向控制引脚端口时钟使能 */
	STEPMOTOR_PULSE_GPIO_CLK_ENABLE();
	STEPMOTOR_X_DIR_GPIO_CLK_ENABLE();
	STEPMOTOR_Y_DIR_GPIO_CLK_ENABLE();
	STEPMOTOR_Z_DIR_GPIO_CLK_ENABLE();
	STEPMOTOR_R_DIR_GPIO_CLK_ENABLE();
  

  for(i=0; i<4; i++)
  {
    /* 步进电机驱动器：脉冲输出 */
    GPIO_InitStruct.Pin = Timer[i].Pulse_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(STEPMOTOR_TIM_PULSE_PORT, &GPIO_InitStruct);
    
    /* 步进电机驱动器：方向控制 */
    STEPMOTOR_DIR_FORWARD(i);//默认设置为顺时针方向
    GPIO_InitStruct.Pin = Stepmotor[i].Dir_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(Stepmotor[i].Dir_Port, &GPIO_InitStruct);

}
 }
/**
  * 函数功能: 步进电机驱动器定时器初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void STEPMOTOR_TIMx_Init()
{
	uint8_t i = 0;	                           
	TIM_ClockConfigTypeDef sClockSourceConfig;  // 定时器时钟
	TIM_OC_InitTypeDef sConfigOC;               // 定时器通道比较输出
  
	STEPMOTOR_TIM_RCC_CLK_ENABLE();
  
	/* STEPMOTOR相关GPIO初始化配置 */
	STEPMOTOR_GPIO_Init();
  
	/* 定时器基本环境配置 */
	htimx_STEPMOTOR.Instance = STEPMOTOR_TIMx;                      	// 定时器编号
	htimx_STEPMOTOR.Init.Prescaler = STEPMOTOR_TIM_PRESCALER;        	// 定时器预分频器
	htimx_STEPMOTOR.Init.CounterMode = TIM_COUNTERMODE_UP;            // 计数方向：向上计数
	htimx_STEPMOTOR.Init.Period = STEPMOTOR_TIM_PERIOD;          		  // 定时器周期
	htimx_STEPMOTOR.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;        // 时钟分频
	HAL_TIM_Base_Init(&htimx_STEPMOTOR);

	/* 定时器时钟源配置 */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       	// 使用内部时钟源
	HAL_TIM_ConfigClockSource(&htimx_STEPMOTOR, &sClockSourceConfig);

	/* 定时器比较输出配置 */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                // 比较输出模式：反转输出
	sConfigOC.Pulse = STEPMOTOR_TIM_PERIOD;              // 脉冲数
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;           // 输出极性
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;           // 快速模式

	/* 使能比较输出通道 */
	for (i=0; i <4; i++)
	{
		HAL_TIM_OC_Stop_IT (&htimx_STEPMOTOR,Timer[i].Channel); 
    HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, Timer[i].Channel);
		TIM_CCxChannelCmd(STEPMOTOR_TIMx, Timer[i].Channel, TIM_CCx_DISABLE);
	}

	/* 配置定时器中断优先级并使能 */
	HAL_NVIC_SetPriority(STEPMOTOR_TIMx_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(STEPMOTOR_TIMx_IRQn);
  
	/* Enable the main output */
	__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR);  
	HAL_TIM_Base_Start(&htimx_STEPMOTOR);// 使能定时器
}
/**
  * 函数功能: 相对位置运动：运动给定的步数
  * 输入参数: Axis    活动轴
  *           step   移动的步数 (正数为顺时针，负数为逆时针).
  *           accel  加速度,实际值为accel*0.1*rad/sec^2
  *           decel  减速度,实际值为decel*0.1*rad/sec^2
  *           speed  最大速度,实际值为speed*0.1*rad/sec
  * 返 回 值: 无
  * 说    明: 以给定的步数移动步进电机，先加速到最大速度，然后在合适位置开始
  *           减速至停止，使得整个运动距离为指定的步数。如果加减速阶段很短并且
  *           速度很慢，那还没达到最大速度就要开始减速
  */
void STEPMOTOR_AxisMoveRel(uint8_t Axis,int32_t step,uint32_t accel, uint32_t decel, uint32_t speed)
{  
	__IO uint16_t tim_count;//获取定时器的计数值，无符号32位整数，且该整数可能会被意外地修改
  // 达到最大速度时的步数
	__IO uint32_t max_s_lim;
  // 必须要开始减速的步数（如果加速没有达到最大速度）
	__IO uint32_t accel_lim;
  
	//在电机运动过程一般不能再继续操作,除了在回原点过程中
	if(MotionStatus[Axis] != STOP)	
	{
		return ;
	}
  
	/* 方向设定 */
	if(step < 0) // 步数为负数
	{
		srd[Axis].dir = MOTOR_DIR_CCW; // 逆时针方向旋转
		STEPMOTOR_DIR_REVERSAL(Axis);
		step = -step;   // 获取步数绝对值
	}
	else
	{
		srd[Axis].dir = MOTOR_DIR_CW; // 顺时针方向旋转
		STEPMOTOR_DIR_FORWARD(Axis);
	}

	if(step == 1)    // 步数为1
	{
		srd[Axis].accel_count = -1;   // 只移动一步
		srd[Axis].run_state = DECEL;  // 减速状态.
		srd[Axis].step_delay = 1000;	// 短延时	
	}
	else if(step != 0)  // 如果目标运动步数不为0
	{
		// 我们的电机控制专题指导手册有详细的计算及推导过程

		// 设置最大速度极限, 计算得到min_delay用于定时器的计数器的值。
		// min_delay = (alpha / tt)/ w
		srd[Axis].min_delay = (int32_t)(A_T_x10/speed);

		// 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.1rad/sec^2
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100
		srd[Axis].step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);//C0,初始速度的定时器值
			

		/*计算加减速需要的参数*/
		// 计算多少步之后达到最大速度的限制
		// max_s_lim = speed^2 / (2*alpha*accel)
		max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));
		// 如果达到最大速度小于0.5步，我们将四舍五入为0
		// 但实际我们必须移动至少一步才能达到想要的速度
		if(max_s_lim == 0)
		{
		  max_s_lim = 1;
		}
			
		// 计算多少步之后我们必须开始减速
		// n1 = (n1+n2)decel / (accel + decel)
		accel_lim = (uint32_t)(step*decel/(accel+decel));
		// 我们必须加速至少1步才能才能开始减速.
		if(accel_lim == 0)
		{
		  accel_lim = 1;
		}
		// 使用限制条件我们可以计算出减速阶段步数
		if(accel_lim <= max_s_lim)
		{
		  srd[Axis].decel_val = accel_lim - step;
		}
		else{
		  srd[Axis].decel_val = -(max_s_lim*accel/decel);
		}
		// 当只剩下一步我们必须减速
		if(srd[Axis].decel_val == 0)
		{
		  srd[Axis].decel_val = -1;
		}

		// 计算开始减速时的步数
		srd[Axis].decel_start = step + srd[Axis].decel_val;

		// 如果最大速度很慢，我们就不需要进行加速运动
		if(srd[Axis].step_delay <= srd[Axis].min_delay)
		{
			srd[Axis].step_delay = srd[Axis].min_delay;
			srd[Axis].run_state = RUN;
		}
		else
		{
			srd[Axis].run_state = ACCEL;
		}    
		// 复位加速度计数值
		srd[Axis].accel_count = 0;
		
	}
	MotionStatus[Axis] = 1;
  
	tim_count=__HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
	__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,Timer[Axis].Channel,tim_count+srd[Axis].step_delay); // 设置定时器比较值
  HAL_TIM_OC_Start_IT (&htimx_STEPMOTOR,Timer[Axis].Channel);
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, Timer[Axis].Channel, TIM_CCx_ENABLE);// 使能定时器通道 
	
}

/** 函数功能: 定点移动
  * 输入参数: targert_step:目标的位置
  *			      accel:加速度
  *			      decel:减速度
  *			      speed:最大速度
  * 返 回 值: 无
  * 说    明: 实现定点移动,输入目标位置相对于原点的步数,
  *  		      以加速度加速到最大速度后匀速寻找目标位置,
  *			      然后在合适的位置减速,到达目标位置.
  */
void STEPMOTOR_AxisMoveAbs(uint8_t Axis, int32_t targert_step,uint32_t accel, uint32_t decel, uint32_t speed)
{
  int32_t rel_step = 0;
	int8_t dir = -1;                           //回原点的方向控制
	rel_step = step_position[Axis]-targert_step ; 	//获取当前位置和目标位置之间的步数值
	
	if(rel_step == 0)	
	{
		dir = 0;
	}
	STEPMOTOR_AxisMoveRel(Axis,dir*rel_step,accel,decel,speed);
}
/** 函数功能: 相对移动一定距离;单位:mm
  * 输入参数: distance  需要移动的距离,以正负区分方向,正数就是正方向,
  *                    负数就是反方向
  *            accel  加速度
  *            decel  减速度
  *            speed  最大速度
  * 返 回 值: 无
  * 说    明: 从当前位置上在导轨上移动一定的距离,以参数distance的正负区分方向
 */
void STEPMOTOR_DisMoveRel(__IO uint8_t Axis, __IO int16_t distance, __IO uint32_t accel, __IO uint32_t decel, __IO uint32_t speed)
{
  __IO int32_t step;
  step = distance*UNIT_STEP_MM;//获得步进目标距离的步数
  STEPMOTOR_AxisMoveRel(Axis,step,accel,decel,speed);
}
/**
  * 函数功能: 移动到目标位置;单位:mm
  * 输入参数: Target_Dis  目标坐标位置
  *           accel  加速度
  *           decel  减速度
  *           speed  最大速度
  * 返 回 值: 最大速度
  * 说    明: 移动到给定的坐标位置,根据当前的位置计算出与目标位置的距离,
  *           并移动到目标位置,
  *             
  */
void STEPMOTOR_DisMoveAbs(uint8_t Axis,  uint16_t Target_Dis, uint32_t accel, uint32_t decel, uint32_t speed)
{
  int32_t step; 
    
  if(Target_Dis > MAX_DISTANCE)
    return;
  else if(Target_Dis < UNITS_DISTANCE)
    return;
  step = (Target_Dis - location[Axis])*UNIT_STEP_MM;     //获得当前位置与目标位置的距离
  STEPMOTOR_AxisMoveRel(Axis,step,accel,decel,speed);
}

/**
  * 函数功能: 定时器中断回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 实现加减速过程
  */

void HAL_TIM_OC_Callback(uint8_t Axis)
{
  __IO static uint16_t tim_count = 0;
  __IO  uint32_t new_step_delay = 0;
  __IO static uint8_t  i[4]={0,0,0,0};
	__IO static uint16_t last_accel_delay[4] = {0,0,0,0};
	// 总移动步数计数器
	__IO static uint32_t step_count[4] = {0,0,0,0};
	// 记录new_step_delay中的余数，提高下一步计算的精度
	__IO static int32_t rest[4] = {0,0,0,0};
	//定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲

  // 设置比较值
  tim_count = __HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
  __HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,Timer[Axis].Channel,tim_count+srd[Axis].step_delay);

  i[Axis]++;       // 定时器中断次数计数值
  if(i[Axis] == 2) // 2次，说明已经输出一个完整脉冲
  {
    i[Axis] = 0;   // 清零定时器中断次数计数值
//    /* 判断是否处于边缘运动 */
//    if(srd[Axis].dir == MOTOR_DIR_CCW)
//    {
//      if(LimNega[Axis] == TRUE)		//逆时针且 遇到反转的极限位置,停止运动
//      {
//        srd[Axis].run_state = STOP;
//        MotionStatus[Axis] = STOP;
//        LimPosi[Axis] = FALSE;    //正转的极限标志位应为FALSE
//      }
//      else 
//      {
//        LimPosi[Axis] = FALSE;	  //在逆时针方向,在碰到反转的极限位置之前,正转的极限标志位应为FALSE
//      }
//    }
//    else 
//    {
//      if(LimPosi[Axis] == TRUE)		
//      {
//        srd[Axis].run_state = STOP;
//        MotionStatus[Axis] = STOP;
//        LimNega[Axis] = FALSE;    //在顺时针方向,碰到极限位置,反转极限标志位应为False
//      }
//      else 
//      {
//        LimNega[Axis] = FALSE;  //在顺时针方向,在碰到正转的极限位置之前,反转的极限标志位应为FALSE
//      }
//    }
    
    switch(srd[Axis].run_state) // 加减速曲线阶段
    {
      case STOP:
    
        // 关闭通道
        TIM_CCxChannelCmd(STEPMOTOR_TIMx,Timer[Axis].Channel,TIM_CCx_DISABLE);       
        __HAL_TIM_CLEAR_FLAG(&htimx_STEPMOTOR, Timer[Axis].Flag_CCx);
        HAL_TIM_OC_Stop_IT (&htimx_STEPMOTOR,Timer[Axis].Channel);
      
        MotionStatus[Axis] = STOP;  		//  电机为停止状态
        step_count[Axis] = 0;  // 清零步数计数器
        rest[Axis] = 0;        // 清零余值
        last_accel_delay[Axis] = 0;
        srd[Axis].accel_count = 0;
        srd[Axis].step_delay = 0;
        srd[Axis].min_delay = 0;
        break;
  
      case ACCEL:
        step_count[Axis]++;      		// 步数加1
        if(srd[Axis].dir == MOTOR_DIR_CW)
        {		  	
          step_position[Axis]++; 	  // 绝对位置加1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]++;
        }
        else
        {
          step_position[Axis]--; 	  // 绝对位置减1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]--;
        }
        srd[Axis].accel_count++; 	  // 加速计数值加1
        
        new_step_delay = srd[Axis].step_delay - (((2 *srd[Axis].step_delay) + rest[Axis])/(4 * srd[Axis].accel_count + 1));//计算新(下)一步脉冲周期(时间间隔)
        rest[Axis] = ((2 * srd[Axis].step_delay)+rest[Axis])%(4 * srd[Axis].accel_count + 1);// 计算余数，下次计算补上余数，减少误差
        
        if(step_count[Axis] >= srd[Axis].decel_start)		// 检查是否应该开始减速
        {
          srd[Axis].accel_count = srd[Axis].decel_val; 	// 加速计数值为减速阶段计数值的初始值
          srd[Axis].run_state = DECEL;           	      // 下个脉冲进入减速阶段
        }
        else if(new_step_delay <= srd[Axis].min_delay)  // 检查是否到达期望的最大速度
        {
          srd[Axis].accel_count = srd[Axis].decel_val; 	// 加速计数值为减速阶段计数值的初始值
          last_accel_delay[Axis] = new_step_delay; 	// 保存加速过程中最后一次延时（脉冲周期）
          new_step_delay = srd[Axis].min_delay;    	// 使用min_delay（对应最大速度speed）
          rest[Axis] = 0;                          	// 清零余值
          srd[Axis].run_state = RUN;               	// 设置为匀速运行状态
        }	
        last_accel_delay[Axis] = new_step_delay; 	  // 保存加速过程中最后一次延时（脉冲周期）
        break;
        
      case RUN:
        step_count[Axis]++; 		 // 步数加1
        if(srd[Axis].dir==MOTOR_DIR_CW)
        {	  	
          step_position[Axis]++; // 绝对位置加1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]++;
        }
        else
        {
          step_position[Axis]--; // 绝对位置减1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]--;
        }
            
        new_step_delay = srd[Axis].min_delay;       // 使用min_delay（对应最大速度speed）
            
        if(step_count[Axis] >= srd[Axis].decel_start)   // 需要开始减速
        {
          srd[Axis].accel_count = srd[Axis].decel_val;  // 减速步数做为加速计数值
          new_step_delay = last_accel_delay[Axis];// 加阶段最后的延时做为减速阶段的起始延时(脉冲周期)
          srd[Axis].run_state = DECEL;            // 状态改变为减速
        }
        break;
        
      case DECEL:
        step_count[Axis]++; 		 // 步数加1
        if(srd[Axis].dir == MOTOR_DIR_CW)
        {		  	
          step_position[Axis]++; // 绝对位置加1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]++;
        }
        else
        {
          step_position[Axis]--; // 绝对位置减1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]--;
        }
        srd[Axis].accel_count++;
        new_step_delay = srd[Axis].step_delay - (((2 * srd[Axis].step_delay) + rest[Axis])/(4 * srd[Axis].accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
        rest[Axis] = ((2 * srd[Axis].step_delay)+rest[Axis])%(4 * srd[Axis].accel_count + 1);// 计算余数，下次计算补上余数，减少误差
        
        //检查是否为最后一步
        if(srd[Axis].accel_count >= 0)
        {
          StopMotor(Axis);
        }
        break;
      case DECEL_MEDLE:
        new_step_delay = srd[Axis].step_delay - (((2 * srd[Axis].step_delay) + rest[Axis])/(4 * srd[Axis].accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
					rest[Axis] = ((2 * srd[Axis].step_delay)+rest[Axis])%(4 * srd[Axis].accel_count + 1);// 计算余数，下次计算补上余数，减少误差
				  //检查是否为最后一步
					if(new_step_delay >= srd[Axis].medle_delay )
					{
            srd[Axis].min_delay = srd[Axis].medle_delay;
            step_count[Axis] = 0;  // 清零步数计数器
            rest[Axis] = 0;        // 清零余值
            srd[Axis].run_state = RUN;              
					}
        break;

      default :break;
    }      
    if(new_step_delay>0xFFFF)
      new_step_delay=0xFFFF;
    srd[Axis].step_delay = new_step_delay; // 为下个(新的)延时(脉冲周期)赋值
  }
}
/**
  * 函数功能: 停止电机转动
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 停止定时器中断和输出，并且清除加减速的参数。
  *             
  */
void StopMotor(uint8_t Axis)
{
  // 关闭通道输出
  TIM_CCxChannelCmd(STEPMOTOR_TIMx,Timer[Axis].Channel,TIM_CCx_DISABLE);       
  srd[Axis].accel_count = 0;
  srd[Axis].step_delay  = 0;
  srd[Axis].min_delay   = 0;
  srd[Axis].medle_delay = 0;
  srd[Axis].run_state = STOP;    
  MotionStatus[Axis]  = STOP;	
}

/*******************************END OF FILE*****************************************/
