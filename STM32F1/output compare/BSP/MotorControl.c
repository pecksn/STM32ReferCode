#include "MotorControl.h"
#include "tim.h"
/*
1-6 为升降台 7-9为开关门
			ENA		PUL		DIR
Motor1		PF1		PA0		PD4
Motor2		PE5		PA1		PD5
Motor3		PE6		PA2		PD6
Motor4		PC2		PA3 	PD7

			ENA		PUL		DIR
Motor5		PC3		PA5 	PC6
Motor6		PC4		PA6 	PC7
Motor7		PC5		PB0		PC8
Motor8		PA5		PB1		PC9

*/
/* ============================== 步进电机使能/失能 ============================== */

//void Motor1_enable(void)			// 电机1使能
//{
//	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);
//}
//void Motor2_enable(void)			// 电机2使能
//{
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
//}
//void Motor3_enable(void)			// 电机3使能
//{
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
//}

//void Motor4_enable(void)			// 电机4使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
//}
//void Motor5_enable(void)			// 电机5使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
//}
//void Motor6_enable(void)			// 电机6使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
//}

//void Motor7_enable(void)			// 电机7使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
//}
//void Motor8_enable(void)			// 电机8使能
//{
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
//}

//void Motor1_disable(void)			// 电机1使能
//{
//	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);
//}
//void Motor2_disable(void)			// 电机2使能
//{
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
//}
//void Motor3_disable(void)			// 电机3使能
//{
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
//}

//void Motor4_disable(void)			// 电机4使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
//}
//void Motor5_disable(void)			// 电机5使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
//}
//void Motor6_disable(void)			// 电机6使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
//}

//void Motor7_disable(void)			// 电机7使能
//{
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
//}
//void Motor8_disable(void)			// 电机8使能
//{
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
//}

/* ============================== 步进电机方向控制,正转逆时针，反转顺时针 ============================== */

void Motor1_course(char course_mod)			// 电机1改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOD,m1d_Pin, GPIO_PIN_SET);}		// 正转
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOD, m1d_Pin, GPIO_PIN_RESET);}			// 反转
}
void Motor2_course(char course_mod)			// 电机2改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOD, m1d_Pin, GPIO_PIN_SET);}
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOD, m2d_Pin, GPIO_PIN_RESET);}
}
void Motor3_course(char course_mod)			// 电机3改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOD, m3d_Pin, GPIO_PIN_SET);}
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOD, m3d_Pin, GPIO_PIN_RESET);}
}

void Motor4_course(char course_mod)			// 电机4改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOD, m4d_Pin, GPIO_PIN_SET);}
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOD, m4d_Pin , GPIO_PIN_RESET);}
}
void Motor5_course(char course_mod)			// 电机5改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOC, m5d_Pin, GPIO_PIN_SET);}
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOC, m5d_Pin, GPIO_PIN_RESET);}
}
void Motor6_course(char course_mod)			// 电机6改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOC, m6d_Pin, GPIO_PIN_SET);}
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOC, m6d_Pin, GPIO_PIN_RESET);}
}

void Motor7_course(char course_mod)			// 电机7改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOC, m7d_Pin, GPIO_PIN_SET);}
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOC, m7d_Pin, GPIO_PIN_RESET);}
}
void Motor8_course(char course_mod)			// 电机8改变方向
{
	if(course_mod == 1){HAL_GPIO_WritePin(GPIOC, m8d_Pin, GPIO_PIN_SET);}
	else if(course_mod == 0){HAL_GPIO_WritePin(GPIOC, m8d_Pin, GPIO_PIN_RESET);}
}


/* ============================== 1-8电机升降函数 ============================== */
void motor1_rise(void){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	Motor1_course(1);
//	Motor1_enable();
}

void motor2_rise(void){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
	Motor2_course(1);
//	Motor2_enable();
}

void motor3_rise(void){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);
	Motor3_course(1);
//	Motor3_enable();
}

void motor4_rise(){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_4);
	Motor4_course(1);
//	Motor4_enable();
}

void motor5_rise(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
	Motor5_course(1);
//	Motor5_enable();
}

void motor6_rise(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
	Motor6_course(1);
//	Motor6_enable();
}

void motor7_rise(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_3);
	Motor7_course(1);
//	Motor7_enable();
}

void motor8_rise(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_4);
	Motor8_course(1);
//	Motor8_enable();
}


//下降
void motor1_descent(void){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	Motor1_course(0);
//	Motor1_enable();
}

void motor2_descent(void){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
	Motor2_course(0);
//	Motor2_enable();
}

void motor3_descent(void){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);
	Motor3_course(0);
//	Motor3_enable();
}

void motor4_descent(void){
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_4);
	Motor4_course(0);
//	Motor4_enable();
}

void motor5_descent(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
	Motor5_course(0);
//	Motor5_enable();
}

void motor6_descent(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
	Motor6_course(0);
//	Motor6_enable();
}

void motor7_descent(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_3);
	Motor7_course(0);
//	Motor7_enable();
}

void motor8_descent(void){
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_4);
	Motor8_course(0);
//	Motor8_enable();
}
/* ============================== 1-8电机停止函数 ============================== */
void motor1_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
}

void motor2_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
}

void motor3_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_3);
}

void motor4_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_4);
}

void motor5_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_1);
}

void motor6_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_2);
}

void motor7_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_3);
}

void motor8_stop(void){
	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_4);
}
