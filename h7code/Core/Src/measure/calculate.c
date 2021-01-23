#include "main.h"
#include "calculate.h"

static float VCurrentFromInt(int32_t measureData)
{
    float ku = 10;
    float vmax = 2.0;
    float korr = 1/0.9625f; //Коррекция тока
    //float korr = 1.f;
    return measureData*vmax/(1<<23)/ku*korr;
}

static float VOutFromInt(int32_t measureData)
{
    float ku = 10; //ОУ
    float ku1 = 0.027;//Резистивный делитель
    float korr = 1.222f/1.178f; //Коррекция напряжения
    float vmax = 2.0;
    return measureData*vmax/(1<<23)/(ku*ku1)*korr;
}

void calculate(int32_t measureV, int32_t measureI,
               float Rshunt, CalcResult* calc_result)
{
    measureI -= 500; //adc0
    measureV -= -100; //adc1

    float Vcurrent = VCurrentFromInt(measureI);
    float Vout = VOutFromInt(measureV);

    calc_result->Vcurrent = Vcurrent;
    calc_result->Vout = Vout;
    float current = Vcurrent/Rshunt;

    calc_result->current = current;

    float resistance = 1e9;
    calc_result->infinity_resistance = true;
    if(current>1e-10)
    {
        calc_result->infinity_resistance = false;
        resistance = (Vout-Vcurrent)/current;
    }

    calc_result->resistance = resistance;
}
