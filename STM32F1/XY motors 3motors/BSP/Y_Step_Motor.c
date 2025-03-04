#include "Y_Step_motor.h"
#include "tim.h"
#include <math.h>

/* ˽�����Ͷ��� --------------------------------------------------------------*/
//�������ͨ��
typedef struct {
  uint16_t  Pulse_Pin ; 	// ��ʱ�������������
  uint32_t  Channel;		  // ��ʱ��ͨ��
  uint32_t  IT_CCx ;  		// ��ʱ��ͨ���ж�ʹ��λ
  uint32_t  Flag_CCx ;    // ��ʱ��SR�жϱ��λ
}Tim;

typedef struct{\
  uint16_t  Dir_Pin ;     //����������ű��
  GPIO_TypeDef *Dir_Port; //����������Ŷ˿�
}StepMotor;

typedef struct {
  IRQn_Type IRQn;         // �жϱ��
  uint8_t   Active_Level; // �ж����ŵ�ƽ
  uint16_t  Pin ; 	      // ���ź�
  GPIO_TypeDef *Port;
}Detect_PIN;              // ԭ��������

/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
TIM_HandleTypeDef htimx_STEPMOTOR;
speedRampData srd[4] ={\
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},   // �Ӽ������߱���
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},
 {STOP,MOTOR_DIR_CCW,0,0,0,0,0},
};
  /* ��ʱ�������������,��Ҫ�޸�ֱ����stepmotor.hͷ�ļ��޸ļ���*/
const Tim Timer[4]={\
{STEPMOTOR_TIM_PULSE_PIN_X, STEPMOTOR_TIM_CHANNEL1, STEPMOTOR_TIM_IT_CC1, STEPMOTOR_TIM_FLAG_CC1},//X��
{STEPMOTOR_TIM_PULSE_PIN_Y, STEPMOTOR_TIM_CHANNEL2, STEPMOTOR_TIM_IT_CC2, STEPMOTOR_TIM_FLAG_CC2},//Y��
{STEPMOTOR_TIM_PULSE_PIN_Z, STEPMOTOR_TIM_CHANNEL3, STEPMOTOR_TIM_IT_CC3, STEPMOTOR_TIM_FLAG_CC3},//Z��
{STEPMOTOR_TIM_PULSE_PIN_R, STEPMOTOR_TIM_CHANNEL4, STEPMOTOR_TIM_IT_CC4, STEPMOTOR_TIM_FLAG_CC4},//R��
};

  /* ԭ��������*/
const Detect_PIN Origin_Detect[4]={ \
{ORIGIN_X_EXTI_IRQn, ORIGIN_X_DOG_MSG, ORIGIN_X_DETECT_PIN, ORIGIN_X_DETECT_PORT,},//X��
{ORIGIN_Y_EXTI_IRQn, ORIGIN_Y_DOG_MSG, ORIGIN_Y_DETECT_PIN, ORIGIN_Y_DETECT_PORT,},//Y��
{ORIGIN_Z_EXTI_IRQn, ORIGIN_Z_DOG_MSG, ORIGIN_Z_DETECT_PIN, ORIGIN_Z_DETECT_PORT,},//Z��
{ORIGIN_R_EXTI_IRQn, ORIGIN_R_DOG_MSG, ORIGIN_R_DETECT_PIN, ORIGIN_R_DETECT_PORT,} //R��
};
  /* ��ת��λ�������*/
const Detect_PIN LimNeg_Detect[4]={\
{ LIMNEG_X_EXTI_IRQn,LIMNEG_X_LEVEL, LIMNEG_X_PIN, LIMNEG_X_PORT},
{ LIMNEG_Y_EXTI_IRQn,LIMNEG_Y_LEVEL, LIMNEG_Y_PIN, LIMNEG_Y_PORT},
{ LIMNEG_Z_EXTI_IRQn,LIMNEG_Z_LEVEL, LIMNEG_Z_PIN, LIMNEG_Z_PORT},
{ LIMNEG_R_EXTI_IRQn,LIMNEG_R_LEVEL, LIMNEG_R_PIN, LIMNEG_R_PORT},
};
  /* ��ת��λ�������*/
