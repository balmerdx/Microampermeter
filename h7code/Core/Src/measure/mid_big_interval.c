#include "main.h"
#include "mid_big_interval.h"
#include "hardware/ADS1271_input.h"

//adc0 - I, adc1 - V
static int64_t adc_I_result = 0;
static int64_t adc_V_result = 0;
static int32_t samples_count = 0;
static RESISTOR r_last = RESISTOR_1_Kom;

static volatile bool adc_store_request = false;
static volatile int64_t adc_I_result_stored = 0;
static volatile int64_t adc_V_result_stored = 0;
static volatile int32_t samples_count_stored = 0;
static RESISTOR r_last_stored = RESISTOR_1_Kom;

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
    }

    samples_count += ADS1271_RECEIVE_DATA_SIZE;

    if(adc_store_request)
    {
        adc_I_result_stored = adc_I_result;
        adc_V_result_stored = adc_V_result;
        samples_count_stored = samples_count;
        r_last_stored = r_last;

        adc_I_result = 0;
        adc_V_result = 0;
        samples_count = 0;

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
    }

    return d;
}
