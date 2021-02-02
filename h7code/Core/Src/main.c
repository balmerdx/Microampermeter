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
#include "hardware/ADS1271_input.h"
#include "UTFT.h"
#include "fonts/font_condensed30.h"
#include "fonts/font_condensed59.h"

#include "measure/calculate.h"
#include "measure/receive_data.h"
#include "measure/usb_communication.h"

/* Private variables ---------------------------------------------------------*/
QSPI_HandleTypeDef hqspi;
SAI_HandleTypeDef hsai_BlockA1;
DMA_HandleTypeDef hdma_sai1_a;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config400MHz(void);
static void CPU_CACHE_Enable(void);
static void MX_DMA_Init(void);
static void MX_GPIO_Init(void);
static void VREF_Init();
static void MX_QUADSPI_Init(void);
static void MX_SAI1_Init(void);

#define QBUFFER_SIZE 2500
#define QBUFFER_ADDR ((1<<QSPI_MEM_BITS)-1-QBUFFER_SIZE)
static uint32_t buffer_qspi[QBUFFER_SIZE];

bool WriteAllQspi(int idx)
{
    //Пишем почти всю память, только кусочек в конце не дописываем
    for(int addr = 0; addr+sizeof(buffer_qspi)-1 < (1<<QSPI_MEM_BITS); addr+= sizeof(buffer_qspi))
    {
        for(int i=0; i<QBUFFER_SIZE; i++)
            buffer_qspi[i] = idx++;
        if(!QspiMemWrite(&hqspi, addr, sizeof(buffer_qspi), (uint8_t*)buffer_qspi))
            return false;
    }

    return true;
}

bool CheckAllQspi(int idx, int* paddr)
{
    for(int addr = 0; addr+sizeof(buffer_qspi)-1 < (1<<QSPI_MEM_BITS); addr+= sizeof(buffer_qspi))
    {
        if(!QspiMemRead(&hqspi, addr, sizeof(buffer_qspi), (uint8_t*)buffer_qspi))
            return false;

        for(int i=0; i<QBUFFER_SIZE; i++)
            if(buffer_qspi[i] != idx++)
            {
                *paddr = addr + i*4;
                return false;
            }
    }

    return true;

}

volatile bool sai_error = false;
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    sai_error = true;
}

void SampleCapacitor()
{
    int x1=40, y1=40, x2=60, y2=60;
    UTFT_setColor(VGA_RED);
    UTFT_fillRect(x1, y1, x2, y2);

    SetEnableV(false);
    DelayMs(1000);

    UTFT_setColor(VGA_GREEN);
    UTFT_fillRect(x1, y1, x2, y2);

    StartAdcBufferFilling();
    DelayUs(100);
    SetEnableV(true);
    DelayUs(2000);
    SetEnableV(false);
    DelayMs(300);

    UTFT_setColor(VGA_BLACK);
    UTFT_fillRect(x1, y1, x2, y2);

    UTFT_setColor(VGA_WHITE);
}