const Detect_PIN LimPos_Detect[4]={\
{ LIMPOS_X_EXTI_IRQn, LIMPOS_X_LEVEL, LIMPOS_X_PIN, LIMPOS_X_PORT,},
{ LIMPOS_Y_EXTI_IRQn, LIMPOS_Y_LEVEL, LIMPOS_Y_PIN, LIMPOS_Y_PORT,},
{ LIMPOS_Z_EXTI_IRQn, LIMPOS_Z_LEVEL, LIMPOS_Z_PIN, LIMPOS_Z_PORT,},
{ LIMPOS_R_EXTI_IRQn, LIMPOS_R_LEVEL, LIMPOS_R_PIN, LIMPOS_R_PORT,},
};

  /* ���������������*/
const StepMotor Stepmotor[4]={\
{STEPMOTOR_X_DIR_PIN,STEPMOTOR_X_DIR_PORT},
{STEPMOTOR_Y_DIR_PIN,STEPMOTOR_Y_DIR_PORT},
{STEPMOTOR_Z_DIR_PIN,STEPMOTOR_Z_DIR_PORT},
{STEPMOTOR_R_DIR_PIN,STEPMOTOR_R_DIR_PORT},
};
//__IO uint8_t  ZeroStep[4] = {IDLE,IDLE,IDLE,IDLE};	        //����ԭ��״̬��
__IO uint8_t  LimPosi[4]  = {FALSE,FALSE,FALSE,FALSE};      //��ת���ޱ�־λ  True:���Ｋ��λ��  False:δ���Ｋ��λ��
__IO uint8_t  LimNega[4]  = {FALSE,FALSE,FALSE,FALSE};      //��ת���ޱ�־λ
__IO uint8_t  MotionStatus[4]   = {STOP,STOP,STOP,STOP};        // �Ƿ����˶���0��ֹͣ��1���˶�
__IO uint8_t  DOG[4]         		= {FALSE,FALSE,FALSE,FALSE};		// �����ź�
__IO uint8_t  HomeCapture[4]    = {FALSE,FALSE,FALSE,FALSE};		// ԭ�㲶���־
__IO int8_t   OriginDir[4] = {MOTOR_DIR_CCW,MOTOR_DIR_CCW,MOTOR_DIR_CCW,MOTOR_DIR_CCW};		// ����ԭ�㷽�� Ĭ����ʱ��
__IO int32_t  step_position[4]  = {0,0,0,0};      // ��ǰλ��  ��λ:������
__IO int16_t  location[4]       = {0,0,0,0};      // ��ǰλ��  ��λ:����(mm)
__IO uint8_t  OriginFlag[4] = {0};                // ���ڱ��������ԭ�����
__IO  BeginState_Typedef  BeginState[4] = {NORMAL,NORMAL,NORMAL,NORMAL};// ������ʼλ��

/* ��չ���� ------------------------------------------------------------------*/
extern uint32_t set_speed;          // �ٶ� ��λΪ0.05rad/sec
extern uint32_t step_accel;         // ���ٶ� ��λΪ0.025rad/sec^2
extern uint32_t step_decel;         // ���ٶ� ��λΪ0.025rad/sec^2
extern uint32_t fastseek_Speed ;    // �ع��ٶ�
extern uint32_t slowseek_Speed ;    // �����ٶ�

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static void STEPMOTOR_GPIO_Init(void);
void StopMotor(uint8_t Axis);

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: STEPMOTOR���GPIO��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void STEPMOTOR_GPIO_Init()
{
	uint8_t i = 0;     
	GPIO_InitTypeDef GPIO_InitStruct; 
  
  /* �������������Ŷ˿�ʱ��ʹ�� */
	STEPMOTOR_PULSE_GPIO_CLK_ENABLE();
	STEPMOTOR_X_DIR_GPIO_CLK_ENABLE();
	STEPMOTOR_Y_DIR_GPIO_CLK_ENABLE();
	STEPMOTOR_Z_DIR_GPIO_CLK_ENABLE();
	STEPMOTOR_R_DIR_GPIO_CLK_ENABLE();
  

  for(i=0; i<4; i++)
  {
    /* ���������������������� */
    GPIO_InitStruct.Pin = Timer[i].Pulse_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(STEPMOTOR_TIM_PULSE_PORT, &GPIO_InitStruct);
    
    /* ���������������������� */
    STEPMOTOR_DIR_FORWARD(i);//Ĭ������Ϊ˳ʱ�뷽��
    GPIO_InitStruct.Pin = Stepmotor[i].Dir_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(Stepmotor[i].Dir_Port, &GPIO_InitStruct);

}
 }
