#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

/* 按键GPIO引脚设置，如需更换其它引脚，修改下面这几项的值即可 */
#define KEY_PORT			GPIOB
#define KEY_READ_Pin		GPIO_Pin_4
#define KEY_WRITE_Pin		GPIO_Pin_5
#define KEY_RCC				RCC_APB2Periph_GPIOB
#define KEY_READ			GPIO_ReadInputDataBit(KEY_PORT,KEY_READ_Pin)				
#define KEY_WRITE  			GPIO_ReadInputDataBit(KEY_PORT,KEY_WRITE_Pin)	

#define PRES_READ			1
#define PRES_WRITE			2

void KeyInit(void);
u8 KeyScan(unsigned char ucMode);

#endif
