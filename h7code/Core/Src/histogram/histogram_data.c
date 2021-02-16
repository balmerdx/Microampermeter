#include "main.h"
#include "histogram_data.h"
#include <math.h>

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

void HistogramDataClear(HistogramData* data)
{
    for(int i=0; i<HISTOGRAMM_BASKETS; i++)
        data->data[i] = 0.f;
}
