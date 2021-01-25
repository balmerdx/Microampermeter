#include "main.h"
#include "receive_data.h"

#include "hardware/ADS1271_input.h"
#include "hardware/gpios.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "calculate.h"

#define BIG_BUFFER_SIZE 0x8000 //32768
//Большой буфер, для того, чтобы потом пересылать из него данные в
static volatile uint32_t big_buf[BIG_BUFFER_SIZE] __attribute__((section(".d2_data"))) __attribute__ ((aligned (4)));
static volatile uint32_t big_buf_current_offset = 0; //Текущее записываемое положение в big_buf
static volatile uint32_t big_buf_required = 0; //требуемое количество сэмплов

#define USB_PACKET_SIZE_INTS 500
static int32_t usb_send_buffer[USB_PACKET_SIZE_INTS];

//adc0 - I, adc1 - V
static uint64_t adc0_result = 0;
static uint64_t adc1_result = 0;
static uint32_t samples_count = 0;

static volatile bool adc_store_request = false;
static volatile int64_t adc0_result_stored = 0;
static volatile int64_t adc1_result_stored = 0;
static volatile uint32_t samples_count_stored = 0;

static int32_t I_max = 0x400000;
static int32_t I_min = 0x40000;//I_max/16
static const int32_t wait_before_new_switch = 3;
static int32_t wait_before_new_switch_count = 0;

//Временно захардкодили, после превышения этого порога мы начинаем заполнять большой буффер
static bool capturing_enable = false;
static int32_t capturing_I_start = 150000;
static RESISTOR capturing_r = RESISTOR_1_Kom;

static void SwitchUpResistor();
static void SwitchDownResistor();

void ADS1271_ReceiveData(uint32_t* data, int size)
{
    //Напряжение канала тока слишком маленькое, желательно увеличить номинал шунтирующего резистора
    bool switch_up = false;
    //Напряжение канала тока слишком большое, желательно уменьшить номинал шунтирующего резистора
    bool switch_down = false;
    for(int i=0; i<size; i+=2)
    {
        int32_t adc_I = Convert24(data[i]);
        if(adc_I<I_min)
            switch_up = true;
        if(adc_I>I_max)
            switch_down = true;

        adc0_result += adc_I;
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

    if(capturing_enable)
    {
        if(GetResistor()==capturing_r)
        {
            for(int i=0; i<size; i+=2)
            {
                int32_t adc_I = Convert24(data[i]);
                if(adc_I > capturing_I_start)
                {
                    StartAdcBufferFilling();
                    capturing_enable = false;
                }
            }
        }
    }

    if(big_buf_current_offset < big_buf_required)
    {
        int size_to_write = big_buf_required - big_buf_current_offset;
        if(size_to_write > size)
            size_to_write = size;

        RESISTOR r = GetResistor();
        volatile uint32_t* ptr = big_buf + big_buf_current_offset;
        for(int i=0; i<size; i+=2)
        {
            //ptr[i] = Convert24(data[i]);
            ptr[i] = data[i] | (((uint32_t)r)<<24);
            ptr[i+1] = data[i+1];
        }

        big_buf_current_offset += size_to_write;
    }

    if(wait_before_new_switch_count>0)
        wait_before_new_switch_count--;
    else
    if(switch_up)
        SwitchUpResistor();
    else
    if(switch_down)
        SwitchDownResistor();
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
    for(uint32_t pos = 0; pos<big_buf_required; pos += USB_PACKET_SIZE_INTS)
    {
        while(!CDC_IsTransmitComplete())
            HAL_Delay(1);

        uint32_t size = USB_PACKET_SIZE_INTS;
        if(pos+size > big_buf_required)
            size = big_buf_required-pos;
        uint8_t result = CDC_Transmit_FS((uint8_t*)(big_buf+pos), size*sizeof(uint32_t));
        if(result!=USBD_OK)
            return false;
    }

    return true;
}

bool SendAdcCurrentNanoampers()
{
    for(uint32_t pos = 0; pos<big_buf_required; pos += USB_PACKET_SIZE_INTS)
    {
        while(!CDC_IsTransmitComplete())
            HAL_Delay(1);

        uint32_t size = USB_PACKET_SIZE_INTS;
        if(pos+size > big_buf_required)
            size = big_buf_required-pos;

        //Transform current
        volatile uint32_t* in_data = big_buf+pos;
        for(int i=0; i<size; i+=2)
        {
            RESISTOR r = (RESISTOR)(in_data[i]>>24);
            int32_t adc_I = Convert24(in_data[i]&0xFFFFFF);
            int32_t adc_V = Convert24(in_data[i+1]);

            CalcResult result;
            calculate(adc_V, adc_I,
                           GetResistorValue(r), &result);

            usb_send_buffer[i] = (int)(result.current*1e9f);
            usb_send_buffer[i+1] = r;
        }


        uint8_t result = CDC_Transmit_FS((uint8_t*)(usb_send_buffer), size*sizeof(uint32_t));
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

void SwitchUpResistor()
{
    //return;
    RESISTOR r = GetResistor();
    if(r==RESISTOR_1_Om)
    {
        SetResistor(RESISTOR_10_Om);
    } else
    if(r==RESISTOR_10_Om)
    {
        SetResistor(RESISTOR_100_Om);
    } else
    if(r==RESISTOR_100_Om)
    {
        SetResistor(RESISTOR_1_Kom);
    }

    wait_before_new_switch_count = wait_before_new_switch;
}

void SwitchDownResistor()
{
    //return;
    RESISTOR r = GetResistor();
    if(r==RESISTOR_1_Kom)
    {
        SetResistor(RESISTOR_100_Om);
    } else
    if(r==RESISTOR_100_Om)
    {
        SetResistor(RESISTOR_10_Om);
    } else
    if(r==RESISTOR_10_Om)
    {
        SetResistor(RESISTOR_1_Om);
    }

    wait_before_new_switch_count = wait_before_new_switch;
}

void EnableCapturingTrigger()
{
    capturing_enable = true;
}

bool IsEnabledCapturingTrigger()
{
    return capturing_enable;
}
