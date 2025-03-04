/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TIM1_Prescaler 3
#define STEPMOTOR_TIM2_PERIOD 65535
#define STEPMOTOR_TIM3_PERIOD 65535
#define TIM3_Prescaler 3
#define TIM2_Prescaler 3
#define STEPMOTOR_TIM4_PERIOD 65535
#define TIM4_Prescaler 3
#define m1p_Pin GPIO_PIN_0
#define m1p_GPIO_Port GPIOA
#define m2p_Pin GPIO_PIN_1
#define m2p_GPIO_Port GPIOA
#define m3p_Pin GPIO_PIN_2
#define m3p_GPIO_Port GPIOA
#define m4p_Pin GPIO_PIN_3
#define m4p_GPIO_Port GPIOA
#define m5p_Pin GPIO_PIN_6
#define m5p_GPIO_Port GPIOA
#define m6p_Pin GPIO_PIN_7
#define m6p_GPIO_Port GPIOA
#define DE_485_Pin GPIO_PIN_4
#define DE_485_GPIO_Port GPIOC
#define RE_485_Pin GPIO_PIN_5
#define RE_485_GPIO_Port GPIOC
#define m7p_Pin GPIO_PIN_0
#define m7p_GPIO_Port GPIOB
#define m8p_Pin GPIO_PIN_1
#define m8p_GPIO_Port GPIOB
#define TX_485_Pin GPIO_PIN_10
#define TX_485_GPIO_Port GPIOB
#define RX_485_Pin GPIO_PIN_11
#define RX_485_GPIO_Port GPIOB
#define m9d_Pin GPIO_PIN_8
#define m9d_GPIO_Port GPIOD
#define m10d_Pin GPIO_PIN_9
#define m10d_GPIO_Port GPIOD
#define m11d_Pin GPIO_PIN_10
#define m11d_GPIO_Port GPIOD
#define m12d_Pin GPIO_PIN_11
#define m12d_GPIO_Port GPIOD
#define m9p_Pin GPIO_PIN_12
#define m9p_GPIO_Port GPIOD
#define m10p_Pin GPIO_PIN_13
#define m10p_GPIO_Port GPIOD
#define m11p_Pin GPIO_PIN_14
#define m11p_GPIO_Port GPIOD
#define m12p_Pin GPIO_PIN_15
#define m12p_GPIO_Port GPIOD
#define m5d_Pin GPIO_PIN_6
#define m5d_GPIO_Port GPIOC
#define m6d_Pin GPIO_PIN_7
#define m6d_GPIO_Port GPIOC
#define m7d_Pin GPIO_PIN_8
#define m7d_GPIO_Port GPIOC
#define m8d_Pin GPIO_PIN_9
#define m8d_GPIO_Port GPIOC
#define m1d_Pin GPIO_PIN_0
#define m1d_GPIO_Port GPIOD
#define m2d_Pin GPIO_PIN_1
#define m2d_GPIO_Port GPIOD
#define m3d_Pin GPIO_PIN_2
#define m3d_GPIO_Port GPIOD
#define m4d_Pin GPIO_PIN_3
#define m4d_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
