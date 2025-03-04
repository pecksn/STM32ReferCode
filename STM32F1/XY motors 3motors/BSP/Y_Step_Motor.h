#ifndef __Y_STEP_MOTOR_H
#define	__Y_STEP_MOTOR_H


/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
typedef struct {
  __IO uint8_t  run_state ;  	// �����ת״̬
  __IO int8_t   dir ;        	// �����ת����
  __IO uint32_t decel_start; 	// ��������λ��
  __IO int32_t  step_delay;  	// �¸��������ڣ�ʱ������������ʱΪ���ٶ�
  __IO int32_t  decel_val;   	// ���ٽ׶β���
  __IO int32_t  min_delay;   	// ��С��������(����ٶȣ������ٶ��ٶ�)
  __IO int32_t  accel_count; 	// �Ӽ��ٽ׶μ���ֵ
  __IO int32_t  medle_delay;
}speedRampData;

typedef enum{
  NORMAL,
  INDOG,
  NEGLIM,
}BeginState_Typedef;
/* �궨�� --------------------------------------------------------------------*/
#define STEPMOTOR_TIMx                        TIM2                //��ʱ���������
#define STEPMOTOR_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM2_CLK_ENABLE()
#define STEPMOTOR_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM2_CLK_DISABLE()

#define STEPMOTOR_TIMx_IRQn                   TIM2_IRQn			  //��ʱ������Ƚ��ж�
#define STEPMOTOR_TIMx_IRQHandler             TIM2_IRQHandler	//�жϷ�����

#define 	AXIS_X  0   //������
#define 	AXIS_Y  1
#define 	AXIS_Z  2
#define 	AXIS_R  3\

#define STEPMOTOR_PULSE_GPIO_CLK_ENABLE       __HAL_RCC_GPIOA_CLK_ENABLE
#define STEPMOTOR_TIM_PULSE_PORT              GPIOA               // TIM1����������� 4��ͨ����Ӧ��������GPIOA 

// X��������Ŷ���
#define STEPMOTOR_TIM_CHANNEL1                TIM_CHANNEL_1			  //��ʱ��ͨ��1//X��
#define STEPMOTOR_TIM_IT_CC1                  TIM_IT_CC1			    //��ʱ��ͨ��1�ж�ʹ��λ
#define STEPMOTOR_TIM_FLAG_CC1                TIM_FLAG_CC1			  //��ʱ��ͨ��1�жϱ�־λ
#define STEPMOTOR_TIM_PULSE_PIN_X             m1p_Pin          //��������X����������

#define STEPMOTOR_X_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()  // �����ת������ƣ�������ղ���Ĭ����ת
#define STEPMOTOR_X_DIR_PORT                  GPIOD                         // ��ӦSTEPMOTOR��DIR-��������ʹ�ù����ӷ���
#define STEPMOTOR_X_DIR_PIN                   m1d_Pin                      // ��DIR+ֱ�ӽӿ������+5V(��3.3V)

#define ORIGIN_X_DETECT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOG_CLK_ENABLE()	//X��ԭ��������
#define ORIGIN_X_DETECT_PORT					GPIOG			          					
#define ORIGIN_X_DETECT_PIN					    GPIO_PIN_0
#define GPIO_SET_AF_NORMAL                    GPIO_AF0_TRACE
#define ORIGIN_X_EXTI_IRQn            	      EXTI0_IRQn                     
#define ORIGIN_X_DOG_MSG					  GPIO_PIN_RESET                //��⵽�����ź�ʱ���ŵ�ƽ

#define LIMPOS_X_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת����λ��������
#define LIMPOS_X_PORT                   	  GPIOG
#define LIMPOS_X_PIN                    	  GPIO_PIN_1
#define GPIO_SET_AF_NORMAL                    GPIO_AF0_TRACE
#define LIMPOS_X_EXTI_IRQn					  EXTI1_IRQn
#define LIMPOS_X_LEVEL						  GPIO_PIN_RESET                //��ת����������Ч��ƽ

