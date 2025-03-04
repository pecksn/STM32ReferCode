#include "pid.h"
#include <math.h>
#include <main.h>

PID_typedef Motor_pid[14];

/**
  * @brief          输出值限制
  * @retval         none
  */
#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

/**
  * @brief          pid整体初始化
  * @retval         none
  */		
void PID_total_init(void)
{
	for(uint8_t i=0;i<2;i++){
		PID_Init(&Motor_pid[i],PID_POSITION_SPEED ,10,0.1,0,8000,16384,0);
	}
}

/**
  * @brief          pid struct data init，结构体初始化，计算的相关参数通过函数参数传到pid结构体
  * @param[out]     pid: PID结构数据指针
  * @param[in]      mode: PID_POSITION_SPEED: 位置式PID，速度
  *                 			PID_POSITION_ANGLE: 位置式PID，角度
	*  											PID_DELTA_SPEED		：差分式PID，速度
  * @param[in]      kp:PID参数p
	* @param[in] 			ki:PID参数i
	* @param[in] 			kd:PID参数d
  * @param[in]      Max_iout:pid最大积分输出
  * @param[in]      Max_out:pid最大输出
	* @param[in]			deadband:PID死区
  * @retval         none
  */
void PID_Init(PID_typedef *PID,PID_mode Mode,float kp,float ki,float kd,float Max_iout,float Max_out,float deadband)
{
	if(PID == NULL)	return;
	
	PID->mode = Mode;																														//把函数参数传入对应的结构体中
	PID->Kp = kp;
	PID->Ki = ki;
	PID->Kd = kd;
	PID->Max_iout = Max_iout;
	PID->Max_out = Max_out;
	PID->DeadBand = deadband;
}

/**
  * @brief          pid计算
  * @param[out]     pid: PID结构数据指针
  * @param[in]      measure:反馈测量数据
	* @param[in] 			target: 目标值
  * @retval         none
  */
float PID_calc(PID_typedef *PID, float measure, float target)
{
	if(PID == NULL)
		return 0;
	
	
	PID->error[2] = PID->error[1];																							//误差值传递，1为上一次，2为上上次
	PID->error[1] = PID->error[0];																							//误差值传递，0为最新误差，1为上一次
	PID->measure = measure;																											//参数传递，下同
	PID->target = target;
	PID->error[0] =target - measure;																						//误差值计算，目标值-测量值
	
	//if(fabsf(PID->error[0]) > PID->DeadBand || PID->DeadBand==0){								//判断死区，不死区内->计算
		
		if(PID->mode == PID_POSITION_SPEED || PID->mode == PID_POSITION_ANGLE){		//位置式PID计算
			
			if(PID->mode == PID_POSITION_ANGLE){																		//位置式PID-角度环
				if(PID->error[0]>4096)	PID->error[0]=PID->error[0]-8191;							//角度环误差处理，限制在-4096 ~~ +4096
				else if(PID->error[0]<-4096)	PID->error[0]=PID->error[0]+8191;
			}
			PID->Pout = PID->Kp * PID->error[0];																		//p输出
			PID->Iout += PID->Ki * PID->error[0];																		//i输出，累加																			//前后误差差值传递，0为最新差值
			PID->D_item = (PID->error[0] - PID->error[1]);													//微分项计算
			PID->Dout = PID->Kd * PID->D_item;																			//d输出，通过微分项
			LimitMax(PID->Iout,PID->Max_iout);																			//i输出限制
			PID->OUT = PID->Pout + PID->Iout + PID->Dout;														//总输出
			LimitMax(PID->OUT,PID->Max_out);																				//输出限制
		}
		else if(PID->mode == PID_DELTA_SPEED){																		//增量式PID-速度环
			PID->Pout = PID->Kp * (PID->error[0] - PID->error[1]);									//p输出
			PID->Iout = PID->Ki * PID->error[0];																		//i输出
			PID->D_item = (PID->error[0] - 2.0f*PID->error[1] + PID->error[2]);			//微分项计算
			PID->Dout = PID->Kd * PID->D_item;																			//d输出，通过微分项
			PID->OUT += PID->Pout + PID->Iout + PID->Dout;													//总输出
			LimitMax(PID->OUT, PID->Max_out);																				//输出限制
		}
	//}
	else{
        PID->OUT=0;																														//误差在死区内，输出为0
	}
	
	return PID->OUT;																														//返回输出值
}