/**
  * ��������: ���������������ʱ����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void STEPMOTOR_TIMx_Init()
{
	uint8_t i = 0;	                           
	TIM_ClockConfigTypeDef sClockSourceConfig;  // ��ʱ��ʱ��
	TIM_OC_InitTypeDef sConfigOC;               // ��ʱ��ͨ���Ƚ����
  
	STEPMOTOR_TIM_RCC_CLK_ENABLE();
  
	/* STEPMOTOR���GPIO��ʼ������ */
	STEPMOTOR_GPIO_Init();
  
	/* ��ʱ�������������� */
	htimx_STEPMOTOR.Instance = STEPMOTOR_TIMx;                      	// ��ʱ�����
	htimx_STEPMOTOR.Init.Prescaler = STEPMOTOR_TIM_PRESCALER;        	// ��ʱ��Ԥ��Ƶ��
	htimx_STEPMOTOR.Init.CounterMode = TIM_COUNTERMODE_UP;            // �����������ϼ���
	htimx_STEPMOTOR.Init.Period = STEPMOTOR_TIM_PERIOD;          		  // ��ʱ������
	htimx_STEPMOTOR.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;        // ʱ�ӷ�Ƶ
	HAL_TIM_Base_Init(&htimx_STEPMOTOR);

	/* ��ʱ��ʱ��Դ���� */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       	// ʹ���ڲ�ʱ��Դ
	HAL_TIM_ConfigClockSource(&htimx_STEPMOTOR, &sClockSourceConfig);

	/* ��ʱ���Ƚ�������� */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                // �Ƚ����ģʽ����ת���
	sConfigOC.Pulse = STEPMOTOR_TIM_PERIOD;              // ������
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;           // �������
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;           // ����ģʽ

	/* ʹ�ܱȽ����ͨ�� */
	for (i=0; i <4; i++)
	{
		HAL_TIM_OC_Stop_IT (&htimx_STEPMOTOR,Timer[i].Channel); 
    HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, Timer[i].Channel);
		TIM_CCxChannelCmd(STEPMOTOR_TIMx, Timer[i].Channel, TIM_CCx_DISABLE);
	}

	/* ���ö�ʱ���ж����ȼ���ʹ�� */
	HAL_NVIC_SetPriority(STEPMOTOR_TIMx_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(STEPMOTOR_TIMx_IRQn);
  
	/* Enable the main output */
	__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR);  
	HAL_TIM_Base_Start(&htimx_STEPMOTOR);// ʹ�ܶ�ʱ��
}
/**
  * ��������: ���λ���˶����˶������Ĳ���
  * �������: Axis    ���
  *           step   �ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��).
  *           accel  ���ٶ�,ʵ��ֵΪaccel*0.1*rad/sec^2
  *           decel  ���ٶ�,ʵ��ֵΪdecel*0.1*rad/sec^2
  *           speed  ����ٶ�,ʵ��ֵΪspeed*0.1*rad/sec
  * �� �� ֵ: ��
  * ˵    ��: �Ը����Ĳ����ƶ�����������ȼ��ٵ�����ٶȣ�Ȼ���ں���λ�ÿ�ʼ
  *           ������ֹͣ��ʹ�������˶�����Ϊָ���Ĳ���������Ӽ��ٽ׶κ̲ܶ���
  *           �ٶȺ������ǻ�û�ﵽ����ٶȾ�Ҫ��ʼ����
  */
