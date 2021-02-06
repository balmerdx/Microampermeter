#pragma once
#include <stdint.h>

typedef struct FilterData
{
    //n_sections - количество sos секций фильтра
    int n_sections;
    //coeff - коэффициенты их количество n_sections*6
    const float* coeff;

    //zi - временные переменные, их надо выделять либо динамически либо статически
    //zi - по количеству равны n_sections*2
    float* zi;
} FilterData;


void SosFilterInit(FilterData* data);

int32_t SosFilterProcess(FilterData* data, int32_t newX);

