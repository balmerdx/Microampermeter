#pragma once
#include "receive_data.h"

//Постоянно суммируем данные.
//Если переключился диапазон - старые данные выкидываем.
//Если запросили данные - начинаем суммировать заново.

typedef struct
{
    int32_t adc_I;
    int32_t adc_V;
    RESISTOR r;
    uint32_t samples_count;

    int32_t adc_I_min;
    int32_t adc_I_max;
} MidData;

MidData GetMidData();

void ReceiveDataFunc_Mid(DataNoError* data);