void STEPMOTOR_AxisMoveRel(uint8_t Axis,int32_t step,uint32_t accel, uint32_t decel, uint32_t speed)
{  
	__IO uint16_t tim_count;//��ȡ��ʱ���ļ���ֵ���޷���32λ�������Ҹ��������ܻᱻ������޸�
  // �ﵽ����ٶ�ʱ�Ĳ���
	__IO uint32_t max_s_lim;
  // ����Ҫ��ʼ���ٵĲ������������û�дﵽ����ٶȣ�
	__IO uint32_t accel_lim;
  
	//�ڵ���˶�����һ�㲻���ټ�������,�����ڻ�ԭ�������
	if(MotionStatus[Axis] != STOP)	
	{
		return ;
	}
  
	/* �����趨 */
	if(step < 0) // ����Ϊ����
	{
		srd[Axis].dir = MOTOR_DIR_CCW; // ��ʱ�뷽����ת
		STEPMOTOR_DIR_REVERSAL(Axis);
		step = -step;   // ��ȡ��������ֵ
	}
	else
	{
		srd[Axis].dir = MOTOR_DIR_CW; // ˳ʱ�뷽����ת
		STEPMOTOR_DIR_FORWARD(Axis);
	}

	if(step == 1)    // ����Ϊ1
	{
		srd[Axis].accel_count = -1;   // ֻ�ƶ�һ��
		srd[Axis].run_state = DECEL;  // ����״̬.
		srd[Axis].step_delay = 1000;	// ����ʱ	
	}
	else if(step != 0)  // ���Ŀ���˶�������Ϊ0
	{
		// ���ǵĵ������ר��ָ���ֲ�����ϸ�ļ��㼰�Ƶ�����

		// ��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ��
		// min_delay = (alpha / tt)/ w
		srd[Axis].min_delay = (int32_t)(A_T_x10/speed);

		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�����accel��λΪ0.1rad/sec^2
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100
		srd[Axis].step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);//C0,��ʼ�ٶȵĶ�ʱ��ֵ
			

		/*����Ӽ�����Ҫ�Ĳ���*/
		// ������ٲ�֮��ﵽ����ٶȵ�����
		// max_s_lim = speed^2 / (2*alpha*accel)
		max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));
		// ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0
		// ��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ�
		if(max_s_lim == 0)
		{
		  max_s_lim = 1;
		}
			
		// ������ٲ�֮�����Ǳ��뿪ʼ����
		// n1 = (n1+n2)decel / (accel + decel)
		accel_lim = (uint32_t)(step*decel/(accel+decel));
		// ���Ǳ����������1�����ܲ��ܿ�ʼ����.
		if(accel_lim == 0)
		{
		  accel_lim = 1;
		}
		// ʹ�������������ǿ��Լ�������ٽ׶β���
		if(accel_lim <= max_s_lim)
		{
		  srd[Axis].decel_val = accel_lim - step;
		}
		else{
		  srd[Axis].decel_val = -(max_s_lim*accel/decel);
		}
		// ��ֻʣ��һ�����Ǳ������
		if(srd[Axis].decel_val == 0)
		{
		  srd[Axis].decel_val = -1;
		}

		// ���㿪ʼ����ʱ�Ĳ���
		srd[Axis].decel_start = step + srd[Axis].decel_val;

		// �������ٶȺ��������ǾͲ���Ҫ���м����˶�
		if(srd[Axis].step_delay <= srd[Axis].min_delay)
		{
			srd[Axis].step_delay = srd[Axis].min_delay;
			srd[Axis].run_state = RUN;
		}
		else
		{
			srd[Axis].run_state = ACCEL;
		}    
		// ��λ���ٶȼ���ֵ
		srd[Axis].accel_count = 0;
		
	}
	MotionStatus[Axis] = 1;
  
	tim_count=__HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
	__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,Timer[Axis].Channel,tim_count+srd[Axis].step_delay); // ���ö�ʱ���Ƚ�ֵ
  HAL_TIM_OC_Start_IT (&htimx_STEPMOTOR,Timer[Axis].Channel);
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, Timer[Axis].Channel, TIM_CCx_ENABLE);// ʹ�ܶ�ʱ��ͨ�� 
	
}
/** 
  * ��������: ����ԭ��
  * �������: fastseek_speed ԭ��ع��ٶ�
  *   		    slowseek_speed �����ٶ�
  *			      accel   ���ٶ�
  *			      decel   ���ٶ�
  * �� �� ֵ: ��
  * ˵    ��: ʵ��Ѱ��ԭ��,����ԭ��λ��ͣ����,�����źź�ԭ���ź�����λ���ص�
  *           �½��غ�������ģ��,��⵽�½��ر�ʾ���񵽽����ź�,��ʼ�Ե����ƶ�,
  *           �����ر�ʾ����ԭ��,����ֹͣ.
  */

