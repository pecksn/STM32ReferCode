#ifndef _MOTORCONTROL_H_
#define _MOTORCONTROL_H_


#include "tim.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

//void Motor1_enable(void);
//void Motor2_enable(void);
//void Motor3_enable(void);
//void Motor4_enable(void);
//void Motor5_enable(void);
//void Motor6_enable(void);
//void Motor7_enable(void);
//void Motor8_enable(void);

//void Motor1_disable(void);
//void Motor2_disable(void);
//void Motor3_disable(void);
//void Motor4_disable(void);
//void Motor5_disable(void);
//void Motor6_disable(void);
//void Motor7_disable(void);
//void Motor8_disable(void);

void Motor1_course(char course_mod);
void Motor2_course(char course_mod);
void Motor3_course(char course_mod);
void Motor4_course(char course_mod);
void Motor5_course(char course_mod);
void Motor6_course(char course_mod);
void Motor7_course(char course_mod);
void Motor8_course(char course_mod);

void motor1_rise(void);
void motor2_rise(void);
void motor3_rise(void);
void motor4_rise(void);
void motor5_rise(void);
void motor6_rise(void);
void motor7_rise(void);
void motor8_rise(void);

void motor1_descent(void);
void motor2_descent(void);
void motor3_descent(void);
void motor4_descent(void);
void motor5_descent(void);
void motor6_descent(void);
void motor7_descent(void);
void motor8_descent(void);

void motor1_stop(void);
void motor2_stop(void);
void motor3_stop(void);
void motor4_stop(void);
void motor5_stop(void);
void motor6_stop(void);
void motor7_stop(void);
void motor8_stop(void);

//void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)；

#endif /* __MOTOR_H__ */








