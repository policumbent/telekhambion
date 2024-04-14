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
ADC_HandleTypeDef hadc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC_Init(void);
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

    
    //Write data on nRF24 register
    nRF24_WritePayload(data, len);

    
    //Set CE Pin High in order to trigger the transmission
    nRF24_CE_H();
    HAL_Delay(5);
    //Wait for empy fifo
    do{
      flag=nRF24_GetStatus_TXFIFO();
      
    }while((flag != nRF24_STATUS_RXFIFO_EMPTY));

    //Set CE Low 
    nRF24_CE_L();
    
    return;
}

void button_debouncing(Button *button){
  button->current = HAL_GPIO_ReadPin(button->GPIO_Port, button->GPIO_Pin);

  if(button->previous != button->current){
    button->toggled = 1;
    button->previous = button->current;
  }
  else if(button->toggled){
    button->toggled = 0;
    button->debounced_value = button->previous;
  }
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
  //MX_ADC_Init();

  /* USER CODE BEGIN 2 */
  //HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
  //HAL_ADC_Start(&hadc);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  const uint8_t TX_Address[]={0xBB,0xBB,0xBB,0xBB,0xBB};
  

  /*NRF24 SETUP AND CHECK CONNECTIVITY*/
  HAL_Delay(100);
  nRF24_Init();
  HAL_Delay(100);
  while(nRF24_Check() == 0){
    HAL_UART_Transmit(&huart2,(uint8_t*) "invalid\r\n",sizeof("invalid\r\n"),500);
    HAL_Delay(1000);
  }
  
  HAL_UART_Transmit(&huart2,(uint8_t*) "NRF connected\r\n",sizeof("NRF connected\r\n"),500);


  nRF24_SetOperationalMode(nRF24_MODE_TX);
  nRF24_SetRFChannel(0);
  nRF24_SetTXPower(nRF24_TXPWR_18dBm);
  nRF24_SetDataRate(nRF24_DR_1Mbps);
  nRF24_SetAddrWidth(5);
  nRF24_SetAddr(6, TX_Address);
  nRF24_SetAddr(0, TX_Address);
  nRF24_SetAutoRetr(nRF24_ARD_NONE,0);
  nRF24_SetCRCScheme(nRF24_CRC_1byte);
  nRF24_SetDynamicPayloadLength(nRF24_DPL_OFF);
  nRF24_DisableAA(7);

  nRF24_SetPowerMode(nRF24_PWR_UP);
  HAL_Delay(500);
  
  nRF24_FlushTX();

  /*VARIABLES DECLARATION*/  
  uint8_t buf[40];
  uint8_t payload=0;
  uint32_t ms_old_deb = HAL_GetTick();
  uint32_t ms_now_deb;
  //uint32_t ms_old_adc = HAL_GetTick();
  //uint32_t ms_now_adc;
  Button b_upshift;
  Button b_downshift;
  Button b_radio;

  //uint16_t raw_data;

  //float voltage;

  //Buttons initialization
  b_downshift.previous = 0;
  b_downshift.current = 0;
  b_downshift.toggled = 0;
  b_downshift.debounced_value = 0;
  b_downshift.GPIO_Port = B_DOWNSHIFT_GPIO_Port;
  b_downshift.GPIO_Pin = B_DOWNSHIFT_Pin;

  b_upshift.previous = 0;
  b_upshift.current = 0;
  b_upshift.toggled = 0;
  b_upshift.debounced_value = 0;
  b_upshift.GPIO_Port = B_UPSHIFT_GPIO_Port;
  b_upshift.GPIO_Pin = B_UPSHIFT_Pin;

  b_radio.previous = 0;
  b_radio.current = 0;
  b_radio.toggled = 0;
  b_radio.debounced_value = 0;
  b_radio.GPIO_Port = B_RADIO_GPIO_Port;
  b_radio.GPIO_Pin = B_RADIO_Pin;

  HAL_Delay(1000);

  while (1)
  { 
    ms_now_deb = HAL_GetTick();
    if(ms_now_deb - ms_old_deb >= DEBOUNCING_PERIOD){
      //DEBOUNCING
      button_debouncing(&b_upshift);
      button_debouncing(&b_downshift);

      payload = (b_downshift.debounced_value << SHIFT_DS) |
                (b_upshift.debounced_value << SHIFT_US) |
                (b_radio.debounced_value << SHIFT_RADIO);
    

      //sprintf((char *)buf, "US:%u DS:%u RD:%u Payload:%u\r\n",
      //        b_upshift.debounced_value,
      //        b_downshift.debounced_value,
      //        b_radio.debounced_value,
      //        payload);

      //HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), 100);
      nRF24_SendPayload((uint8_t*) &payload, sizeof(payload));
      ms_old_deb=ms_now_deb;
    }
  
    //ms_now_adc = HAL_GetTick();

    //if(ms_now_adc - ms_old_adc >= ADC_PERIOD){
    //  HAL_ADC_PollForConversion(&hadc,HAL_MAX_DELAY);
//
    //  raw_data = HAL_ADC_GetValue(&hadc);
//
    //  voltage = (float)raw_data * (float)0.0164;
//
    //  sprintf((char*)buf, "raw value: %d | Value: %.2f\r\n", raw_data,voltage);
    //  HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), 100);
    //  ms_old_adc = ms_now_adc;
    //}

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
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_8B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = ENABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

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
