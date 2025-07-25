
#include "shoot.h"
float maxBulletSpeed;

Motor_C610 Turnplate[1] = {Motor_C610(1)};
Motor_C620 FriMotor[3] = {Motor_C620(2),Motor_C620(3),Motor_C620(1)};



void C_Shoot :: Control(E_ShootMode 			shoot_mode,
												E_ShootCtrlMode 	ctrl_mode,
												E_FriWheelState 	fri_wheel_stste,
												E_LaserState    	laser_state,
												E_BulletBayState 	bullet_bay_state,
												E_FireFlag				fire_flag,
												uint8_t 					max_speed,
											  uint16_t 				cooling_rate,
											  uint16_t 				cooling_limit,
											  uint16_t					shooter_heat)
{
	shootMode = shoot_mode;
	fireFlag = fire_flag;
	ctrlMode = ctrl_mode;
	friWheelState = fri_wheel_stste;
	laserState = laser_state;
	bulletBayState = bullet_bay_state;
	
	
	laserCtrl();
	friWheelCtrl();
	bulletBayCtrl();
	heatCalc();
	shootCtrl();
	if(bulletSpeed>maxBulletSpeed)
		maxBulletSpeed = bulletSpeed;
	MotorMsgSend(&hcan1,FriMotor);
}



//热量计算
void C_Shoot :: heatCalc()
{
	if(bulletSpeed!=lastBulletSpeed)
		{
			index++;
			heat+=10;
		}
		if(heatRf-heat>9)
		{
			heat+=10;
		}
		heat-= coolingRate/1000.0;
		if(heat<0)
		{
			heat = 0;
		}
		lastBulletSpeed = bulletSpeed;
}




//发射控制
void C_Shoot :: shootCtrl()
{
	if(fireFlag == STOP_F)
	{
//		delayCnt = 0;
	}
	if(friWheelState == OPEN_F && fireFlag == FIRE_F && fireFlag2 == FIRE_F)
	{
		switch(shootMode)
		{
			case RUN:
				if(((turnPlateAngle.Current-turnPlateAngle.Target)>360*3.6*1.9)&&(delayCnt == 0))  //卡弹2.5倍就回拨
				{
					turnPlateAngle.Target = turnPlateAngle.Current;
				}
				else if(delayCnt == 0 && ((heat<heatLimit - 30
					)||heatLimit == -1)) //调试时关闭热量限制 25
				//if(delayCnt == 0 )
				{
					turnPlateAngle.Target -= 360*3.6;
					if(bulletSpeed > 14.9 && maxSpeed == 15)
					{
						friSpeed_15 -= 20;
					}
					else if(bulletSpeed > 17.9 && maxSpeed == 18)
					{
						friSpeed_18 -= 20;
					}
					else if(bulletSpeed > 29.9 && maxSpeed == 30)
					{
						friSpeed_30 -= 30;
					}
					else
					{
						
					}
				}
				delayCnt++;
				delayCnt%=period;
				break;
		}
	}
	turnPlateAngle.Current = FriMotor[2].getAngle();
	turnPlateSpeed.Target = turnPlateAngle.Adjust();
	turnPlateSpeed.Current = FriMotor[2].getSpeed();
	FriMotor[2].Out = turnPlateSpeed.Adjust();
}


//摩擦轮控制
void C_Shoot :: friWheelCtrl()
{
	switch(friWheelState)
	{
		case OPEN_F:
			switch(maxSpeed)
			{
				case -1:
					leftFriSpeed.Target = -friSpeed_30;
					rightFriSpeed.Target = friSpeed_30;
				break;
				case 15:
					leftFriSpeed.Target = -friSpeed_15;
					rightFriSpeed.Target = friSpeed_15;
				break;
				case 18:
					leftFriSpeed.Target = -friSpeed_18;
					rightFriSpeed.Target = friSpeed_18;
				break;
				case 22:
					leftFriSpeed.Target = -friSpeed_22;
					rightFriSpeed.Target = friSpeed_22;
				break;
				case 30:
					leftFriSpeed.Target = -friSpeed_30;
					rightFriSpeed.Target = friSpeed_30;
				break;
				default:
					leftFriSpeed.Target = -friSpeed_15;
					rightFriSpeed.Target = friSpeed_15;
				break;
			}
		if(friWheelDelay<1000)
		{
			friWheelDelay++;
		}
		else
		{
			fireFlag2 = FIRE_F;
		}
		break;
		case 	CLOSE_F:
			leftFriSpeed.Target = 0;
			rightFriSpeed.Target = 0;
			friWheelDelay = 0;
			fireFlag2 = STOP_F;
		break;
	}
	leftFriSpeed.Current = FriMotor[0].getSpeed();
	rightFriSpeed.Current = FriMotor[1].getSpeed();
	FriMotor[0].Out = leftFriSpeed.Adjust();
	FriMotor[1].Out = rightFriSpeed.Adjust();
}


//红点控制
void C_Shoot :: laserCtrl()
{
	switch(laserState)
	{
		case OPEN_L:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
		break;
		case CLOSE_L:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
		break;
	}
	
}


//弹仓盖控制
void C_Shoot :: bulletBayCtrl()
{
	if(bulletBayDelay == 0) 
	{
		switch(bulletBayState)
		{
			case OPEN_B:
				__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,1890);
				Pitch_Yaw.pitchAngle.Target = 8769;
			break;
			case CLOSE_B:
				__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,610);
			break;
		}
	}
	bulletBayDelay++;
	bulletBayDelay%=500;
}


void C_Shoot :: pid_init(E_ShootPidType type,float kp,float ki,float kd, float ki_max,float out_max)
{
	switch(type)
	{
		case LEFT_FRI_SPEED:
			leftFriSpeed.SetPIDParam(kp, ki, kd, ki_max, out_max);
			break;
		case RIGHT_FRI_SPEED:
			rightFriSpeed.SetPIDParam(kp, ki, kd, ki_max, out_max);
			break;
		case TURNPLATE_SPEED:
			turnPlateSpeed.SetPIDParam(kp, ki, kd, ki_max, out_max);
			break;
		case TURNPLATE_ANGLE:
			turnPlateAngle.SetPIDParam(kp, ki, kd, ki_max, out_max);
	}
}

void C_Shoot :: Reset()
{
	FriMotor[0].Out = FriMotor[1].Out = FriMotor[2].Out = 0;
	turnPlateAngle.Target = turnPlateAngle.Current;
	MotorMsgSend(&hcan1,FriMotor);
	friWheelState = CLOSE_F;
	laserState = CLOSE_L;
	bulletBayState = CLOSE_B;
}

float C_Shoot :: abs(float x)
{
	if(x>0)
		return x;
	else
		return -x;
}
	


