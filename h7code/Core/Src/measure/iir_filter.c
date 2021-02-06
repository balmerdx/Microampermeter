#include "main.h"
#include "iir_filter.h"

#include <math.h>
#include <stdio.h>
#include <inttypes.h>

enum
{
    B0 = 0,
    B1 = 1,
    B2 = 2,
    //A0 = 3, always 1
    A1 = 4,
    A2 = 5,
};

void SosFilterInit(FilterData* data)
{
    for(int i=0; i<data->n_sections*2; i++)
    {
        data->zi[i] = 0;
    }
}

int32_t SosFilterProcess(FilterData* data, int32_t newX)
{
    float x = newX;
    float* zi0 = data->zi;
    float* zi1 = data->zi + data->n_sections;

    for(int i=0; i<data->n_sections; i++)
    {
        size_t offset = i*6;
        float x_n = x;

        float b0 = data->coeff[offset+B0];
        float b1 = data->coeff[offset+B1];
        float b2 = data->coeff[offset+B2];
        float a1 = data->coeff[offset+A1];
        float a2 = data->coeff[offset+A2];

        x = b0 * x_n + zi0[i];
        zi0[i] = (b1 * x_n - a1 * x + zi1[i]);
        zi1[i] = (b2 * x_n - a2 * x);
    }

    return lroundl(x);
}


