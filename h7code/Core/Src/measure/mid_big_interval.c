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
static int32_t samples_V_count = 0;
static int64_t adc_V_result = 0;
static RESISTOR r_last = RESISTOR_1_Kom;
static int32_t samples_current_count = 0;
static double current_result = 0;
static float current_min = CURRENT_MAX;
static float current_max =-CURRENT_MAX;

static volatile bool adc_store_request = false;
static volatile int32_t samples_V_count_stored = 0;
static volatile int64_t adc_V_result_stored = 0;
static RESISTOR r_last_stored = RESISTOR_1_Kom;
static int32_t samples_current_count_stored = 0;
static volatile double current_result_stored = 0;
static volatile float current_min_stored = 0;
static volatile float current_max_stored = 0;

static void OnFilterNextSample(float current);

void ReceiveDataFunc_Mid(DataNoError* data)
{
    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        adc_V_result += data[i].adc_V;

        float current = calculateCurrent(data[i].adc_I, GetResistorValueInv(data[i].r));
        FilterNextSample(current, OnFilterNextSample);
    }

    samples_V_count += ADS1271_RECEIVE_DATA_SIZE;

    if(adc_store_request)
    {
        samples_V_count_stored = samples_V_count;
        adc_V_result_stored = adc_V_result;
        r_last_stored = r_last;

        samples_current_count_stored = samples_current_count;
        current_result_stored = current_result;
        current_min_stored = current_min;
        current_max_stored = current_max;

        current_result = 0;
        adc_V_result = 0;
        samples_V_count = 0;
        samples_current_count = 0;
        current_min = CURRENT_MAX;
        current_max =-CURRENT_MAX;

        adc_store_request = false;
    }

}

void OnFilterNextSample(float current)
{
    samples_current_count++;
    current_result += current;
    current_min = fminf(current_min, current);
    current_max = fmaxf(current_max, current);
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
    d.samples_V_count = samples_V_count_stored;
    d.samples_current_count = samples_current_count_stored;

    if(samples_V_count_stored==0)
    {
        d.adc_V = 100;
    } else
    {
        d.adc_V = adc_V_result_stored/samples_V_count_stored;
    }

    if(samples_current_count_stored==0)
    {
        d.current = 0;
        d.current_min = 0;
        d.current_max = 0;
    } else
    {
        d.current = current_result_stored/samples_current_count_stored;
        d.current_min = current_min_stored;
        d.current_max = current_max_stored;
    }

    return d;
}
