/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define DISPLAY_FRAME_SIZE      							16U
#define DISPLAY_START_BYTE      							0x5AU
#define DISPLAY_END_BYTE        							0xEFU
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define BUFF_START_ADDR            								0U

#define BUFF_VOLTAGE_READ          								(BUFF_START_ADDR)                  //0
#define BUFF_CURRENT_READ          								(BUFF_VOLTAGE_READ + SIZE_FLOAT)   //4

#define BUFF_VOLTAGE_SET           								(BUFF_CURRENT_READ + SIZE_FLOAT)   //8
#define BUFF_CURRENT_SET           								(BUFF_VOLTAGE_SET + SIZE_FLOAT)    //12 - c

#define BUFF_STATUS_READ           								(BUFF_CURRENT_SET + SIZE_FLOAT)    //16 -10
#define BUFF_LED_STATUS            								(BUFF_STATUS_READ + SIZE_UINT8)    //17 -11

#define BUFF_SWITCH_STATE          								(BUFF_LED_STATUS + SIZE_UINT8)     //18 -12

#define COMM_BUFF_SIZE             								(BUFF_SWITCH_STATE + SIZE_UINT8)   //19  -13

#define	USART1_RX_SIZE										 	64

extern uint8_t usart1RxBuff[USART1_RX_SIZE];

typedef enum
{
	REMOTE = 0,
	LOCAL
}ControlMode;

extern float VoltageSet;
extern float CurrentSet;

extern float ReadVoltage;
extern float ReadCurrent;


extern uint16_t voltageValue;
extern uint16_t currentValue;
extern uint16_t setCurrentValue;

extern uint8_t testBuf[16];

extern uint32_t count1,count2,count3;
extern uint8_t state;
extern uint8_t Output_Status;
extern uint8_t ACF_Status,OTP_Status,FanFail_Status,Overload_Status;
extern uint8_t remLocStatus;

extern uint8_t mode;
extern uint8_t SystemStatus ;
extern uint8_t LedStatus;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void display_update();
void display_ISET();
void Fault_Display();

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

