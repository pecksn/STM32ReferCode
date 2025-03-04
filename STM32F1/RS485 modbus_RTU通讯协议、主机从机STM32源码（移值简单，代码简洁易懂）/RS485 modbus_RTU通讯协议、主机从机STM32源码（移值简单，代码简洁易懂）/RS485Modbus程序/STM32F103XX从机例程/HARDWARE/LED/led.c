
/* ͷ�ļ� */
#include "led.h"


/****************************************************************************************************
 * �������ƣ� void LED_Init(void)
 * ��ڲ�������
 * ����  ֵ����
 * ����˵������ʼ��LED�ƶ�Ӧ��GPIO����
 * 			 ����Ϊ���������IO��ת�ٶ�Ϊ50MHz
 *			 ʹ�ÿ⺯��ʵ�֣����Ӳ��ԭ��
 *			 ��led.h���о���Ķ���
 *			 ��Ӳ�����Ӳ�ͬ������led.h�и��ļ���
 ***************************************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_RCC | RCC_APB2Periph_AFIO, ENABLE);	//��LED�˿�ʱ�� �͸���ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 		//��PB.3.4 PA15 ������ͨIO�� ����Ҫע��

	GPIO_InitStructure.GPIO_Pin = LED_PIN;				//����LED����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//LED����ģʽ->�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//LED���ŷ�ת�ٶ�->50MHz
	GPIO_Init(LED_PROT, &GPIO_InitStructure);			//�����趨������ʼ��LED��Ӧ��GPIO
	GPIO_SetBits(LED_PROT,LED_PIN);						//�a�����Ϊ�� �ر�LED��
}

/****************************************************************************************************
 * �������ƣ� void LedPoint(u16 uiCycle)
 * ��ڲ�����u16 uiCycle
 * ����  ֵ����
 * ����˵����led������˸���������ڶ�ʱ���е��ã�uiCycle��ֵ = ��ʱ���жϵ�����
 ***************************************************************************************************/
void LedPoint(u16 uiCycle)
{
	static u16 uiCnt = 0;
	
	uiCnt ++;
	if (uiCnt == uiCycle)
	{
		uiCnt = 0;
		LED ^= 1;
	}
}
/* END */
 
