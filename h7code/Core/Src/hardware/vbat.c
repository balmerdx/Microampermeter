#include "main.h"
#include "vbat.h"

/* ADC handle declaration */
ADC_HandleTypeDef        AdcHandle;
/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef   sConfig;

void VBatInit()
{
    AdcHandle.Instance          = ADC3;

    if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
    {
        /* ADC de-initialization Error */
        Error_Handler();
    }

    AdcHandle.Init.ClockPrescaler           = ADC_CLOCK_ASYNC_DIV4;
    AdcHandle.Init.Resolution               = ADC_RESOLUTION_12B;
    AdcHandle.Init.ScanConvMode             = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    AdcHandle.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
    AdcHandle.Init.LowPowerAutoWait         = DISABLE;                       /* Auto-delayed conversion feature disabled */
    AdcHandle.Init.ContinuousConvMode       = ENABLE;                        /* Continuous mode enabled (automatic conversion restart after each conversion) */
    AdcHandle.Init.NbrOfConversion          = 1;                             /* Parameter discarded because sequencer is disabled */
    AdcHandle.Init.DiscontinuousConvMode    = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    AdcHandle.Init.NbrOfDiscConversion      = 1;                             /* Parameter discarded because sequencer is disabled */
    AdcHandle.Init.ExternalTrigConv         = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
    AdcHandle.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
    AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;         /* DR register used as output (DMA mode disabled) */
    AdcHandle.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;         /* Left shift of final results */
    AdcHandle.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
    AdcHandle.Init.OversamplingMode         = ENABLE;                       /* Oversampling disable */
    AdcHandle.Init.Oversampling.Ratio                 = 128;
    AdcHandle.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_7;
    AdcHandle.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
    AdcHandle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;

    /* Initialize ADC peripheral according to the passed parameters */
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        Error_Handler();
    }

    /* ### - 2 - Start calibration ############################################ */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }

    /* ### - 3 - Channel configuration ######################################## */
    sConfig.Channel                = ADC_CHANNEL_VBAT;           /* Sampled channel number */
    sConfig.Rank                   = ADC_REGULAR_RANK_1;         /* Rank of sampled channel number ADCx_CHANNEL */
    sConfig.SamplingTime           = ADC_SAMPLETIME_810CYCLES_5; /* Sampling time (number of clock cycles unit) */
    sConfig.SingleDiff             = ADC_SINGLE_ENDED;           /* Single input channel */
    sConfig.OffsetNumber           = ADC_OFFSET_NONE;            /* No offset subtraction */
    sConfig.Offset                 = 0;                          /* Parameter discarded because offset correction is disabled */
    sConfig.OffsetRightShift       = DISABLE;                    /* No Right Offset Shift */
    sConfig.OffsetSignedSaturation = DISABLE;                    /* No Signed Saturation */
    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
    {
        Error_Handler();
    }
}

float VBatVoltage()
{
    if (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK)
    {
        Error_Handler();
    }

    /* Read the converted value */
    uint32_t uwConvertedValue = HAL_ADC_GetValue(&AdcHandle);

    /* Convert the result from 16 bit value to the voltage dimension (mV unit) */
    float Vref = 2.42f;//Volts - захардкодили актуальное значение
    return 4 * ((uwConvertedValue * Vref) / 0xFFF);
}
