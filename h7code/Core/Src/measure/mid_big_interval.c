#include "main.h"
#include "mid_big_interval.h"
#include "hardware/ADS1271_input.h"
#include "hardware/gpios.h"
#include "my_filter.h"
#include "calculate.h"

#include <limits.h>
#include <sys/param.h>


#define CURRENT_MAX 1
//adc0 - I, adc1 - V
static double current_result = 0;
static int64_t adc_V_result = 0;
static int32_t samples_count = 0;
static RESISTOR r_last = RESISTOR_1_Kom;
static float current_min = CURRENT_MAX;
static float current_max =-CURRENT_MAX;

static volatile bool adc_store_request = false;
static volatile double current_result_stored = 0;
static volatile int64_t adc_V_result_stored = 0;
static volatile int32_t samples_count_stored = 0;
static RESISTOR r_last_stored = RESISTOR_1_Kom;
static volatile float current_min_stored = 0;
static volatile float current_max_stored = 0;

void ReceiveDataFunc_Mid(DataNoError* data)
{
    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        float current = calculateCurrent(data[i].adc_I, GetResistorValueInv(data[i].r));
        current_result += current;
        adc_V_result += data[i].adc_V;

        float current_x4 = SosFilterProcess_x4_0(current);
        if((i&3)==3)
        {
            float current_x16 = SosFilterProcess_x4_1(current_x4);

            if((i&15)==15)
            {
                current_min = fminf(current_min, current_x16);
                current_max = fmaxf(current_max, current_x16);
            }
        }
    }

    samples_count += ADS1271_RECEIVE_DATA_SIZE;

    if(adc_store_request)
    {
        current_result_stored = current_result;
        adc_V_result_stored = adc_V_result;
        samples_count_stored = samples_count;
        r_last_stored = r_last;
        current_min_stored = current_min;
        current_max_stored = current_max;

        current_result = 0;
        adc_V_result = 0;
        samples_count = 0;
        current_min = CURRENT_MAX;
        current_max =-CURRENT_MAX;

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
        d.current = 0;
        d.current_min = 0;
        d.current_max = 0;
        d.adc_V = 100;
    } else
    {
        d.samples_count = samples_count_stored;
        d.current = current_result_stored/samples_count_stored;
        d.adc_V = adc_V_result_stored/samples_count_stored;

        d.current_min = current_min_stored;
        d.current_max = current_max_stored;
    }

    return d;
}
