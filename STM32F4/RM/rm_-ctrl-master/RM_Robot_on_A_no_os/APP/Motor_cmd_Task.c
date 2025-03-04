#include "Motor_cmd_Task.h"
#include "main.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "pid.h"
#include "remote_control.h"


extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern PID_typedef 			Motor_pid[14];
extern Motor_measure_t  Motor_measure[14];
extern RC_Type remote_control;
int16_t spd;

void Motor_cmd_task(void const * argument)
{
	can_filter_init();
	PID_Total_Init();
	
	for(;;){

		//template begin
		//≤‚ ‘:  M2006£¨ID£∫1  RC-CH1  Maxspd:5000
		spd = remote_control.ch1*5000/660;
		Motor_measure[0].Output = PID_calc(&Motor_pid[0],Motor_measure[0].speed,spd);
		Set_motor_cmd(&hcan1,First_STDID,Motor_measure[0].Output ,0,0,0);
		//template end 
		
		osDelay(10);
	}
}


















