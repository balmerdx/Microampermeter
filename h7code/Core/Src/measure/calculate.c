#include "main.h"
#include "calculate.h"
#include "settings.h"

float current_mul_original = 1.f;
float current_mul = 1.f;
float voltage_mul_original = 1.f;
float voltage_mul = 1.f;

static float VCurrentFromInt(int32_t measureData)
{
    return (measureData-g_settings.offset_adc_I) * current_mul;
}

float calculateCurrent(int32_t measureI, float RshuntInv)
{
    float Vcurrent = VCurrentFromInt(measureI);
    return Vcurrent*RshuntInv;
}

float calculateVoltage(int32_t measureV)
{
    return (measureV-g_settings.offset_adc_V)*voltage_mul;
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
    {//calculate currentMul
        float ku = 10;
        float vmax = 2.0;
        //float korr = 1/0.9625f; //Коррекция тока
        current_mul_original = vmax/(1<<23)/ku;//*korr;
        current_mul = current_mul_original;
    }

    {
        voltage_mul_original = 1/10.f*//ОУ
            1/0.027f*//Резистивный делитель
            2.0f*//Напряжение Vref
            1.f/(1<<23);

        voltage_mul = voltage_mul_original;
    }
}
