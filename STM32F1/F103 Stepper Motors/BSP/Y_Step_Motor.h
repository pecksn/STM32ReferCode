#ifndef __Y_STEP_MOTOR_H
#define	__Y_STEP_MOTOR_H

#include "tim.h"

typedef struct
{
	float AccStep;
	float ConstStep;
	float DecStep;
}pSmCtrl_typedef;


typedef struct
{
	pSmCtrl_typedef p[2];
}Abc;

extern Abc *abc;
typedef struct
{
	float CurrentSpeed;

}pCurrentMotor_typedef;

extern pSmCtrl_typedef *pSmCtrl;
extern unsigned short pTData[500];
void Y_motor_init(void);
short GetTData(unsigned short* pTData,short SpeedMin,short SpeedMax,short nAccTime);
void ReloadTimer(TIM_HandleTypeDef *htim, uint32_t period);
void PlanMovePath(unsigned short speed,unsigned int step,unsigned int TabIndex,unsigned int SpeedMax);
void ReloadTimer(TIM_HandleTypeDef *htim, uint32_t period);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);



#endif	/* __Y_STEP_MOTOR_H */
/****************************END OF FILE****************************/