void STEPMOTOR_AxisHome( uint8_t Axis, int32_t fastseek_speed,  uint32_t slowseek_speed,
                         uint32_t accel, uint32_t decel)
{
  if(srd[Axis].run_state != STOP)
    return ;
  if(slowseek_speed > fastseek_speed)//�ٶȲ������Ϸ��淶
    return ;
  OriginDir[Axis] = MOTOR_DIR_CCW;// �ع�ԭ��ķ���,Ĭ���Ƿ�ת���޵ķ���.
  /* ���жϵ�ǰ����λ�� */
  if(HAL_GPIO_ReadPin(LimNeg_Detect[Axis].Port, LimNeg_Detect[Axis].Pin) == LimNeg_Detect[Axis].Active_Level)  
  {
    BeginState[Axis] = NEGLIM;
    OriginDir[Axis] = MOTOR_DIR_CW;
  }
  else if(HAL_GPIO_ReadPin(LimPos_Detect[Axis].Port, LimPos_Detect[Axis].Pin) == LimPos_Detect[Axis].Active_Level)
  {
    BeginState[Axis] = NORMAL;
  }
  else if(HAL_GPIO_ReadPin(Origin_Detect[Axis].Port,Origin_Detect[Axis].Pin) == Origin_Detect[Axis].Active_Level)
  {
    BeginState[Axis] = INDOG;
  }
  else 
  {
    BeginState[Axis] = NORMAL;// ʣ������״̬�޷��ֱ����.ͳһ��NORMAL����
  }
  OriginFlag[Axis] = 1;
  /* �����ĸ�λ�õ�һ�����Ǽ������� */
  STEPMOTOR_AxisMoveRel(Axis,OriginDir[Axis]*MAX_NUM_LAP, accel, decel,fastseek_speed);
  /* ����������ٶȵıȽ�ֵ */
  srd[Axis].medle_delay = (int32_t)(A_T_x10/slowseek_speed);
}

/** ��������: �����ƶ�
  * �������: targert_step:Ŀ���λ��
  *			      accel:���ٶ�
  *			      decel:���ٶ�
  *			      speed:����ٶ�
  * �� �� ֵ: ��
  * ˵    ��: ʵ�ֶ����ƶ�,����Ŀ��λ�������ԭ��Ĳ���,
  *  		      �Լ��ٶȼ��ٵ�����ٶȺ�����Ѱ��Ŀ��λ��,
  *			      Ȼ���ں��ʵ�λ�ü���,����Ŀ��λ��.
  */
void STEPMOTOR_AxisMoveAbs(uint8_t Axis, int32_t targert_step,uint32_t accel, uint32_t decel, uint32_t speed)
{
  int32_t rel_step = 0;
	int8_t dir = -1;                           //��ԭ��ķ������
	rel_step = step_position[Axis]-targert_step ; 	//��ȡ��ǰλ�ú�Ŀ��λ��֮��Ĳ���ֵ
	
	if(rel_step == 0)	
	{
		dir = 0;
	}
	STEPMOTOR_AxisMoveRel(Axis,dir*rel_step,accel,decel,speed);
}
/** ��������: ����ƶ�һ������;��λ:mm
  * �������: distance  ��Ҫ�ƶ��ľ���,���������ַ���,��������������,
  *                    �������Ƿ�����
  *            accel  ���ٶ�
  *            decel  ���ٶ�
  *            speed  ����ٶ�
  * �� �� ֵ: ��
  * ˵    ��: �ӵ�ǰλ�����ڵ������ƶ�һ���ľ���,�Բ���distance���������ַ���
 */
void STEPMOTOR_DisMoveRel(__IO uint8_t Axis, __IO int16_t distance, __IO uint32_t accel, __IO uint32_t decel, __IO uint32_t speed)
{
  __IO int32_t step;
  step = distance*UNIT_STEP_MM;//��ò���Ŀ�����Ĳ���
  STEPMOTOR_AxisMoveRel(Axis,step,accel,decel,speed);
}
/**
  * ��������: �ƶ���Ŀ��λ��;��λ:mm
  * �������: Target_Dis  Ŀ������λ��
  *           accel  ���ٶ�
  *           decel  ���ٶ�
  *           speed  ����ٶ�
  * �� �� ֵ: ����ٶ�
  * ˵    ��: �ƶ�������������λ��,���ݵ�ǰ��λ�ü������Ŀ��λ�õľ���,
  *           ���ƶ���Ŀ��λ��,
  *             
  */
