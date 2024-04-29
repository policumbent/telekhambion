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
#include "adc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_ADC_Init();
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
  #if DEBUG
    uint8_t buf[40];
  #endif /* DEBUG */

  uint8_t send = 0;
  uint8_t payload=0;

  #if ADC_ON
    uint32_t ms_old_adc = HAL_GetTick();
    uint32_t ms_now_adc;

    uint16_t raw_data;

    float voltage;
  #endif /* ADC_ON*/
  
  controller_button_init();

  HAL_Delay(1000);

  while (1)
  {
    send = controller_encode_payload(&payload);
    if (send) {
      nRF24_SendPayload((uint8_t*) &payload, sizeof(payload));
    }

    #if DEBUG
      sprintf((char *)buf, "US:%u DS:%u RD:%u Payload:%u\r\n",
              b_upshift.debounced_value,
              b_downshift.debounced_value,
              b_radio.debounced_value,
              payload);
      HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), 100);
    #endif /* DEBUG */

    #if ADC_ON
      ms_now_adc = HAL_GetTick();

      if(ms_now_adc - ms_old_adc >= ADC_PERIOD){
        HAL_ADC_PollForConversion(&hadc,HAL_MAX_DELAY);

        raw_data = HAL_ADC_GetValue(&hadc);

        voltage = (float)raw_data * (float)0.0164;

        #if DEBUG
          sprintf((char*)buf, "raw value: %d | Value: %.2f\r\n", raw_data,voltage);
          HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), 100);
        #endif /* DEBUG */
        ms_old_adc = ms_now_adc;
      }
    #endif /* ADC_ON */

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
