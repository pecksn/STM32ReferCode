#include "led_flow_task.h"
#include "cmsis_os.h"
#include "main.h"


void led_RGB_flow_task(void const * argument)
{
	HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET);
	for(;;){
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
		osDelay(500);
	}
}



















