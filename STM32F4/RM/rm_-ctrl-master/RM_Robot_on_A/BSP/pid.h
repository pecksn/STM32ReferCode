#ifndef __PID_H
#define __PID_H

#include <main.h>

typedef enum 
{
	PID_POSITION_SPEED = 0,
	PID_POSITION_ANGLE,
	PID_DELTA_SPEED
}PID_mode;

typedef struct
{
	PID_mode mode;			//ģʽ	λ��ʽ�ٶȣ�λ��ʽ�Ƕȣ�����ʽ�ٶ�
	float Kp;
	float Ki;
	float Kd;
	
	float Max_iout;		//�����������
	float Max_out;		//�������
	
	float measure;		//����ֵ
	float target;			//Ŀ��ֵ
	float DeadBand;		//����
	
	float Pout;
	float Iout;
	float Dout;
	float D_item;			//΢����	
	float error[3];		//ƫ��		0����
	float OUT;				//���
}PID_typedef;


/**
  * @brief          pid �ṹ�����ݳ�ʼ��
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      mode: PID_POSITION_SPEED: λ��ʽPID���ٶ�
  *                 			PID_POSITION_ANGLE: λ��ʽPID���Ƕ�
	*  											PID_DELTA_SPEED		�����ʽPID���ٶ�
  * @param[in]      kp:PID����p
	* @param[in] 			ki:PID����i
	* @param[in] 			kd:PID����d
  * @param[in]      Max_iout:pid���������
  * @param[in]      Max_out:pid������
	* @param[in]			deadband:PID����
  * @retval         none
  */
void PID_Init(PID_typedef *PID,PID_mode Mode,float kp,float ki,float kd,float Max_iout,float Max_out,float deadband);


/**
  * @brief          pid����
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      measure:������������
	* @param[in] 			target: Ŀ��ֵ
  * @retval         none
  */
float PID_calc(PID_typedef *PID, float measure, float target);


void PID_Total_Init(void);


#endif






