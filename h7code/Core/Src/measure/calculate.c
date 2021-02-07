#include "main.h"
#include "calculate.h"

int32_t current_zero_offset = -500;
float current_mul = 1;

static float VCurrentFromInt(int32_t measureData)
{
    return measureData * current_mul;
}

static float VOutFromInt(int32_t measureData)
{
    float ku = 10; //ОУ
    float ku1 = 0.027;//Резистивный делитель
    float korr = 1.222f/1.178f; //Коррекция напряжения
    float vmax = 2.0;
    return measureData*vmax/(1<<23)/(ku*ku1)*korr;
}

float calculateCurrent(int32_t measureI, float RshuntInv)
{
    measureI += current_zero_offset; //adc0
    float Vcurrent = VCurrentFromInt(measureI);
    return Vcurrent*RshuntInv;
}

void calculateRV(int32_t measureV, float current,
               float Rshunt, CalcResult* calc_result)
{
    measureV -= -100; //adc1

    float Vout = VOutFromInt(measureV);
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
