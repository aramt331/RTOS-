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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>      // vsnprintf
#include <stdarg.h>     // va_list, va_start, va_end
#include <string.h>     // strlen
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USE_MUTEX      1
#define LOG_BUF_SIZE   256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* Definitions for FastTask */
osThreadId_t FastTaskHandle;
const osThreadAttr_t FastTask_attributes = {
  .name = "FastTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MedTask */
osThreadId_t MedTaskHandle;
const osThreadAttr_t MedTask_attributes = {
  .name = "MedTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for SlowTask */
osThreadId_t SlowTaskHandle;
const osThreadAttr_t SlowTask_attributes = {
  .name = "SlowTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void StartFastTask(void *argument);
void StartMedTask(void *argument);
void StartSlowTask(void *argument);

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
  /* USER CODE BEGIN 2 */
  {
      const char *banner =
#if USE_MUTEX
          "\r\n=== UART MUTEX DEMO — MUTEX ON  ===\r\n\r\n";
#else
          "\r\n=== UART MUTEX DEMO — MUTEX OFF (expect garbage) ===\r\n\r\n";
#endif
      HAL_UART_Transmit(&huart1, (uint8_t *)banner, strlen(banner), 200);
  }



  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of FastTask */
  FastTaskHandle = osThreadNew(StartFastTask, NULL, &FastTask_attributes);

  /* creation of MedTask */
  MedTaskHandle = osThreadNew(StartMedTask, NULL, &MedTask_attributes);

  /* creation of SlowTask */
  SlowTaskHandle = osThreadNew(StartSlowTask, NULL, &SlowTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  if (FastTaskHandle == NULL)
      HAL_UART_Transmit(&huart1, (uint8_t *)"FAST FAIL\r\n", 11, 100);
  else
      HAL_UART_Transmit(&huart1, (uint8_t *)"FAST OK\r\n", 9, 100);

  if (MedTaskHandle == NULL)
      HAL_UART_Transmit(&huart1, (uint8_t *)"MED FAIL\r\n", 11, 100);
  else
      HAL_UART_Transmit(&huart1, (uint8_t *)"MED OK\r\n", 9, 100);

  if (SlowTaskHandle == NULL)
      HAL_UART_Transmit(&huart1, (uint8_t *)"Slow FAIL\r\n", 11, 100);
  else
      HAL_UART_Transmit(&huart1, (uint8_t *)"Slow OK\r\n", 9, 100);
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
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
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartFastTask */
/**
* @brief Function implementing the FastTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFastTask */
void StartFastTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	(void)argument;

	for(;;){

		HAL_UART_Transmit(&huart1, (uint8_t*)"High task wants CPU\r\n", 21, HAL_MAX_DELAY); // short work
		osDelay(100);

	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartMedTask */
/**
* @brief Function implementing the MedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMedTask */
void StartMedTask(void *argument)
{
  /* USER CODE BEGIN StartMedTask */

  (void)argument;

  /* Infinite loop */
  for(;;)
  {
      HAL_UART_Transmit(&huart1, (uint8_t*)"Med task running\r\n", 18, HAL_MAX_DELAY);
      for (volatile int i=0; i<1000000; i++); // busy loop
      osDelay(100);


  }
  /* USER CODE END StartMedTask */
}

/* USER CODE BEGIN Header_StartSlowTask */
/**
* @brief Function implementing the SlowTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSlowTask */
void StartSlowTask(void *argument)
{
  /* USER CODE BEGIN StartSlowTask */
  (void)argument;

  /* Infinite loop */
  for(;;)
  {
      HAL_UART_Transmit(&huart1, (uint8_t*)"Low task running\r\n", 18, HAL_MAX_DELAY); // busy loop
      osDelay(100);
  }
  /* USER CODE END StartSlowTask */
}

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
#ifdef USE_FULL_ASSERT
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
