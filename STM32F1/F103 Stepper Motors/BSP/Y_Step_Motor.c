#include "Y_Step_motor.h"
#include "tim.h"

/***************************************************************************************************************
  * 函数名称： GetTData
  * 函数描述：  根据y=a*x+b线性加速,斜率a=(SpeedMax-SpeedMin)/nAccTime,b=SpeedMin,nAccTime单位为ms,speed步/秒
  * 参数 pTData 生成的加速数据
  * 返回值：数组的长度
  * 其它说明： 
  * 
****************************************************************************************************************/

//void Y_motor_init(){
//	unsigned short pTData[500]={0};
//}
unsigned short pTData[500]={0};

int CurrentSpeed=0;

pSmCtrl_typedef ppSmCtrl={0};
pSmCtrl_typedef *pSmCtrl=&ppSmCtrl;

short GetTData(unsigned short* pTData,short SpeedMin,short SpeedMax,short nAccTime)
{
	short TabLen= 0;	
	double tSum=0;
	short RealSpeed=SpeedMin;//实时速度初始值
	unsigned int nAccTimer = nAccTime*1000*72;	//加速时间对应的定时器的数值:1S定时器数值变化72M次，那么1ms就变化72000次                                     
	short deltaSpeed = (SpeedMax-SpeedMin); 	//为了加快计算速度，所以预先计算斜率数值的一部分
	
	while(RealSpeed<SpeedMax)
	{
		RealSpeed = deltaSpeed*tSum/nAccTimer+SpeedMin;//deltaSpeed/nAccTimer是斜率a，SpeedMin是b
		pTData[TabLen] = 72000000/2*RealSpeed;//由于发一个高电平然后一个低电平电机才走一步，所以单个电平的速度需要乘2
		tSum  += 2*pTData[TabLen];		//走一步                                                                                                 需要一个高电平和低电平所以*2 
		TabLen++; 
	}

	return TabLen;
}
/***************************************************************************************************************
  * 函数名称：  路径规划
  * 函数描述： speed-运行速度，step-需要走的步数
  *speed 是步进电机的目标速度，单位是脉冲/秒。这个参数表示你想要设置的步进电机的运行速度
  * 其它说明： 
  * 
****************************************************************************************************************/
void PlanMovePath(unsigned short speed,unsigned int step,unsigned int TabLen,unsigned int SpeedMax)
{	

	extern unsigned short pTData[500];
	unsigned short nTimerSpeed = 72000000/2*speed;	//speed 1S脉冲个数 12M
	unsigned short TabIndex =0;
	for(int i=0;i<TabLen;i++)
	{
		if(pTData[i]<=nTimerSpeed)  
		{
			TabIndex = i;
			break;
		}
	}

	if(speed>SpeedMax)
	{
		TabIndex = TabLen;
	}

	if((TabIndex) <= (step>>1))	//梯形走法,取step的一半
	{
		pSmCtrl->AccStep = TabIndex;
		pSmCtrl->ConstStep = step - (TabIndex)*2;
		pSmCtrl->DecStep = pSmCtrl->AccStep;
	}
	else//三角形
	{
		pSmCtrl->AccStep = (step>>1);
		pSmCtrl->ConstStep = 0;
		pSmCtrl->DecStep = step-pSmCtrl->AccStep;
	}
	
	if(step==1)
	{
		pSmCtrl->AccStep = 1;
		pSmCtrl->ConstStep = 0;
		pSmCtrl->DecStep = 0;
	}
}

void ReloadTimer(TIM_HandleTypeDef *htim, uint32_t period)
{
    // 计算重装载值
    uint32_t reloadValue = (PWM_FREQUENCE / period) - 1;

    // 停止定时器
    HAL_TIM_Base_Stop(htim);

    // 更新重装载值
    htim->Instance->ARR = reloadValue;

    // 清除定时器中断标志位
    __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);

    // 重启定时器
    HAL_TIM_Base_Start(htim);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	return;
}
if (pSmCtrl->AccStep != 0)      //加速阶段
{
    pSmCtrl->AccStep--;
    ReloadTimer(&htim2, pTData[CurrentSpeed]);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}
else if (pSmCtrl->ConstStep != 0)    //匀速阶段
{
    pSmCtrl->ConstStep--;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}
else if (pSmCtrl->DecStep != 0)  //减速阶段
{
    pSmCtrl->DecStep--;
    if (CurrentSpeed > 0)
    {
        CurrentSpeed--;
    }

    ReloadTimer(htim, pTData[CurrentSpeed]);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}
else
{
    HAL_TIM_Base_Stop(&htim2);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);    
}
}

