#include "main.h"
#include "calculate.h"
#include "settings.h"

float calculateVoltage(int32_t measureV)
{
    return (measureV-g_settings.offset_adc_V) * g_settings_permanent.mul_V;
}

float calculateShuntVoltage(int32_t measureI)
{
    return (measureI-g_settings.offset_adc_I) * g_settings_permanent.mul_I;
}

float calculateCurrent(int32_t measureI, float RshuntInv)
{
    float Vcurrent = calculateShuntVoltage(measureI);
    return Vcurrent*RshuntInv;
}


void calculateRV(int32_t measureV, float current,
               float Rshunt, CalcResult* calc_result)
{
    float Vout = calculateVoltage(measureV);
    calc_result->Vout = Vout;

    calc_result->current = current;

    float resistance = 1e9;
    calc_result->infinity_resistance = true;
    if(current>1e-10)
    {
        calc_result->infinity_resistance = false;
        resistance = Vout/current - Rshunt;
    }

    calc_result->resistance = resistance;
}

void CorrectionInit()
{
}
