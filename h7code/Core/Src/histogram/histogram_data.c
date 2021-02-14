#include "main.h"
#include "histogram_data.h"
#include "fastmath.h"
#include <math.h>

//Возвращает максимальную найденную ошибку
//fasterlog2 по факту имеет более-менее ошибку только для чисел больше 1, иначе отвратительная точность
//fastlog2 - не сильно быстрее, чем logf
//1.44269504f*logf(x) - 1000 штук за 276 us
//fastlog2(x) - 1000 штук за 161 us
//fasterlog2(x) - 1000 штук за 46 us
float TestFastLog2(TestLog2Type type)
{
    float error = 0;
    int count = 10000;
    float step = 0.002f;
    for(int i=0; i<count; i++)
    {
        //float p = (i+1)*step;
        float p = i*step+1;
        float x = powf(2, p);
        float p1;
        switch(type)
        {
        default:
        case TEST_LOG2_NORMAL: p1 = 1.44269504f*logf(x); break;
        case TEST_LOG2_FAST: p1 = fastlog2(x); break;
        case TEST_LOG2_FASTER: p1 = fasterlog2(x); break;
        }

        float err = fabsf(p-p1)/p;
        if(err > error)
            error = err;
    }

    return error;
}

volatile float result;

int TestFastLogSpeed(TestLog2Type type)
{
    int count = 1000;
    uint16_t start = TimeUs();
    float sum = 0;
    if(type==TEST_LOG2_NORMAL)
    {
        for(int i=0; i<count; i++)
        {
            float x = i+1e-4f;
            sum += 1.44269504f*logf(x);
        }
    }

    if(type==TEST_LOG2_FAST)
    {
        for(int i=0; i<count; i++)
        {
            float x = i+1e-4f;
            sum += fastlog2(x);
        }
    }

    if(type==TEST_LOG2_FASTER)
    {
        for(int i=0; i<count; i++)
        {
            float x = i+1e-4f;
            sum += fasterlog2(x);
        }
    }
    result = sum;

    return (uint16_t)(TimeUs()-start);
}


//
const float basket_per_decade = 0.125f;

int HistogramCurrentToBasketIndex(float current)
{
    if(current < HISTOGRAMM_MIN_CURRENT)
        current = HISTOGRAMM_MIN_CURRENT;

    int idx = (int)floorf(log10f(current/HISTOGRAMM_MIN_CURRENT)*HISTOGRAMM_BASKET_PER_DECADE);
    if(idx < 0)
        idx = 0;
    if(idx >= HISTOGRAMM_BASKETS)
        idx = HISTOGRAMM_BASKETS-1;
    return idx;
}

float HistogramBasketMaxCurrent(int basketIndex)
{
    return powf(10.f, basketIndex/(float)HISTOGRAMM_BASKET_PER_DECADE)*HISTOGRAMM_MIN_CURRENT;
}
