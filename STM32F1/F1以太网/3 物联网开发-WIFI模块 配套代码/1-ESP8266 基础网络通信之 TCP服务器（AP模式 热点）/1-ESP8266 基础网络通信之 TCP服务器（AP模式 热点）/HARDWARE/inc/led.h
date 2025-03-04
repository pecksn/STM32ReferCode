/*-------------------------------------------------*/
/*            ModuleStar STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��LED���ܵ�ͷ�ļ�                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LED_H
#define __LED_H

#define LED1_OUT(x)   GPIO_WriteBit(GPIOA, GPIO_Pin_5,  (BitAction)x)  //����PA5  �ĵ�ƽ�����Ե���Ϩ��LED1
#define LED2_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_1,  (BitAction)x)  //����PB1  �ĵ�ƽ�����Ե���Ϩ��LED2
#define LED3_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)x)  //����PB10 �ĵ�ƽ�����Ե���Ϩ��LED3
#define LED4_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)x)  //����PB11 �ĵ�ƽ�����Ե���Ϩ��LED4

#define LED1_IN_STA   GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5)  //PA5  ����LED1,��ȡ��ƽ״̬�������ж�LED1�ǵ�������Ϩ��
#define LED2_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1)  //PB1  ����LED2,��ȡ��ƽ״̬�������ж�LED2�ǵ�������Ϩ��
#define LED3_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10) //PB10 ����LED3,��ȡ��ƽ״̬�������ж�LED3�ǵ�������Ϩ��
#define LED4_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_11) //PB11 ����LED4,��ȡ��ƽ״̬�������ж�LED4�ǵ�������Ϩ��

#define LED1_ON       GPIO_ResetBits(GPIOA, GPIO_Pin_5)         //������������PA5��ƽ������LED1
#define LED1_OFF      GPIO_SetBits(GPIOA, GPIO_Pin_5)           //������������PA5��ƽ��Ϩ��LED1

#define LED2_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_1)         //������������PB1��ƽ������LED2
#define LED2_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_1)           //������������PB1��ƽ��Ϩ��LED2

#define LED3_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_10)        //������������PB10��ƽ������LED3
#define LED3_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_10)          //������������PB10��ƽ��Ϩ��LED3

#define LED4_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_11)        //������������PB11��ƽ������LED4
#define LED4_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_11)          //������������PB11��ƽ��Ϩ��LED4

void LED_Init(void);               //��ʼ��	
void LED_AllOn(void);              //��������LED
void LED_AllOff(void);             //Ϩ������LED

#endif