#define LIMNEG_X_GPIO_CLK_ENABLE()      	  __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת��λ��������
#define LIMNEG_X_PORT                   	  GPIOG       
#define LIMNEG_X_PIN                    	  GPIO_PIN_2  
#define GPIO_SET_AF_NORMAL                    GPIO_AF0_TRACE
#define LIMNEG_X_EXTI_IRQn					  EXTI2_IRQn
#define LIMNEG_X_LEVEL						  GPIO_PIN_RESET                //��ת����������Ч��ƽ

//Y��������Ŷ���
#define STEPMOTOR_TIM_CHANNEL2                TIM_CHANNEL_2			            // ��ʱ��ͨ��2//Y��
#define STEPMOTOR_TIM_IT_CC2                  TIM_IT_CC2			              // ��ʱ��ͨ��2�ж�ʹ��λ
#define STEPMOTOR_TIM_FLAG_CC2                TIM_FLAG_CC2                  // ��ʱ��ͨ��1�жϱ�־λ
#define STEPMOTOR_TIM_PULSE_PIN_Y             m2p_Pin                    // ��������Y����������

#define STEPMOTOR_Y_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()     // �����ת������ƣ�������ղ���Ĭ����ת
#define STEPMOTOR_Y_DIR_PORT                  GPIOD                            // ��ӦSTEPMOTOR��DIR+��������ʹ�ù����ӷ���
#define STEPMOTOR_Y_DIR_PIN                   m2d_Pin                     // ��DIR-ֱ�ӽӿ������GND

#define ORIGIN_Y_DETECT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOG_CLK_ENABLE()	
#define ORIGIN_Y_DETECT_PORT					        GPIOG								          
#define ORIGIN_Y_DETECT_PIN					          GPIO_PIN_3                    //Y��ԭ���⹦����������
#define ORIGIN_Y_EXTI_IRQn            	      EXTI3_IRQn                    
#define ORIGIN_Y_DOG_MSG						          GPIO_PIN_RESET                //�����ź����ŵ�ƽ

#define LIMPOS_Y_GPIO_CLK_ENABLE()      		  __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת����λ��������
#define LIMPOS_Y_PORT                   		  GPIOG       
#define LIMPOS_Y_PIN                    		  GPIO_PIN_4
#define LIMPOS_Y_EXTI_IRQn						        EXTI4_IRQn
#define LIMPOS_Y_LEVEL						            GPIO_PIN_RESET                //��ת����������Ч��ƽ

#define LIMNEG_Y_GPIO_CLK_ENABLE()      		  __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת��λ��������
#define LIMNEG_Y_PORT                   		  GPIOG       
#define LIMNEG_Y_PIN                    		  GPIO_PIN_5  
#define LIMNEG_Y_EXTI_IRQn						        EXTI9_5_IRQn
#define LIMNEG_Y_LEVEL						            GPIO_PIN_RESET                   //��������������Ч��ƽ

// Z����������Ŷ���
#define STEPMOTOR_TIM_CHANNEL3                TIM_CHANNEL_3			            //��ʱ��ͨ��3//Z��
#define STEPMOTOR_TIM_IT_CC3                  TIM_IT_CC3			
#define STEPMOTOR_TIM_FLAG_CC3                TIM_FLAG_CC3
#define STEPMOTOR_TIM_PULSE_PIN_Z              m3p_Pin                   //��������Z����������

#define STEPMOTOR_Z_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()     // �����ת������ƣ�������ղ���Ĭ����ת
#define STEPMOTOR_Z_DIR_PORT                  GPIOD                            // ��ӦSTEPMOTOR��DIR+��������ʹ�ù����ӷ���
#define STEPMOTOR_Z_DIR_PIN                   m3d_Pin                      // ��DIR-ֱ�ӽӿ������GND

