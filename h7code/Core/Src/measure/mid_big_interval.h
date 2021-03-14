#pragma once
#include "receive_data.h"

//Постоянно суммируем данные.
//Если переключился диапазон - старые данные выкидываем.
//Если запросили данные - начинаем суммировать заново.

typedef struct
{
    float voltage;
    uint32_t samples_current_count;

    float current;
    float current_min;
    float current_max;
} MidData;


MidData GetMidData();

void ReceiveDataFunc_Mid(DataNoError* data);
