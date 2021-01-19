#include "main.h"
#include "receive_data.h"

#include "hardware/ADS1271_input.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#define BIG_BUFFER_SIZE 0x8000 //32768
//Большой буфер, для того, чтобы потом пересылать из него данные в
static volatile uint32_t big_buf[BIG_BUFFER_SIZE] __attribute__((section(".d2_data"))) __attribute__ ((aligned (4)));
static volatile uint32_t big_buf_current_offset = 0; //Текузие записываемое положение в big_buf
static volatile uint32_t big_buf_required = 0; //требуемое количество сэмплов

//adc0 - I, adc1 - V
static uint64_t adc0_result = 0;
static uint64_t adc1_result = 0;
static uint32_t samples_count = 0;

static volatile bool adc_store_request = false;
static volatile int64_t adc0_result_stored = 0;
static volatile int64_t adc1_result_stored = 0;
static volatile uint32_t samples_count_stored = 0;

void ADS1271_ReceiveData(uint32_t* data, int size)
{
    for(int i=0; i<size; i+=2)
    {
        adc0_result += Convert24(data[i]);
        adc1_result += Convert24(data[i+1]);
    }

    samples_count += size/2;

    if(adc_store_request)
    {
        adc0_result_stored = adc0_result;
        adc1_result_stored = adc1_result;
        samples_count_stored = samples_count;

        adc0_result = 0;
        adc1_result = 0;
        samples_count = 0;

        adc_store_request = false;
    }

    if(big_buf_current_offset < big_buf_required)
    {
        int size_to_write = big_buf_required - big_buf_current_offset;
        if(size_to_write > size)
            size_to_write = size;

        volatile uint32_t* ptr = big_buf + big_buf_current_offset;
        for(int i=0; i<size; i++)
        {
            //ptr[i] = Convert24(data[i]);
            ptr[i] = data[i];
        }

        big_buf_current_offset += size_to_write;
    }
}

MidData GetMidData()
{
    MidData d;
    adc_store_request = true;
    while(adc_store_request);

    d.samples_count = samples_count_stored;

    d.adc0_mid = adc0_result_stored/samples_count_stored;
    d.adc1_mid = adc1_result_stored/samples_count_stored;

    return d;
}

bool SendAdcBuffer()
{
    const uint32_t packet_size_ints = 500;

    for(uint32_t pos = 0; pos<big_buf_required; pos += packet_size_ints)
    {
        uint32_t size = packet_size_ints;
        if(pos+size > big_buf_required)
            size = big_buf_required-pos;
        uint8_t result;
        for(int i=0; i<1000; i++)
        {
            result = CDC_Transmit_FS((uint8_t*)(big_buf+pos), size*sizeof(uint32_t));
            if(result!=USBD_BUSY)
                break;
            HAL_Delay(1);
        }

        if(result!=USBD_OK)
            return false;
    }

    return true;
}

void StartAdcBufferFilling()
{
    big_buf_current_offset = 0;
    big_buf_required = BIG_BUFFER_SIZE;
}

bool IsAdcBufferFull()
{
    return big_buf_current_offset == big_buf_required && big_buf_required>0;
}
