/**
 * @file PVD.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-05-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "PVD.h"

PWR_PVDTypeDef pvd_config; //������

void PVD_Config(void) //��ʼ��
{
    __HAL_RCC_PWR_CLK_ENABLE(); //��ʼ��ʱ��
    pvd_config.PVDLevel = PWR_PVDLEVEL_7;
    pvd_config.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
    HAL_PWR_ConfigPVD(&pvd_config);
    HAL_PWR_EnablePVD(); //ʹ��

    HAL_NVIC_EnableIRQ(PVD_IRQn);         //ʹ��pvd�ж�ͨ��
    HAL_NVIC_SetPriority(PVD_IRQn, 0, 0); //��ռ���ȼ�0�������ȼ�0
}

void HAL_PWR_PVDCallback(void) //�жϻص�����
{
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
}