void STEPMOTOR_DisMoveAbs(uint8_t Axis,  uint16_t Target_Dis, uint32_t accel, uint32_t decel, uint32_t speed)
{
  int32_t step; 
    
  if(Target_Dis > MAX_DISTANCE)
    return;
  else if(Target_Dis < UNITS_DISTANCE)
    return;
  step = (Target_Dis - location[Axis])*UNIT_STEP_MM;     //��õ�ǰλ����Ŀ��λ�õľ���
  STEPMOTOR_AxisMoveRel(Axis,step,accel,decel,speed);
}

/**
  * ��������: ��ʱ���жϻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ʵ�ּӼ��ٹ���
  */

void HAL_TIM_OC_Callback(uint8_t Axis)
{
  __IO static uint16_t tim_count = 0;
  __IO  uint32_t new_step_delay = 0;
  __IO static uint8_t  i[4]={0,0,0,0};
	__IO static uint16_t last_accel_delay[4] = {0,0,0,0};
	// ���ƶ�����������
	__IO static uint32_t step_count[4] = {0,0,0,0};
	// ��¼new_step_delay�е������������һ������ľ���
	__IO static int32_t rest[4] = {0,0,0,0};
	//��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ����������

  // ���ñȽ�ֵ
  tim_count = __HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
  __HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,Timer[Axis].Channel,tim_count+srd[Axis].step_delay);

  i[Axis]++;       // ��ʱ���жϴ�������ֵ
  if(i[Axis] == 2) // 2�Σ�˵���Ѿ����һ����������
  {
    i[Axis] = 0;   // ���㶨ʱ���жϴ�������ֵ
    /* �ж��Ƿ��ڱ�Ե�˶� */
    if(srd[Axis].dir == MOTOR_DIR_CCW)
    {
      if(LimNega[Axis] == TRUE)		//��ʱ���� ������ת�ļ���λ��,ֹͣ�˶�
      {
        srd[Axis].run_state = STOP;
        MotionStatus[Axis] = STOP;
        LimPosi[Axis] = FALSE;    //��ת�ļ��ޱ�־λӦΪFALSE
      }
      else 
      {
        LimPosi[Axis] = FALSE;	  //����ʱ�뷽��,��������ת�ļ���λ��֮ǰ,��ת�ļ��ޱ�־λӦΪFALSE
      }
    }
    else 
    {
      if(LimPosi[Axis] == TRUE)		
      {
        srd[Axis].run_state = STOP;
        MotionStatus[Axis] = STOP;
        LimNega[Axis] = FALSE;    //��˳ʱ�뷽��,��������λ��,��ת���ޱ�־λӦΪFalse
      }
      else 
      {
        LimNega[Axis] = FALSE;  //��˳ʱ�뷽��,��������ת�ļ���λ��֮ǰ,��ת�ļ��ޱ�־λӦΪFALSE
      }
    }
    
    switch(srd[Axis].run_state) // �Ӽ������߽׶�
    {
      case STOP:
    
        // �ر�ͨ��
        TIM_CCxChannelCmd(STEPMOTOR_TIMx,Timer[Axis].Channel,TIM_CCx_DISABLE);       
        __HAL_TIM_CLEAR_FLAG(&htimx_STEPMOTOR, Timer[Axis].Flag_CCx);
        HAL_TIM_OC_Stop_IT (&htimx_STEPMOTOR,Timer[Axis].Channel);
      
        MotionStatus[Axis] = STOP;  		//  ���Ϊֹͣ״̬
        step_count[Axis] = 0;  // ���㲽��������
        rest[Axis] = 0;        // ������ֵ
        last_accel_delay[Axis] = 0;
        srd[Axis].accel_count = 0;
        srd[Axis].step_delay = 0;
        srd[Axis].min_delay = 0;
        break;
  
      case ACCEL:
        step_count[Axis]++;      		// ������1
        if(srd[Axis].dir == MOTOR_DIR_CW)
        {		  	
          step_position[Axis]++; 	  // ����λ�ü�1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]++;
        }
        else
        {
          step_position[Axis]--; 	  // ����λ�ü�1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]--;
        }
        srd[Axis].accel_count++; 	  // ���ټ���ֵ��1
        
        new_step_delay = srd[Axis].step_delay - (((2 *srd[Axis].step_delay) + rest[Axis])/(4 * srd[Axis].accel_count + 1));//������(��)һ����������(ʱ����)
        rest[Axis] = ((2 * srd[Axis].step_delay)+rest[Axis])%(4 * srd[Axis].accel_count + 1);// �����������´μ��㲹���������������
        
        if(step_count[Axis] >= srd[Axis].decel_start)		// ����Ƿ�Ӧ�ÿ�ʼ����
        {
          srd[Axis].accel_count = srd[Axis].decel_val; 	// ���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
          srd[Axis].run_state = DECEL;           	      // �¸����������ٽ׶�
        }
        else if(new_step_delay <= srd[Axis].min_delay)  // ����Ƿ񵽴�����������ٶ�
        {
          srd[Axis].accel_count = srd[Axis].decel_val; 	// ���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
          last_accel_delay[Axis] = new_step_delay; 	// ������ٹ��������һ����ʱ���������ڣ�
          new_step_delay = srd[Axis].min_delay;    	// ʹ��min_delay����Ӧ����ٶ�speed��
          rest[Axis] = 0;                          	// ������ֵ
          srd[Axis].run_state = RUN;               	// ����Ϊ��������״̬
        }	
        last_accel_delay[Axis] = new_step_delay; 	  // ������ٹ��������һ����ʱ���������ڣ�
        break;
        
      case RUN:
        step_count[Axis]++; 		 // ������1
        if(srd[Axis].dir==MOTOR_DIR_CW)
        {	  	
          step_position[Axis]++; // ����λ�ü�1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]++;
        }
        else
        {
          step_position[Axis]--; // ����λ�ü�1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]--;
        }
            
        new_step_delay = srd[Axis].min_delay;       // ʹ��min_delay����Ӧ����ٶ�speed��
            
        if(step_count[Axis] >= srd[Axis].decel_start)   // ��Ҫ��ʼ����
        {
          srd[Axis].accel_count = srd[Axis].decel_val;  // ���ٲ�����Ϊ���ټ���ֵ
          new_step_delay = last_accel_delay[Axis];// �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������)
          srd[Axis].run_state = DECEL;            // ״̬�ı�Ϊ����
        }
        break;
        
      case DECEL:
        step_count[Axis]++; 		 // ������1
        if(srd[Axis].dir == MOTOR_DIR_CW)
        {		  	
          step_position[Axis]++; // ����λ�ü�1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]++;
        }
        else
        {
          step_position[Axis]--; // ����λ�ü�1
          if( 0 == (step_position[Axis] % UNIT_STEP_MM )) 
            location[Axis]--;
        }
        srd[Axis].accel_count++;
        new_step_delay = srd[Axis].step_delay - (((2 * srd[Axis].step_delay) + rest[Axis])/(4 * srd[Axis].accel_count + 1)); //������(��)һ����������(ʱ����)
        rest[Axis] = ((2 * srd[Axis].step_delay)+rest[Axis])%(4 * srd[Axis].accel_count + 1);// �����������´μ��㲹���������������
        
        //����Ƿ�Ϊ���һ��
        if(srd[Axis].accel_count >= 0)
        {
          StopMotor(Axis);
        }
        break;
      case DECEL_MEDLE:
        new_step_delay = srd[Axis].step_delay - (((2 * srd[Axis].step_delay) + rest[Axis])/(4 * srd[Axis].accel_count + 1)); //������(��)һ����������(ʱ����)
					rest[Axis] = ((2 * srd[Axis].step_delay)+rest[Axis])%(4 * srd[Axis].accel_count + 1);// �����������´μ��㲹���������������
				  //����Ƿ�Ϊ���һ��
					if(new_step_delay >= srd[Axis].medle_delay )
					{
            srd[Axis].min_delay = srd[Axis].medle_delay;
            step_count[Axis] = 0;  // ���㲽��������
            rest[Axis] = 0;        // ������ֵ
            srd[Axis].run_state = RUN;              
					}
        break;

      default :break;
    }      
    if(new_step_delay>0xFFFF)
      new_step_delay=0xFFFF;
    srd[Axis].step_delay = new_step_delay; // Ϊ�¸�(�µ�)��ʱ(��������)��ֵ
  }
}
/**
  * ��������: ֹͣ���ת��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ֹͣ��ʱ���жϺ��������������Ӽ��ٵĲ�����
  *             
  */
