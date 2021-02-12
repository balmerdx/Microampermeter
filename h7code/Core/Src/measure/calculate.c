#include "main.h"
#include "calculate.h"
#include "settings.h"

float current_mul = 1;
const float voltage_mul_original =
        1/10.f*//ОУ
        1/0.027f*//Резистивный делитель
        2.0f*//Напряжение Vref
        1.f/(1<<23);
float voltage_mul = voltage_mul_original*
        1.222f/1.178f;

static float VCurrentFromInt(int32_t measureData)
{
    return measureData * current_mul;
}

float calculateCurrent(int32_t measureI, float RshuntInv)
{
    measureI -= g_settings.offset_adc_I; //adc0
    float Vcurrent = VCurrentFromInt(measureI);
    return Vcurrent*RshuntInv;
}

void calculateRV(int32_t measureV, float current,
               float Rshunt, CalcResult* calc_result)
{
    float Vout = (measureV-g_settings.offset_adc_V)*voltage_mul;
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
        float korr = 1/0.9625f; //Коррекция тока
        current_mul = vmax/(1<<23)/ku*korr;
    }

}
