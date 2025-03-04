/**
 * @file RMClient_UI.c
 * @author Miraggio (w1159904119@gmail)
 * @brief ����ϵͳ�ͻ����Զ���UI
 * @version 0.1
 * @date 2021-04-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "RMClient_UI.h"
#include "DR16_Remote.h"

#define Light_Nums 8
#define INIT_NUMS 9
#define UPDATA_NUMS 7


void(*Dispaly_Init[])(void) = DISPLAY_INIT
void(*Display_Updata[])(void) = DISPLAY_UPDATA
RMClient_UI_t RMClient_UI = {&Dispaly_Init, &Display_Updata};

int8_t Updata_TimeWait;
int8_t InitShow_Flag;

/**
  * @brief  UI ��ʾ
  * @param  None
  * @retval None
  */
void UserDefined_UI(void)
{
  // --- ��Ҫʵʱ���µ����� --- //
  Updata_TimeWait++;

  if (Updata_TimeWait >= 2)
  {

    for(int i = 0 ; i < UPDATA_NUMS ; i++)
    {
      Display_Updata[i]();
    }

    Updata_TimeWait = 0;
  }

  // --- ����ʵʱ���µ����� --- //
  if (InitShow_Flag > 5) // Ctrl ��⵽����η���
  {

    for (int i = 0; i < INIT_NUMS; i++)
    {
      Dispaly_Init[i]();
    }

    InitShow_Flag = 0;
  }

}
