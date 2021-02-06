#include "main.h"
#include "mid_big_interval.h"
#include "hardware/ADS1271_input.h"
#include "my_filter.h"

#include <limits.h>
#include <sys/param.h>


#define INT24_MAX 0xFFFFFF
//adc0 - I, adc1 - V
static int64_t adc_I_result = 0;
static int64_t adc_V_result = 0;
static int32_t samples_count = 0;
static RESISTOR r_last = RESISTOR_1_Kom;
static int32_t adc_I_min = INT24_MAX;
static int32_t adc_I_max =-INT24_MAX;

static volatile bool adc_store_request = false;
static volatile int64_t adc_I_result_stored = 0;
static volatile int64_t adc_V_result_stored = 0;
static volatile int32_t samples_count_stored = 0;
static RESISTOR r_last_stored = RESISTOR_1_Kom;
static volatile int32_t adc_I_min_stored = 0;
static volatile int32_t adc_I_max_stored = 0;

void ReceiveDataFunc_Mid(DataNoError* data)
{
    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        if(data[i].r != r_last)
        {
            r_last = data[i].r;
            adc_I_result = 0;
            adc_V_result = 0;
            samples_count = -i;
        }

        adc_I_result += data[i].adc_I;
        adc_V_result += data[i].adc_V;

        int32_t adc_I_x4 = SosFilterProcess_x4_0(data[i].adc_I);
        if((i&3)==3)
        {
            int32_t adc_I_x16 = SosFilterProcess_x4_1(adc_I_x4);

            if((i&15)==15)
            {
                adc_I_min = MIN(adc_I_min, adc_I_x16);
                adc_I_max = MAX(adc_I_max, adc_I_x16);
            }
        }
    }

    samples_count += ADS1271_RECEIVE_DATA_SIZE;

    if(adc_store_request)
    {
        adc_I_result_stored = adc_I_result;
        adc_V_result_stored = adc_V_result;
        samples_count_stored = samples_count;
        r_last_stored = r_last;
        adc_I_min_stored = adc_I_min;
        adc_I_max_stored = adc_I_max;

        adc_I_result = 0;
        adc_V_result = 0;
        samples_count = 0;
        adc_I_min = INT24_MAX;
        adc_I_max =-INT24_MAX;

        adc_store_request = false;
    }

}

MidData GetMidData()
{
    MidData d;

    //Чтобы не зависло
    if(ContainsReceiveDataFunc(ReceiveDataFunc_Mid))
    {
        adc_store_request = true;
        while(adc_store_request);
    }

    d.r = r_last_stored;
    if(samples_count_stored==0)
    {
        d.adc_I = 0;
        d.adc_V = 100;
    } else
    {
        d.samples_count = samples_count_stored;
        d.adc_I = adc_I_result_stored/samples_count_stored;
        d.adc_V = adc_V_result_stored/samples_count_stored;

        d.adc_I_min = adc_I_min_stored;
        d.adc_I_max = adc_I_max_stored;
    }

    return d;
}
