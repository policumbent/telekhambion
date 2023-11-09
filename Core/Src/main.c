/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nrf24.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void print_fifo_status(uint8_t flag){
  switch(flag){
    case nRF24_STATUS_TXFIFO_EMPTY:
      HAL_UART_Transmit(&huart2,(uint8_t*) "EMPTYFIFO\r\n", sizeof("EMPTYFIFO\r\n"), 500);
      break;
    case nRF24_STATUS_TXFIFO_DATA:
      HAL_UART_Transmit(&huart2,(uint8_t*) "DATAFIFO\r\n", sizeof("DATAFIFO\r\n"), 500);
      break;
    case nRF24_STATUS_TXFIFO_FULL:
      HAL_UART_Transmit(&huart2,(uint8_t*) "FULLFIFO\r\n", sizeof("FULLFIFO\r\n"), 500);
      break;
    case nRF24_STATUS_TXFIFO_ERROR:
      HAL_UART_Transmit(&huart2,(uint8_t*) "ERRORFIFO\r\n", sizeof("ERRORFIFO\r\n"), 500);
      break;    
  }
}

void nRF24_SendPayload(uint8_t* data, uint8_t len) {
    uint8_t flag;

    //Set CE pin low in order to force nRF state in Standby-I
    nRF24_CE_L();

    //print_fifo_status(nRF24_GetStatus_TXFIFO());
    
    //Write data on nRF24 register
    nRF24_WritePayload(data, len);

    //print_fifo_status(nRF24_GetStatus_TXFIFO());
    
    //Set CE Pin High in order to trigger the transmission
    nRF24_CE_H();
    HAL_Delay(5);
    //Wait for empy fifo
    do{
      flag=nRF24_GetStatus_TXFIFO();
      //print_fifo_status(flag);
      //HAL_Delay(500);
    }while((flag != nRF24_STATUS_RXFIFO_EMPTY));

    //Set CE Low 
    nRF24_CE_L();
    
    //print_fifo_status(nRF24_GetStatus_TXFIFO());

    return;
}

uint8_t button_decode(){
  uint8_t ds_status, us_status, radio_status, payload;
  ds_status = HAL_GPIO_ReadPin(B_DOWNSHIFT_GPIO_Port, B_DOWNSHIFT_Pin);
  us_status = HAL_GPIO_ReadPin(B_UPSHIFT_GPIO_Port, B_UPSHIFT_Pin);
  radio_status = HAL_GPIO_ReadPin(B_RADIO_GPIO_Port, B_RADIO_Pin);

  payload = (ds_status << SHIFT_DS) | (us_status << SHIFT_US) | (radio_status << SHIFT_RADIO);
  return payload;
}

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  const uint8_t TX_Address[]={0xBB,0xBB,0xBB,0xBB,0xBB};
  //char check[]="valid check\r\n";
  //uint8_t not_check[]="not valid check\r\n";

  /*NRF24 SETUP*/
  HAL_Delay(100);
  nRF24_Init();
  HAL_Delay(1000);
  while(nRF24_Check() == 0){
    HAL_UART_Transmit(&huart2,(uint8_t*) "invalid\r\n",sizeof("invalid\r\n"),500);
    HAL_Delay(1000);
  }
  
  HAL_UART_Transmit(&huart2,(uint8_t*) "valid\r\n",sizeof("valid\r\n"),500);


  nRF24_SetOperationalMode(nRF24_MODE_TX);
  nRF24_SetRFChannel(0);
  nRF24_SetTXPower(nRF24_TXPWR_18dBm);
  nRF24_SetDataRate(nRF24_DR_250kbps);
  nRF24_SetAddrWidth(5);
  nRF24_SetAddr(6, TX_Address);
  nRF24_SetAddr(0, TX_Address);
  nRF24_SetAutoRetr(nRF24_ARD_NONE,0);
  nRF24_SetCRCScheme(nRF24_CRC_1byte);
  nRF24_SetDynamicPayloadLength(nRF24_DPL_OFF);
  nRF24_DisableAA(7);

  nRF24_SetPowerMode(nRF24_PWR_UP);
  HAL_Delay(500);
  
  //uint8_t stringa[]="barimerda\r\n";
  nRF24_FlushTX();
    
  uint8_t buf[8];
  uint8_t payload=0;

  HAL_Delay(3000);

  while (1)
  { 
    payload = button_decode();
    HAL_UART_Transmit(&huart2, (uint8_t*) "----------\r\n", sizeof("----------\r\n"), 100);
    sprintf((char *)buf, "%u\r\n", payload);
    HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), 100);
    nRF24_SendPayload((uint8_t*) &payload, sizeof(payload));

    print_fifo_status(nRF24_GetStatus_TXFIFO());
    
    HAL_Delay(500);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NRF_CSN_Pin NRF_CE_Pin */
  GPIO_InitStruct.Pin = NRF_CSN_Pin|NRF_CE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : B_DOWNSHIFT_Pin B_RADIO_Pin */
  GPIO_InitStruct.Pin = B_DOWNSHIFT_Pin|B_RADIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : B_UPSHIFT_Pin */
  GPIO_InitStruct.Pin = B_UPSHIFT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(B_UPSHIFT_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
