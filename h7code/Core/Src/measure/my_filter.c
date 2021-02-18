#include "main.h"
#include "my_filter.h"
#include "iir_filter.h"
#include "hardware/ADS1271_input.h"

FilterX g_filterX = FilterX_16;

//fmax = 0.25 Т.е. фильтр в 4 раза частоту уменьшает максимальную
//signal.cheby2(16, 70, fmax, 'low', analog=False, output="sos")
#define coeff_sos_x4_sections 8
const float coeff_sos_x4[]={
    0.001282070078055012, 0.002292205228482419, 0.001282070078055012, 1.0, -0.34166847751339063, 0.03821970324808857,
    1.0, 0.6825229858195228, 1.0000000000000002, 1.0, -0.4664894834231138, 0.12529535331799338,
    1.0, -0.25720458349397307, 0.9999999999999998, 1.0, -0.6659807096858221, 0.26591023949407566,
    1.0, -0.8044279326878758, 1.0, 1.0, -0.8820990647916817, 0.4214917656960164,
    1.0, -1.1076902781388334, 1.0, 1.0, -1.0792584857522882, 0.5689644539094694,
    1.0, -1.2770993587568327, 0.9999999999999998, 1.0, -1.2452794241061382, 0.7015680454138651,
    1.0, -1.368815733805565, 1.0, 1.0, -1.381216713379076, 0.8223360479588602,
    1.0, -1.4093701647021322, 1.0, 1.0, -1.4933941303028764, 0.9393521857339246,
    };

float zi_0[coeff_sos_x4_sections*2];
FilterData data0 = {
    .n_sections = coeff_sos_x4_sections,
    .coeff = coeff_sos_x4,
    .zi = zi_0
};

float zi_1[coeff_sos_x4_sections*2];
FilterData data1 = {
    .n_sections = coeff_sos_x4_sections,
    .coeff = coeff_sos_x4,
    .zi = zi_1
};

float zi_2[coeff_sos_x4_sections*2];
FilterData data2 = {
    .n_sections = coeff_sos_x4_sections,
    .coeff = coeff_sos_x4,
    .zi = zi_2
};

float zi_3[coeff_sos_x4_sections*2];
FilterData data3 = {
    .n_sections = coeff_sos_x4_sections,
    .coeff = coeff_sos_x4,
    .zi = zi_3
};

float zi_4[coeff_sos_x4_sections*2];
FilterData data4 = {
    .n_sections = coeff_sos_x4_sections,
    .coeff = coeff_sos_x4,
    .zi = zi_4
};

static int current_index;

void MyFilterInit()
{
    SosFilterInit(&data0);
    SosFilterInit(&data1);
    SosFilterInit(&data2);
    SosFilterInit(&data3);
    SosFilterInit(&data4);

    current_index = 0;
}

float SosFilterProcess_x4_0(float newX)
{
    return SosFilterProcess(&data0, newX);
}


float SosFilterProcess_x4_1(float newX)
{
    return SosFilterProcess(&data1, newX);
}

void FilterNextSample(float sample, FilterNextSampleCallback callback)
{
    if(g_filterX==FilterX_1)
        callback(sample);
    current_index++;

    float sample1 = SosFilterProcess(&data0, sample);
    if((current_index&(4-1))==0)
    {
        if(g_filterX==FilterX_4)
            callback(sample1);

        float sample2 = SosFilterProcess(&data1, sample1);
        if((current_index&(16-1))==0)
        {
            if(g_filterX==FilterX_16)
                callback(sample2);
            float sample3 = SosFilterProcess(&data2, sample2);
            if((current_index&(64-1))==0)
            {
                if(g_filterX==FilterX_64)
                    callback(sample3);
                float sample4 = SosFilterProcess(&data3, sample3);
                if((current_index&(256-1))==0)
                {
                    if(g_filterX==FilterX_256)
                        callback(sample4);
                    float sample5 = SosFilterProcess(&data4, sample4);
                    if((current_index&(1024-1))==0)
                    {
                        if(g_filterX==FilterX_1024)
                            callback(sample5);
                    }
                }
            }
        }
    }
}

float FilterSPS()
{
    float f = 1.0f;
    switch(g_filterX)
    {
    case FilterX_1: f = 1.0f; break;
    case FilterX_4: f = 1.0f/4; break;
    case FilterX_16: f = 1.0f/16; break;
    case FilterX_64: f = 1.0f/64; break;
    case FilterX_256: f = 1.0f/256; break;
    case FilterX_1024: f = 1.0f/1024; break;
    }

    return ADS1271_SPS * f;
}