void DrawResult()
{
    if(EncButtonPressed())
    {
        EnableCapturingTrigger();
        //SampleCapacitor();
    }

    int x, y;
    int xstart = 10;
    int yoffset = 30;
    y = 0;

    MidData d = GetMidData();

    CalcResult calc_result;
    calculate(d.adc1_mid, d.adc0_mid,
                   GetResistorValue(GetResistor()), &calc_result);

    x = UTF_DrawString(xstart, y, "count=");
    x = UTF_printNumI(d.samples_count, x, y, 100, UTF_RIGHT);
    y += yoffset;
/*
    x = UTF_DrawString(xstart, y, "ADC0=");
    x = UTF_printNumI(d.adc0_mid, x, y, 100, UTF_RIGHT);
    y += yoffset;

    x = UTF_DrawString(xstart, y, "ADC1=");
    x = UTF_printNumI(d.adc1_mid, x, y, 100, UTF_RIGHT);
    y += yoffset;
*/
    x = UTF_DrawString(xstart, y, "interrupt=");
    x = UTF_printNumF(GetPercentInInterrupt(), x, y, 2, UTF_StringWidth("00.00"), UTF_RIGHT);
    x = UTF_DrawString(x, y, "%");
    y += yoffset;

    x = UTF_DrawString(xstart, y, "Vcurrent(mV)=");
    x = UTF_printNumF(calc_result.Vcurrent*1000, x, y, 5, 100, UTF_RIGHT);
    y += yoffset;

    x = UTF_DrawString(xstart, y, "Vout(mV)=");
    x = UTF_printNumF(calc_result.Vout*1000, x, y, 5, 100, UTF_RIGHT);
    y += yoffset;

    if(calc_result.infinity_resistance)
    {
        x = UTF_DrawString(xstart, y, "Infinity            ");

    } else
    {
        x = UTF_DrawString(xstart, y, "R(KOm)=");
        x = UTF_printNumF(calc_result.resistance*1e-3f, x, y, 5, 100, UTF_RIGHT);
    }
    y += yoffset;

    x = UTF_DrawString(xstart, y, "I(uA)=");
    x = UTF_printNumF(calc_result.current*1e6, x, y, 3, 100, UTF_RIGHT);
    y += yoffset;

    RESISTOR r = GetResistor();
    if(r==RESISTOR_1_Om)
    {
        x = UTF_DrawString(xstart, y, "SHUNT=1 Om  ");
    } else
    if(r==RESISTOR_10_Om)
    {
        x = UTF_DrawString(xstart, y, "SHUNT=10 Om  ");
    } else
    if(r==RESISTOR_100_Om)
    {
        x = UTF_DrawString(xstart, y, "SHUNT=100 Om  ");
    } else
    {
        x = UTF_DrawString(xstart, y, "SHUNT=1 KOm  ");
    }
    y += yoffset;


    //Capture info
    char cap[100];
    strcpy(cap, "  ");
    if(sai_error)
    {
        strcat(cap, "SAI Error");
    } else
    if(IsEnabledCapturingTrigger())
    {
        strcat(cap, "Capture started");
    } else
    {
        if(IsAdcBufferFull())
            strcat(cap, "Capture completed");
        else
            strcat(cap, "No capture");
    }

    x = UTF_DrawStringJustify(0, y, cap, UTFT_getDisplayXSize(), UTF_LEFT);
    y += yoffset;
/*
    static int index = 0;
    if(index++%5==0)
    {
        float vmax = 0.1;
        float vmin = 0.005;
        //Переключаем пределы по халявному.
        if(calc_result.Vcurrent > vmax)
        {
            if(GetResistor()==RESISTOR_1_Kom)
            {
                SetResistor(RESISTOR_100_Om);
            } else
            if(GetResistor()==RESISTOR_100_Om)
            {
                SetResistor(RESISTOR_10_Om);
            }

        } else
        if(calc_result.Vcurrent < vmin)
        {
            if(GetResistor()==RESISTOR_10_Om)
            {
                SetResistor(RESISTOR_100_Om);
            } else
            if(GetResistor()==RESISTOR_100_Om)
            {
                SetResistor(RESISTOR_1_Kom);
            }
        }
    }
*/
}

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

  DelayInit();
  VREF_Init();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SAI1_Init();
  MX_USB_DEVICE_Init();
  MX_QUADSPI_Init();

  QuadEncInit();
  GpiosInit();
  ADS1271_Init();

  SetResistor(RESISTOR_1_Kom);

  UTFT_InitLCD(UTFT_LANDSCAPE);
  UTFT_fillScr(VGA_BLACK);
  UTFT_setColor(VGA_WHITE);
  UTF_SetFont(font_condensed30);

  if(!QspiMemInit(&hqspi))
  {
      UTF_DrawString(0, 100, "QspiMemInit fail");
      while(1);
  }

  ADS1271_Start();

  uint32_t prev_draw_time = TimeMs();
  while (1)
  {
      uint32_t cur_time = TimeMs();
      if( (uint32_t)(cur_time-prev_draw_time) > 500)
      {
          DrawResult();
          prev_draw_time = cur_time;
      }

      UsbCommandsQuant();
      DelayMs(1);
  }


  float f = 0;
  int idx = 0;
  while (1)
  {

      int x, y;
      int xstart = 50;
      int yoffset = 30;
      y = 0;

      x = UTF_printNumF(f, xstart, y, 3, 100, UTF_RIGHT);
      UTF_DrawString(x, y, "Hello, QT!");
      y += yoffset;

      bool write_all = true;
      bool ok;
      if(write_all)
      {
          ok = WriteAllQspi(idx);
      } else
      {
          for(int i=0; i<QBUFFER_SIZE; i++)
              buffer_qspi[i] = idx + i;
          ok = QspiMemWrite(&hqspi, QBUFFER_ADDR, sizeof(buffer_qspi), (uint8_t*)buffer_qspi);
      }

      for(int i=0; i<QBUFFER_SIZE; i++)
          buffer_qspi[i] = 0;
      x = UTF_DrawString(xstart, y, ok?"Write ok":"Write fail");
      y += yoffset;

      if(write_all)
      {
          uint16_t start_time = TimeMs();
          int err_addr;
          ok = CheckAllQspi(idx, &err_addr);
          uint16_t read_time = TimeMs()-start_time;
          x = UTF_DrawString(xstart, y, ok?"Check ok" : "Check fail adr=");
          //x = UTF_printNumI(err_addr, x, y, 100, UTF_LEFT);
          x = UTF_printNumI(buffer_qspi[0], x, y, 100, UTF_LEFT);

          y += yoffset;
          x = UTF_DrawString(xstart, y, "Read time=");
          x = UTF_printNumI(read_time, x, y, 100, UTF_LEFT);
          x = UTF_DrawString(x, y, " ms   ");
          y += yoffset;
      } else
      {
          uint16_t start_time = TimeUs();
          ok = QspiMemRead(&hqspi, QBUFFER_ADDR, sizeof(buffer_qspi), (uint8_t*)buffer_qspi);
          uint16_t read_time = TimeUs()-start_time;
          bool equal = true;
          for(int i=0; i<QBUFFER_SIZE; i++)
              if(buffer_qspi[i]!=(idx + i))
                  equal = false;
          if(!ok)
              x = UTF_DrawString(xstart, y, "Read fail");
          else
              x = UTF_DrawString(xstart, y, equal ? "Mem equal" : "Mem not equal");
          x = UTF_printNumI(buffer_qspi[0], x, y, 100, UTF_LEFT);
          y += yoffset;

          x = UTF_DrawString(xstart, y, "Read time=");
          x = UTF_printNumI(read_time, x, y, 100, UTF_LEFT);
          x = UTF_DrawString(x, y, " us   ");
          y += yoffset;
      }




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
  RCC_OscInitStruct.PLL.PLLQ = 4;//200 MHz QSPI
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

  PeriphClkInitStruct.PeriphClockSelection =
          RCC_PERIPHCLK_SAI1|
          RCC_PERIPHCLK_USB|
          RCC_PERIPHCLK_QSPI;
  PeriphClkInitStruct.PLL2.PLL2M = 8;
  PeriphClkInitStruct.PLL2.PLL2N = 216;
  //PeriphClkInitStruct.PLL2.PLL2P = 2; //108 MHz
  PeriphClkInitStruct.PLL2.PLL2P = 4; //54 MHz
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
  PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;//HCLK3 = 200 MHz
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1); //Видимо уже не требуется
  /** Enable USB Voltage detector
  */
  HAL_PWREx_EnableUSBVoltageDetector();

  CPU_CACHE_Enable();
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
  hqspi.Init.FifoThreshold = 4;
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
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockA1.Init.DataSize = SAI_DATASIZE_24;
  hsai_BlockA1.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockA1.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_HF;
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_MCKDIV;
  hsai_BlockA1.Init.Mckdiv = 2; //MCLK - 27 MHz
  //hsai_BlockA1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_ENABLE; //MCLK/512 FS = 52.73 KHz
  hsai_BlockA1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE; //MCLK/256 FS =105.46 KHz
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.PdmInit.Activation = DISABLE;
  hsai_BlockA1.Init.PdmInit.MicPairsNbr = 1;
  hsai_BlockA1.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;
  hsai_BlockA1.FrameInit.FrameLength = 64; //SCLK 6.75 MHz
  hsai_BlockA1.FrameInit.ActiveFrameLength = 1;
  hsai_BlockA1.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
  hsai_BlockA1.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
  hsai_BlockA1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockA1.SlotInit.FirstBitOffset = 0;
  hsai_BlockA1.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  hsai_BlockA1.SlotInit.SlotNumber = 2;
  hsai_BlockA1.SlotInit.SlotActive = SAI_SLOTACTIVE_ALL;
  if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
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
