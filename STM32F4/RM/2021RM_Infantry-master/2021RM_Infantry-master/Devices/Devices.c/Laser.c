/**
 * @file Laser.c
 * @author Miraggio (w1159904119@gmail)
 * @brief Dji����Ӧ�ú����ӿ�
 * @version 0.1
 * @date 2021-04-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Laser.h"


//    @arg GPIO_PIN_RESET: to clear the port pin
//    @arg GPIO_PIN_SET: to set the port pin
/**
 * @brief ���⿪��
 * 
 * @param open 
 * @return  
 */
void LASER_Set(GPIO_PinState open) {

	HAL_GPIO_WritePin(LASER_GPIO_Port, LASER_Pin, open);

}
