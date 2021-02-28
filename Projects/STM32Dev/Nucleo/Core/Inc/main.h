/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32l4xx_hal.h"

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
#define GPI_A03_1_Pin GPIO_PIN_3
#define GPI_A03_1_GPIO_Port GPIOA
#define GPI_A04_2_Pin GPIO_PIN_4
#define GPI_A04_2_GPIO_Port GPIOA
#define GPI_A05_3_Pin GPIO_PIN_5
#define GPI_A05_3_GPIO_Port GPIOA
#define GPO_B00_0_Pin GPIO_PIN_0
#define GPO_B00_0_GPIO_Port GPIOB
#define GPO_B01_1_Pin GPIO_PIN_1
#define GPO_B01_1_GPIO_Port GPIOB
#define GPI_A08_4_Pin GPIO_PIN_8
#define GPI_A08_4_GPIO_Port GPIOA
#define GPI_A11_5_Pin GPIO_PIN_11
#define GPI_A11_5_GPIO_Port GPIOA
#define GPI_A12_6_Pin GPIO_PIN_12
#define GPI_A12_6_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define GPO_B03_2_Pin GPIO_PIN_3
#define GPO_B03_2_GPIO_Port GPIOB
#define GPO_H03_5_Pin GPIO_PIN_3
#define GPO_H03_5_GPIO_Port GPIOH
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
