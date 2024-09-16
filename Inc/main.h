/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stdint.h"
#include "string.h"
#include "key.h"
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
#define MOTOR_AIN1_Pin GPIO_PIN_1
#define MOTOR_AIN1_GPIO_Port GPIOA
#define K210_UART_TX_Pin GPIO_PIN_2
#define K210_UART_TX_GPIO_Port GPIOA
#define K210_UART_RX_Pin GPIO_PIN_3
#define K210_UART_RX_GPIO_Port GPIOA
#define MOTOR_AIN2_Pin GPIO_PIN_10
#define MOTOR_AIN2_GPIO_Port GPIOB
#define SERVO_TIM_CIRCLE_Pin GPIO_PIN_6
#define SERVO_TIM_CIRCLE_GPIO_Port GPIOC
#define SERVO_TIM_UP_Pin GPIO_PIN_7
#define SERVO_TIM_UP_GPIO_Port GPIOC
#define DWN_Pin GPIO_PIN_8
#define DWN_GPIO_Port GPIOC
#define SERVO_RUN_Pin GPIO_PIN_9
#define SERVO_RUN_GPIO_Port GPIOC
#define UP_Pin GPIO_PIN_8
#define UP_GPIO_Port GPIOA
#define ESP_TX_Pin GPIO_PIN_9
#define ESP_TX_GPIO_Port GPIOA
#define ESP_RX_Pin GPIO_PIN_10
#define ESP_RX_GPIO_Port GPIOA
#define LFT_Pin GPIO_PIN_12
#define LFT_GPIO_Port GPIOA
#define RHT_Pin GPIO_PIN_10
#define RHT_GPIO_Port GPIOC
#define MID_Pin GPIO_PIN_12
#define MID_GPIO_Port GPIOC
#define TFT_SCL_Pin GPIO_PIN_4
#define TFT_SCL_GPIO_Port GPIOB
#define TFT_SDA_Pin GPIO_PIN_5
#define TFT_SDA_GPIO_Port GPIOB
#define TFT_RES_Pin GPIO_PIN_6
#define TFT_RES_GPIO_Port GPIOB
#define TFT_DC_Pin GPIO_PIN_7
#define TFT_DC_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_8
#define TFT_CS_GPIO_Port GPIOB
#define TFT_BLK_Pin GPIO_PIN_9
#define TFT_BLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define IMAGE_X 320
#define IMAGE_Y 240

#define IMAGE_X_CENTRAL 320/2
#define IMAGE_Y_CENTRAL 240/2

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
