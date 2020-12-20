#include "main.h"
#include "voltage.h"

void VoltageInit()
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_Start(&hadc2);
}

void VoltageGet(float* Vout, float* Vbat)
{
    LL_ADC_REG_StartConversion(hadc1.Instance);
    LL_ADC_REG_StartConversion(hadc2.Instance);
    HAL_ADC_PollForConversion(&hadc1, 10);
    HAL_ADC_PollForConversion(&hadc2, 10);
    uint32_t vout_value = HAL_ADC_GetValue(&hadc1);
    uint32_t vbat_value = HAL_ADC_GetValue(&hadc2);

    float Vref = 2.048f;

    *Vout = vout_value*(2*Vref/65535);
    *Vbat = vbat_value*(2*Vref/65535);
}