void StopMotor(uint8_t Axis)
{
  // �ر�ͨ�����
  TIM_CCxChannelCmd(STEPMOTOR_TIMx,Timer[Axis].Channel,TIM_CCx_DISABLE);       
  srd[Axis].accel_count = 0;
  srd[Axis].step_delay  = 0;
  srd[Axis].min_delay   = 0;
  srd[Axis].medle_delay = 0;
  srd[Axis].run_state = STOP;    
  MotionStatus[Axis]  = STOP;	
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  int8_t Axis = -1;
  switch(GPIO_Pin)
  {
    /* �����������޶��� */
    case LIMPOS_X_PIN:
      StopMotor(AXIS_X);
      break;
    case LIMPOS_Y_PIN:
      StopMotor(AXIS_Y);
      break;
    case LIMPOS_Z_PIN:
      StopMotor(AXIS_Z);
      break;
    case LIMPOS_R_PIN:
      StopMotor(AXIS_R);
      break;
    
    case LIMNEG_X_PIN:  
      StopMotor(AXIS_X);
      LimNega[AXIS_X] = TRUE;
      Axis  = AXIS_X ;
      break;
    case LIMNEG_Y_PIN:  
      StopMotor(AXIS_Y);
      LimNega[AXIS_Y] = TRUE;
      Axis  = AXIS_Y ;
      break;
    case LIMNEG_Z_PIN:  
      StopMotor(AXIS_Z);
      LimNega[AXIS_Z] = TRUE;
    Axis  = AXIS_Z;
      break;
    case LIMNEG_R_PIN:  
      StopMotor(AXIS_R);
      LimNega[AXIS_R] = TRUE;
      Axis= AXIS_R;
      break;      
    default :
      switch(GPIO_Pin)
      {
        case ORIGIN_X_DETECT_PIN: Axis = AXIS_X;break;
        case ORIGIN_Y_DETECT_PIN: Axis = AXIS_Y;break;
        case ORIGIN_Z_DETECT_PIN: Axis = AXIS_Z;break;
        case ORIGIN_R_DETECT_PIN: Axis = AXIS_R;break;
      }
      break;
  }
  if(Axis == -1)
    return;
  if( OriginFlag[Axis] == 1)
  {
    /* ������ת��λ,˵�����ϻ�����ԭ����� */
    if(GPIO_Pin == LimNeg_Detect[Axis].Pin)
    {
      BeginState[Axis] = NEGLIM;
      StopMotor(Axis);
      /* ��������ԭ�� */
      STEPMOTOR_AxisHome(Axis,fastseek_Speed,slowseek_Speed,step_accel,step_decel);	
    }
    if(GPIO_Pin == Origin_Detect[Axis].Pin)
    {
      /* �½���ΪDOG�ź�ǰ�� */
      if(HAL_GPIO_ReadPin(Origin_Detect[Axis].Port,Origin_Detect[Axis].Pin) == ORIGIN_X_DOG_MSG)
      {
        // ������ٵ�����      
        srd[Axis].accel_count = srd[Axis].decel_val; 	// ���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
        srd[Axis].run_state = DECEL_MEDLE;
      }
      else
      {
        /* ���񵽽����ź�DOG���(ԭ��) */
        if(BeginState[Axis] == NEGLIM)//�������޺�������DOG��˵����
        {
          StopMotor(Axis);
          STEPMOTOR_AxisHome(Axis,fastseek_Speed,slowseek_Speed,step_accel,step_decel);
        }
        /* ��״̬2�𲽵�ʱ���������,ֱ�Ӽ��� */
        else  if(BeginState[Axis] == INDOG)
        {	
          srd[Axis].accel_count = srd[Axis].decel_val;// ���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
          srd[Axis].run_state = DECEL_MEDLE;
        }
        else
        {
          /* ��״̬1�𲽵�ʱ���������, */
          StopMotor(Axis);//��������»ص�ԭ��
          step_position[Axis] = 0;
          OriginFlag[Axis] = 0;
        }
      }
    }
  }
}

/*******************************END OF FILE*****************************************/
