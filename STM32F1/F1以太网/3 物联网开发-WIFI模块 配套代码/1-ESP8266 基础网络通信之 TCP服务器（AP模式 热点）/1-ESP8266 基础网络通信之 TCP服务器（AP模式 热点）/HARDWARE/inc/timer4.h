/*-------------------------------------------------*/
/*            ModuleStar STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            实现定时器4功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef _TIMER4_H
#define _TIMER4_H

extern char timer4_flag;   //外部变量声明，1:定时器4中断，有数据到

void TIM4_Init(unsigned short int, unsigned short int);

#endif