#define ORIGIN_Z_DETECT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOG_CLK_ENABLE()	
#define ORIGIN_Z_DETECT_PORT					        GPIOG								
#define ORIGIN_Z_DETECT_PIN					          GPIO_PIN_6
#define ORIGIN_Z_EXTI_IRQn            	      EXTI9_5_IRQn
#define ORIGIN_Z_DOG_MSG						          GPIO_PIN_RESET

#define LIMPOS_Z_GPIO_CLK_ENABLE()      		  __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת����λ��������
#define LIMPOS_Z_PORT                   		  GPIOG       
#define LIMPOS_Z_PIN                    		  GPIO_PIN_7
#define LIMPOS_Z_EXTI_IRQn						        EXTI9_5_IRQn
#define LIMPOS_Z_LEVEL						            GPIO_PIN_RESET                //��ת����������Ч��ƽ

#define LIMNEG_Z_GPIO_CLK_ENABLE()      		  __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת��λ��������
#define LIMNEG_Z_PORT                   		  GPIOG       
#define LIMNEG_Z_PIN                    		  GPIO_PIN_8  
#define LIMNEG_Z_EXTI_IRQn						        EXTI9_5_IRQn
#define LIMNEG_Z_LEVEL						            GPIO_PIN_RESET                  //��������������Ч��ƽ

// R����ض���
#define STEPMOTOR_TIM_CHANNEL4                TIM_CHANNEL_4			            //��ʱ��ͨ��4//R��
#define STEPMOTOR_TIM_IT_CC4                  TIM_IT_CC4	
#define STEPMOTOR_TIM_FLAG_CC4                TIM_FLAG_CC4
#define STEPMOTOR_TIM_PULSE_PIN_R             m4p_Pin                   //��������R����������

#define STEPMOTOR_R_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()     // �����ת������ƣ�������ղ���Ĭ����ת
#define STEPMOTOR_R_DIR_PORT                  GPIOD                            // ��ӦSTEPMOTOR��DIR+��������ʹ�ù����ӷ���
#define STEPMOTOR_R_DIR_PIN                   m4d_Pin                      // ��DIR-ֱ�ӽӿ������GND

#define ORIGIN_R_DETECT_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOG_CLK_ENABLE()	
#define ORIGIN_R_DETECT_PORT					        GPIOG								
#define ORIGIN_R_DETECT_PIN					          GPIO_PIN_9
#define ORIGIN_R_EXTI_IRQn            	      EXTI9_5_IRQn
#define ORIGIN_R_DOG_MSG						          GPIO_PIN_RESET

#define LIMPOS_R_GPIO_CLK_ENABLE()      		  __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת����λ��������
#define LIMPOS_R_PORT                   		  GPIOG       
#define LIMPOS_R_PIN                    		  GPIO_PIN_10    
#define LIMPOS_R_EXTI_IRQn						        EXTI15_10_IRQn
#define LIMPOS_R_LEVEL						            GPIO_PIN_RESET                //��ת����������Ч��ƽ

#define LIMNEG_R_GPIO_CLK_ENABLE()      		  __HAL_RCC_GPIOG_CLK_ENABLE()  // ��ת��λ��������
#define LIMNEG_R_PORT                   		  GPIOG       
#define LIMNEG_R_PIN                    		  GPIO_PIN_15 
#define LIMNEG_R_EXTI_IRQn						        EXTI15_10_IRQn
#define LIMNEG_R_LEVEL						            GPIO_PIN_RESET                   //��������������Ч��ƽ

#define STEPMOTOR_DIR_FORWARD(Axis) \
HAL_GPIO_WritePin(Stepmotor[Axis].Dir_Port,Stepmotor[Axis].Dir_Pin,GPIO_PIN_SET)//���õ������,����Axis:��ǰ���
#define STEPMOTOR_DIR_REVERSAL(Axis)\
HAL_GPIO_WritePin(Stepmotor[Axis].Dir_Port,Stepmotor[Axis].Dir_Pin,GPIO_PIN_RESET)


// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/��STEPMOTOR_TIMx_PRESCALER+1��
#define STEPMOTOR_TIM_PRESCALER                3  // �������������ϸ������Ϊ��   32  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               7  // �������������ϸ������Ϊ��   16  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               15  // �������������ϸ������Ϊ��   8  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               31  // �������������ϸ������Ϊ��   4  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               63  // �������������ϸ������Ϊ��   2  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               127  // �������������ϸ������Ϊ��  1  ϸ��

// ���嶨ʱ�����ڣ�����Ƚ�ģʽ��������Ϊ0xFFFF
#define STEPMOTOR_TIM_PERIOD                  0xFFFF

#define FALSE                                 0
#define TRUE                                  1
#define MOTOR_DIR_CW                          1  // ˳ʱ��:������
#define MOTOR_DIR_CCW                         -1 // ��ʱ��:������

#define ORIGIN_POSITION	                      0  // ԭ������
#define STOP                                  0  // �Ӽ�������״̬��ֹͣ
#define ACCEL                                 1  // �Ӽ�������״̬�����ٽ׶�
#define DECEL                                 2  // �Ӽ�������״̬�����ٽ׶�
#define RUN                                   3  // �Ӽ�������״̬�����ٽ׶�
#define DECEL_MEDLE                           4  // ���ٵ������ٶ�


#define T1_FREQ                               (SystemCoreClock/(STEPMOTOR_TIM_PRESCALER+1)) // Ƶ��ftֵ
#define FSPR                                  200// ���������Ȧ����  �����:1.8�� 360/1.8 = 200 �����������Ҫ200��תһȦ
#define MICRO_STEP                            32 // �������������ϸ����
#define SPR                                   (FSPR*MICRO_STEP)   // ��תһȦ��Ҫ��������

// ��ѧ����
#define ALPHA                                 ((float)(2*3.14159/SPR))       // ��= 2*pi/spr�����
#define A_T_x10                               ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148                           ((float)((T1_FREQ*0.676)/10))  // 0.676Ϊ�������ֵ
#define A_SQ                                  ((float)(2*100000*ALPHA)) 
#define A_x200                                ((float)(200*ALPHA))
#define MAX_NUM_LAP 						              INT32_MAX
#define MAX_NUM_STEP 						              UINT32_MAX

#define UNIT_STEP_MM                          (SPR/UNITS_DISTANCE)//����1mm��Ҫ�Ĳ���
#define MAX_STEP_MM                           (MAX_DISTANCE/UNITS_DISTANCE)*UNIT_STEP_MM //
#define UNITS_DISTANCE                        5   //�������תһȦ,����ǰ��5mm
#define MAX_DISTANCE                          400 //��������ƶ��������400mm

#define  FASTSEEK_SPEED   300		//ԭ��ع��ٶ�
#define  SLOWSEEK_SPEED   100		//ԭ��ع������ٶ�

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx_STEPMOTOR;
/* �������� ------------------------------------------------------------------*/
void HAL_TIM_OC_Callback(uint8_t Axis);
void STEPMOTOR_TIMx_Init( void );
void STEPMOTOR_AxisMoveRel( uint8_t Axis, int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void STEPMOTOR_AxisMoveAbs( uint8_t Axis, int32_t targert_step, uint32_t accel, uint32_t decel, uint32_t speed);

void STEPMOTOR_DisMoveRel( uint8_t Axis, int16_t distance, uint32_t accel, uint32_t decel, uint32_t speed);
void STEPMOTOR_DisMoveAbs( uint8_t Axis, uint16_t Target_Dis, uint32_t accel, uint32_t decel, uint32_t speed);

void STEPMOTOR_AxisHome( uint8_t Axis, int32_t fastseek_speed,  uint32_t slowseek_speed,
                         uint32_t accel, uint32_t decel);
#endif	/* __Y_STEP_MOTOR_H */
/****************************END OF FILE****************************/

