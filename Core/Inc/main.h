/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define PS_OK_Pin GPIO_PIN_13
#define PS_OK_GPIO_Port GPIOC
#define Set_Local_Remote_Pin GPIO_PIN_14
#define Set_Local_Remote_GPIO_Port GPIOC
#define OTP_Pin GPIO_PIN_15
#define OTP_GPIO_Port GPIOC
#define PS_ON_OFF_STATUS_Pin GPIO_PIN_3
#define PS_ON_OFF_STATUS_GPIO_Port GPIOC
#define FAN_PWM_Pin GPIO_PIN_0
#define FAN_PWM_GPIO_Port GPIOB
#define fan_read_1_Pin GPIO_PIN_12
#define fan_read_1_GPIO_Port GPIOB
#define fan_read_2_Pin GPIO_PIN_13
#define fan_read_2_GPIO_Port GPIOB
#define fan_read_3_Pin GPIO_PIN_14
#define fan_read_3_GPIO_Port GPIOB
#define fan_read_4_Pin GPIO_PIN_15
#define fan_read_4_GPIO_Port GPIOB
#define display_rem_status_Pin GPIO_PIN_7
#define display_rem_status_GPIO_Port GPIOC
#define display_on_off_Pin GPIO_PIN_8
#define display_on_off_GPIO_Port GPIOC
#define display_tx_Pin GPIO_PIN_9
#define display_tx_GPIO_Port GPIOA
#define display_rx_Pin GPIO_PIN_10
#define display_rx_GPIO_Port GPIOA
#define Local_on_off_Pin GPIO_PIN_12
#define Local_on_off_GPIO_Port GPIOA
#define Ref_select_Pin GPIO_PIN_15
#define Ref_select_GPIO_Port GPIOA
#define ADC_DOUT_Pin GPIO_PIN_11
#define ADC_DOUT_GPIO_Port GPIOC
#define ADC_DAC_SCK_Pin GPIO_PIN_12
#define ADC_DAC_SCK_GPIO_Port GPIOC
#define ADC_CS_Pin GPIO_PIN_2
#define ADC_CS_GPIO_Port GPIOD
#define ADC_DAC_IN_Pin GPIO_PIN_5
#define ADC_DAC_IN_GPIO_Port GPIOB
#define ADC_MCLK_Pin GPIO_PIN_6
#define ADC_MCLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
