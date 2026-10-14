/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include <stdio.h>

/* USER CODE BEGIN 0 */

uint8_t usart1RxBuff[USART1_RX_SIZE] = {0};

float VoltageSet  = 0.0f;
float CurrentSet  = 0.0f;

float ReadVoltage = 0.0f;
float ReadCurrent = 0.0f;


uint16_t voltageValue;
uint16_t currentValue;
uint16_t setCurrentValue;


// ----------------Variables---------------------------
uint32_t count1,count2,count3;
uint8_t state=0;
uint8_t Output_Status=0;
uint8_t ACF_Status,OTP_Status,FanFail_Status,Overload_Status;
uint8_t remLocStatus;

uint8_t mode = REMOTE;
uint8_t SystemStatus = 0;
uint8_t LedStatus = 0;


//-----------------------------------------------------


uint8_t testBuf[16] =
{
    0x5A,               // Start byte

    '8', '0', '0', '0', // Read Voltage  = "4850"

    '1', '8', '5', '1', // Read Current  = "1234"

    '1', '8', '5', '0', // Set Current   = "1000"

    0x01,               // LED Status
    0x00,               // Fault Status

    0xEF                // End byte
};




/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void Fault_Display()
{
	testBuf[0] = 0x5A;
	if (ACF_Status == 1U)
	{
	testBuf[1] = 'A';
	testBuf[2] = 'C';
	testBuf[3] = 'F';
	testBuf[4] = ' ';

	testBuf[5] = ' ';
	testBuf[6] = ' ';
	testBuf[7] = ' ';
	testBuf[8] = ' ';
	}
	else if (OTP_Status == 1U)
	{
	testBuf[1] = 'O';
	testBuf[2] = 't';
	testBuf[3] = 'P';
	testBuf[4] = ' ';

	testBuf[5] = ' ';
	testBuf[6] = ' ';
	testBuf[7] = ' ';
	testBuf[8] = ' ';
	}
	else if (FanFail_Status == 1U)
	{
	testBuf[1] = 'F';
	testBuf[2] = 'A';
	testBuf[3] = 'n';
	testBuf[4] = ' ';

	testBuf[5] = 'F';
	testBuf[6] = 'A';
	testBuf[7] = 'I';
	testBuf[8] = 'L';
	}


	HAL_UART_Transmit(
		            &huart1,
		            (const uint8_t *)testBuf,
		            16,
		            1000
		        );


}
void display_ISET()
{
	char temp[5];

	testBuf[0] = 0x5A;
	testBuf[1] = '1';
	testBuf[2] = 'S';
	testBuf[3] = 'E';
	testBuf[4] = 't';

	 setCurrentValue =
	         (uint16_t)((CurrentSet * 10.0f));

	snprintf(temp, sizeof(temp), "%04u", setCurrentValue);

	testBuf[5] = temp[0];
	testBuf[6] = temp[1];
	testBuf[7] = temp[2];
	testBuf[8] = temp[3];

	/*---------------------------------------------
	         * BYTE 9-12 : Set Current
	         *--------------------------------------------*/
	        snprintf(temp, sizeof(temp), "%04u", setCurrentValue);

	        testBuf[9]  = temp[0];
	        testBuf[10] = temp[1];
	        testBuf[11] = temp[2];
	        testBuf[12] = temp[3];


	        /*---------------------------------------------
	         * BYTE 13 : LED Status
	         *--------------------------------------------*/


	        testBuf[13] = LedStatus;


	        /*---------------------------------------------
	         * BYTE 14 : Fault Status
	         *--------------------------------------------*/
	        testBuf[14] = SystemStatus;


	        /*---------------------------------------------
	         * BYTE 15 : End
	         *--------------------------------------------*/
	        testBuf[15] = 0xEF;


	        /*---------------------------------------------
	         * Send complete frame
	         *--------------------------------------------*/
	        HAL_UART_Transmit(
	            &huart1,
	            (const uint8_t *)testBuf,
	            16,
	            1000
	        );


}

void display_update()
{
	char temp[5];

    voltageValue =
         (uint16_t)((ReadVoltage * 100.0f));

     currentValue =
         (uint16_t)((ReadCurrent * 10.0f));

     setCurrentValue =
         (uint16_t)((CurrentSet * 10.0f));

//-----------------------------------------------------------
     /*---------------------------------------------
         * BYTE 0 : Start
         *--------------------------------------------*/
        testBuf[0] = 0x5A;


        /*---------------------------------------------
         * BYTE 1-4 : Read Voltage
         *--------------------------------------------*/
        snprintf(temp, sizeof(temp), "%04u", voltageValue);

        testBuf[1] = temp[0];
        testBuf[2] = temp[1];
        testBuf[3] = temp[2];
        testBuf[4] = temp[3];


        /*---------------------------------------------
         * BYTE 5-8 : Read Current
         *--------------------------------------------*/
        snprintf(temp, sizeof(temp), "%04u", currentValue);

        testBuf[5] = temp[0];
        testBuf[6] = temp[1];
        testBuf[7] = temp[2];
        testBuf[8] = temp[3];


        /*---------------------------------------------
         * BYTE 9-12 : Set Current
         *--------------------------------------------*/
        snprintf(temp, sizeof(temp), "%04u", setCurrentValue);

        testBuf[9]  = temp[0];
        testBuf[10] = temp[1];
        testBuf[11] = temp[2];
        testBuf[12] = temp[3];


        /*---------------------------------------------
         * BYTE 13 : LED Status
         *--------------------------------------------*/

        if (Output_Status == 1)
        {
            /* Set bit 5 */
            LedStatus |= (1U << 5);
        }
        else
        {
            /* Clear bit 5 */
            LedStatus &= ~(1U << 5);
        }

        testBuf[13] = LedStatus;


        /*---------------------------------------------
         * BYTE 14 : Fault Status
         *--------------------------------------------*/
        testBuf[14] = SystemStatus;


        /*---------------------------------------------
         * BYTE 15 : End
         *--------------------------------------------*/
        testBuf[15] = 0xEF;


        /*---------------------------------------------
         * Send complete frame
         *--------------------------------------------*/
        HAL_UART_Transmit(
            &huart1,
            (const uint8_t *)testBuf,
            16,
            1000
        );
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = display_tx_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(display_tx_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = display_rx_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(display_rx_GPIO_Port, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, display_tx_Pin|display_rx_Pin);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
