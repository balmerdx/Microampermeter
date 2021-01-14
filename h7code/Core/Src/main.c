/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

#include "hardware/delay.h"
#include "hardware/gpios.h"
#include "hardware/quadrature_encoder.h"
#include "hardware/qspi_mem.h"
#include "UTFT.h"
#include "fonts/font_condensed30.h"
#include "fonts/font_condensed59.h"

/* Private variables ---------------------------------------------------------*/
QSPI_HandleTypeDef hqspi;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config400MHz(void);
void SystemClock_Config100MHz(void);
static void CPU_CACHE_Enable(void);
static void MX_GPIO_Init(void);
static void VREF_Init();
static void MX_QUADSPI_Init(void);

static bool test_usb = false;
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config400MHz();
  //SystemClock_Config100MHz();

  DelayInit();
  VREF_Init();

  if(test_usb)
  {
      MX_USB_DEVICE_Init();

      UTFT_InitLCD(UTFT_LANDSCAPE);
      QuadEncInit();

      if(1)
      { //Test LCD speed
          uint32_t start_ms = TimeMs();
          int count = 256;
          for(int i=0; i<count; i++)
            UTFT_fillScr(i);

          uint32_t delta_ms = TimeMs()-start_ms;

          UTFT_fillScr(VGA_BLUE);
          UTFT_setColor(VGA_GREEN);
          UTFT_fillRect(10,10, 20,20);
          UTF_SetFont(font_condensed59);

          int x = 0, y = 100;
          x = UTF_printNumF(count*1000.f/delta_ms, x, y, 3, 100, UTF_RIGHT);
          UTF_SetFont(font_condensed30);
          UTF_DrawString(x, y, " FPS");

          while(1)
          {

          }
      }

      UTFT_fillScr(VGA_BLUE);
      UTFT_setColor(VGA_GREEN);
      UTFT_fillRect(10,10, 20,20);
      UTF_SetFont(font_condensed30);

      while(1)
      {
          int16_t value = QuadEncValue();
          int xstart = 0;
          int x = xstart, y = 100;
          x = UTF_printNumI(value, x, y, 100, UTF_RIGHT);
          UTF_SetFont(font_condensed30);
          UTF_DrawString(x, y, " ENC");

          y += UTF_Height();

          x = xstart;
          UTF_DrawString(x, y, QuadEncButton()?"P":"X");

          HAL_Delay(30);
      }

  }


  UTFT_InitLCD(UTFT_LANDSCAPE);
  UTFT_fillScr(VGA_BLACK);
  UTFT_setColor(VGA_GREEN);
  UTFT_fillRect(10,10, 20,20);
  UTF_SetFont(font_condensed30);


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_QUADSPI_Init();

  if(!QspiMemInit(&hqspi))
  {
      UTF_DrawString(0, 100, "QspiMemInit fail");
      while(1);
  }

  //UTF_DrawString(0, 100, "QspiMemInit ok");


  float f = 0;
  int idx = 3;

#define QBUFFER_SIZE 10000
#define QBUFFER_ADDR 0
static uint8_t buffer[QBUFFER_SIZE];

  while (1)
  {

      int x, y;
      int xstart = 50;
      int yoffset = 30;
      y = 0;

      x = UTF_printNumF(f, xstart, y, 3, 100, UTF_RIGHT);
      UTF_DrawString(x, y, "Hello, QT!");
      y += yoffset;

      bool ok;
      for(int i=0; i<QBUFFER_SIZE; i++)
          buffer[i] = idx + i;
      ok = QspiMemWrite(&hqspi, QBUFFER_ADDR, QBUFFER_SIZE, buffer);

      for(int i=0; i<QBUFFER_SIZE; i++)
          buffer[i] = 0;
      x = UTF_DrawString(xstart, y, ok?"Write ok":"Write fail");
      y += yoffset;

      uint16_t start_time = TimeUs();
      ok = QspiMemRead(&hqspi, QBUFFER_ADDR, QBUFFER_SIZE, buffer);
      uint16_t read_time = TimeUs()-start_time;

      if(!ok)
          x = UTF_DrawString(xstart, y, "Read fail");
      else
      {
          bool equal = true;
          for(int i=0; i<QBUFFER_SIZE; i++)
              if(buffer[i]!=(uint8_t)(idx + i))
                  equal = false;
          if(equal)
          {
              x = UTF_DrawString(xstart, y, equal ? "Mem equal" : "Mem not equal");
          }
      }

      x = UTF_printNumI(buffer[0], x, y, 100, UTF_LEFT);
      y += yoffset;

      x = UTF_DrawString(xstart, y, "Read time=");
      x = UTF_printNumI(read_time, x, y, 100, UTF_LEFT);
      x = UTF_DrawString(x, y, " us   ");
      y += yoffset;

      x = UTF_DrawString(0, y, "GetPCLK1Freq=");
      x = UTF_printNumI(HAL_RCC_GetPCLK1Freq(), x, y, UTFT_getDisplayXSize()-x, UTF_LEFT);
      y += yoffset;
      x = UTF_DrawString(0, y, "GetHCLKFreq=");
      x = UTF_printNumI(HAL_RCC_GetHCLKFreq(), x, y, UTFT_getDisplayXSize()-x, UTF_LEFT);
      y += yoffset;



      f += 0.01f;
      idx++;

      DelayMs(500);
  }
}


void SystemClock_Config400MHz(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = 2;
  //RCC_OscInitStruct.PLL.PLLQ = 32;//25 MHz QSPI
  RCC_OscInitStruct.PLL.PLLQ = 8;//50 MHz QSPI
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;



  if(1)
  {
      //200 MHz periph
      RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
      RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
      RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
      RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  } else
  {
      //100 MHz
      RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV4;
      RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV4;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV4;
      RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV4;
      RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV4;
  }



  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_QSPI;
  PeriphClkInitStruct.PLL3.PLL3M = 8;
  PeriphClkInitStruct.PLL3.PLL3N = 192;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
  /** Enable USB Voltage detector
  */
  HAL_PWREx_EnableUSBVoltageDetector();

  CPU_CACHE_Enable();
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config100MHz(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_0;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_SPI4
                              |RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.PLL2.PLL2M = 8;
  PeriphClkInitStruct.PLL2.PLL2N = 192;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 4;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 8;
  PeriphClkInitStruct.PLL3.PLL3N = 192;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  //RCC_MCODIV_1 - 8 МГц на ADC CLK
  //RCC_MCODIV_2 - 4 МГц на ADC CLK
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
  /** Enable USB Voltage detector
  */
  HAL_PWREx_EnableUSBVoltageDetector();
}

static void VREF_Init()
{
    __HAL_RCC_VREF_CLK_ENABLE();
    HAL_SYSCFG_EnableVREFBUF();
    HAL_SYSCFG_VREFBUF_HighImpedanceConfig(SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE);
    HAL_SYSCFG_VREFBUF_VoltageScalingConfig(SYSCFG_VREFBUF_VOLTAGE_SCALE1); //2.5V
}

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 2;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = QSPI_MEM_BITS; //8 MB = 23 bits address
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_2;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
}

/* USER CODE BEGIN 4 */
/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
    while(1);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
