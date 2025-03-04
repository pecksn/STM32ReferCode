/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ѧϰС��
  * ��    ��: ���Ჽ������������
  ******************************************************************************
  * ˵����
	      ����STM32��ʱ����̿�����·������������ö�ʱ����4��ͨ���ֱ������·����
				����Ŀ�ʼ��ֹͣ������״̬����������ÿһͨ�����ٶȡ�����ʱ��ȵ�Ҫ�أ���
				���������T�ͼӼ���������ֹͣ���ȶ�����
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "STEPMOTOR.h" 
#include "key.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
#define  FASTSEEK_SPEED   300		//ԭ��ع��ٶ�
#define  SLOWSEEK_SPEED   100		//ԭ��ع������ٶ�
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
// �ٶ����ֵ���������͵����������Щ�����1800����Щ���Դﵽ4000
uint32_t set_speed  = 500;         // �ٶ� ��λΪ0.05rad/sec
// ���ٶȺͼ��ٶ�ѡȡһ�����ʵ����Ҫ��ֵԽ���ٶȱ仯Խ�죬�Ӽ��ٽ׶αȽ϶���
// ���Լ��ٶȺͼ��ٶ�ֵһ������ʵ��Ӧ���жೢ�Գ����Ľ��
uint32_t step_accel = 150;         // ���ٶ� ��λΪ0.1rad/sec^2
uint32_t step_decel = 50;          // ���ٶ� ��λΪ0.1rad/sec^2

uint32_t fastseek_Speed = FASTSEEK_SPEED;
uint32_t slowseek_Speed = SLOWSEEK_SPEED;

/* ��չ���� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     // ʹ��PWRʱ��

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  // ���õ�ѹ�������ѹ����1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // �ⲿ����8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        // ��HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    // ��PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            // PLLʱ��Դѡ��HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 // 8��ƵMHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               // 336��Ƶ
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     // 2��Ƶ���õ�168MHz��ʱ��
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 // USB/SDIO/������������ȵ���PLL��Ƶϵ��
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // ϵͳʱ�ӣ�168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHBʱ�ӣ� 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1ʱ�ӣ�42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2ʱ�ӣ�84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // ʹ��CSS���ܣ�����ʹ���ⲿ�����ڲ�ʱ��ԴΪ����
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms�ж�һ��
	// HAL_RCC_GetHCLKFreq()/100000	 10us�ж�һ��
	// HAL_RCC_GetHCLKFreq()/1000000 1us�ж�һ��
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                 // ���ò�����ϵͳ�δ�ʱ��
  /* ϵͳ�δ�ʱ��ʱ��Դ */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* ϵͳ�δ�ʱ���ж����ȼ����� */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{
  /* ��λ�������裬��ʼ��Flash�ӿں�ϵͳ�δ�ʱ�� */
  HAL_Init();
  /* ����ϵͳʱ�� */
  SystemClock_Config();
  /* ����GPIO��Ϊ����ʹ�� */
  KEY_GPIO_Init();
  /* ���ö�ʱ��������� */
  STEPMOTOR_TIMx_Init();
  
  /* ����ѭ�� */
  while (1)
	{
//		STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����X��ԭ��
//    STEPMOTOR_AxisHome(AXIS_Y,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����Y��ԭ��
//    STEPMOTOR_AxisHome(AXIS_Z,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
//    STEPMOTOR_AxisHome(AXIS_R,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
    
		if(KEY1_StateRead() == KEY_DOWN)//��������״̬���ƣ��γɿ��Ʊջ����ɿ�ʼ�㡢�����㡢ԭ�㲿λ�Ĵ�����������Ϣʵ�ֱջ����ơ�
		{
      STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����X��ԭ��
      STEPMOTOR_AxisHome(AXIS_Y,2* FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����Y��ԭ��
      STEPMOTOR_AxisHome(AXIS_Z,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
      STEPMOTOR_AxisHome(AXIS_R,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);
		}
		if(KEY2_StateRead() == KEY_DOWN)
		{ //�������������ʽ���Ʋ���������е�ָ��λ�ã��������ƣ�ʵ�ֵ����������
      STEPMOTOR_DisMoveAbs(AXIS_X,100,step_accel,step_decel,set_speed);//X���ƶ���400mm��λ��
      STEPMOTOR_DisMoveAbs(AXIS_Y,200,step_accel,step_decel,set_speed);//Y���ƶ���400mm��λ��
      STEPMOTOR_DisMoveAbs(AXIS_Z,100,step_accel,step_decel,set_speed);//Z���ƶ���400mm��λ��
      STEPMOTOR_DisMoveAbs(AXIS_R,100,step_accel,step_decel,set_speed);//R���ƶ���400mm��λ��
		}
    if(KEY3_StateRead() == KEY_DOWN)
    { //��GPIO���̿�����������DIR����ʵ�����з������
      STEPMOTOR_AxisMoveRel(AXIS_X,30*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // X�ᷴ���ƶ�30Ȧ
      STEPMOTOR_AxisMoveRel(AXIS_Y,60*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // Y�ᷴ���ƶ�30Ȧ
      STEPMOTOR_AxisMoveRel(AXIS_Z,30*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // Z�ᷴ���ƶ�30Ȧ
      STEPMOTOR_AxisMoveRel(AXIS_R,30*SPR*MOTOR_DIR_CCW,step_accel,step_decel,set_speed); // R�ᷴ���ƶ�30Ȧ
    }
		
    if(KEY4_StateRead() == KEY_DOWN)
    {
      STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����X��ԭ��
    }		
		
    if(KEY5_StateRead() == KEY_DOWN)
    {
      STEPMOTOR_AxisHome(AXIS_Y,2* FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����Y��ԭ��
    }		
		
		
  }
}


/*******************************END OF FILE*****************************************/

