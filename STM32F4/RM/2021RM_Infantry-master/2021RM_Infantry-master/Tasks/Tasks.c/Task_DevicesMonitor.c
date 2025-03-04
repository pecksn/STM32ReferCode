/**
 * @file Task_DevicesMonitor.c
 * @author Miraggio (w1159904119@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-03-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Task_DevicesMonitor.h"
#include "DR16_Remote.h"
#include "OLED.h"
#include "Robot_Config.h"
#include "typedef.h"

/**
 * @brief �������
 * 
 * @param argument 
 * @return  
 */
WorldTime_RxTypedef Detect_FPS;
uint32_t DeTest_FPS;
uint8_t RoboInit_Complete = 0; //�����˳�ʼ����ɱ�־λ
void Detect(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(200); //ÿ200����ǿ�ƽ����ܿ���

    DevicesMonitor_FUN.DevicesInit();

    for (;;)
    {

        DevicesMonitor_FUN.DevicesMonitor_update();
#ifdef Enable_DeviceMonitor
        DevicesMonitor_FUN.DevicesMonitor_Alert();
#endif
        if (RoboInit_Complete == 0) //û��ʼ��֮ǰһֱι��
        {
            HAL_IWDG_Refresh(&hiwdg); //ι��
        }
        else
        {
            if (Robot_FPS == 500 && GetData_FPS == 1000 && DR16_Fun.DR16_DataCheck() == 0)
            {
                HAL_IWDG_Refresh(&hiwdg); //ι��
            }
        }

        Get_FPS(&Detect_FPS, &DeTest_FPS);
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
