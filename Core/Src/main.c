/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  count1++;
	  count3++;
	    if(state<99)
	    {
	 	  if((usart1RxBuff[0]==0x5b) && (usart1RxBuff[1]==0x05) && (usart1RxBuff[2]==0xef) && mode == LOCAL)
	 	  {
	 		  usart1RxBuff[0]=0;
	 		  usart1RxBuff[1]=0;
	 		  usart1RxBuff[2]=0;

	 		  if(Output_Status==0)
	 		  {
	 			  Output_Status = 1;
	 		  }
	 		  else if(Output_Status==1)
	 		  {
	 			  Output_Status = 0;
	 		  }
	 	  }
	    }

	    //RM Key pressed
	    if((state < 99) && (usart1RxBuff[0] == 0x5B) && (usart1RxBuff[1] == 0x09) &&(usart1RxBuff[2] == 0xef))
	    {
	 	    usart1RxBuff[0] = 0U;
	 	    usart1RxBuff[1] = 0U;
	 	    usart1RxBuff[2] = 0U;

	 	    state = 101;

	    }

	 	  /* Bit 0 = ACF */
	 	  if (ACF_Status == 1U)
	 	  {
	 	      SystemStatus |= (1U << 0);
	 	  }
	 	  else
	 	  {
	 	      SystemStatus &= ~(1U << 0);
	 	  }


	 	  /* Bit 1 = OTP */
	 	  if (OTP_Status == 1U)
	 	  {
	 	      SystemStatus |= (1U << 1);
	 	  }
	 	  else
	 	  {
	 	      SystemStatus &= ~(1U << 1);
	 	  }


	 	  /* Bit 2 = FAN FAIL */
	 	  if (FanFail_Status == 1U)
	 	  {
	 	      SystemStatus |= (1U << 2);
	 	  }
	 	  else
	 	  {
	 	      SystemStatus &= ~(1U << 2);
	 	  }


	 	  /* Bit 3 = OVERLOAD */
	 	  if (Overload_Status == 1U)
	 	  {
	 	      SystemStatus |= (1U << 3);
	 	  }
	 	  else
	 	  {
	 	      SystemStatus &= ~(1U << 3);
	 	  }


	       if(SystemStatus>0)
	       {
	     	  state = 99;
	       }

	       if(count3 == 3000)
	       {
	     	  state = 101;
	       }
	 	  switch(state)
	 	  {

	 	  case(0):
	 				if(count1>2000)
	 			    {
	 					display_update();
	 					count1 = 0;
	 			    }
	 	  	  	  	  if(usart1RxBuff[0]==0x5b && usart1RxBuff[1]==0x01 && usart1RxBuff[2]==0xef)
	 				  {
	 					  state = 1;
	 					  usart1RxBuff[0]=0;
	 					  usart1RxBuff[1]=0;
	 					  usart1RxBuff[2]=0;
	 					  display_ISET();

	 				  }

	 	   break;

	 	  case(1):      // Set Button Pressed
	 	  				if(count2>50000000)
	 	  			    {
	 	  					//display_update();
	 	  					count2 = 0;
	 	  					state = 0;
	 	  			    }
	 	  	  	  	    count2++;

	 	  	  	  	 if(usart1RxBuff[0]==0x5b && usart1RxBuff[1]==0x01 && usart1RxBuff[2]==0xef)
	 				  {
	 					  //state = 1;
	 					  usart1RxBuff[0]=0;
	 					  usart1RxBuff[1]=0;
	 					  usart1RxBuff[2]=0;
	 					  display_ISET();
	 					  state = 0;

	 				  }





	 	  break;

	 	  case(99) : if(count1>2000)
	 					{
	 		  	  	  	  Fault_Display();
	 						count1 = 0;
	 					}

	 	  if(usart1RxBuff[0]==0x5b && usart1RxBuff[1]==0x05 && usart1RxBuff[2]==0xef)
	 	 	  {
	 	 		  usart1RxBuff[0]=0;
	 	 		  usart1RxBuff[1]=0;
	 	 		  usart1RxBuff[2]=0;

	 	 		  state=0;
	 	 	  }



	 		break;

	 	  case(101): // Handler for Remote button press
	 	  {

	 		    if ((usart1RxBuff[0] == 0x5B) && (usart1RxBuff[1] == 0x09) && (usart1RxBuff[2] == 0xEF))
	 		    {
	 		        usart1RxBuff[0] = 0U;
	 		        usart1RxBuff[1] = 0U;
	 		        usart1RxBuff[2] = 0U;

	 		        if (mode == REMOTE)
	 		        {
	 		            mode = LOCAL;

	 		            LedStatus = 0x02;
	 		        }
	 		        else
	 		        {
	 		            mode = REMOTE;

	 		            LedStatus = 0x00;
	 		        }

	 		        state = 0;
	 		    }
	 		    else if ((usart1RxBuff[0] == 0x5B) && (usart1RxBuff[1] == 0x05) && (usart1RxBuff[2] == 0xEF))
	 		    {
	 		        usart1RxBuff[0] = 0U;
	 		        usart1RxBuff[1] = 0U;
	 		        usart1RxBuff[2] = 0U;

	 		        if (mode == LOCAL)
	 		        {
	 		            if (Output_Status == 0U)
	 		            {
	 		                Output_Status = 1U;
	 		            }
	 		            else
	 		            {
	 		                Output_Status = 0U;
	 		            }
	 		        }
	 		        state = 0;
	 		    }

	 		    break;
	 	  }

	 	  }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
