#include "Motor_cmd_Task.h"
#include "main.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "pid.h"
#include "remote_control.h"
#include "ANO_send_data.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern PID_typedef 			Motor_pid[14];
extern Motor_measure_t  Motor_measure[14];
extern int16_t setspd[4];
extern RC_Type remote_control;
int16_t tcnt;

void Motor_cmd_Task(void const * argument)
{
	can_filter_init();
	PID_total_init();
	
	
	while(1){
		setspd[0] = remote_control.ch1 * 3000 / 660;
		setspd[1] = -setspd[0];
		
		for(uint8_t i=0;i<2;i++)
				Motor_measure[i].Output = PID_calc(&Motor_pid[i],Motor_measure[i].speed,setspd[i]);
			
			Set_motor_cmd(&hcan1,First_STDID,Motor_measure[0].Output ,Motor_measure[1].Output ,0,0);
			osDelay(10);
	}
}














