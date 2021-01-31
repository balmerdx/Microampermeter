#include "main.h"
#include "receive_data.h"

#include "hardware/ADS1271_input.h"
#include "hardware/gpios.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "calculate.h"

//SAMPLES_R - структуры, для учета сдвига данных происходящих при переключении предела измерения.
//На столько сэмплов запаздывают данные относительно
#define SAMPLES_R_OFFSET 38
//Размер буфера должен быть степенью двойки и больше SAMPLES_R_OFFSET
#define SAMPLES_R_OFFSET_SIZE 64
#define SAMPLES_R_OFFSET_MASK (SAMPLES_R_OFFSET_SIZE-1)
//Столько сэмплов после переключения невалидные (естественно уже после сдвига на R_OFFSET)
#define SAMPLES_R_ERROR 9
static uint8_t samples_r_buffer[SAMPLES_R_OFFSET_SIZE];
static int samples_r_buffer_offset = 0;

#define BIG_BUFFER_SIZE 0x4000 //16k*8 bytes
//Большой буфер, для того, чтобы потом пересылать из него данные в
static volatile ADS1271_Data big_buf[BIG_BUFFER_SIZE] __attribute__((section(".d2_data"))) __attribute__ ((aligned (4)));
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

void ADS1271_ReceiveDataInit()
{
    for(int i=0; i<SAMPLES_R_OFFSET_SIZE; i++)
        samples_r_buffer[i] = GetResistor();
}

void ADS1271_FillSamplesR(uint8_t r)
{
    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        samples_r_buffer[(samples_r_buffer_offset+i)&SAMPLES_R_OFFSET_MASK] = r;
    }

    samples_r_buffer_offset = (samples_r_buffer_offset+ADS1271_RECEIVE_DATA_SIZE)&SAMPLES_R_OFFSET_MASK;
}

//Использовать только внутри ADS1271_ReceiveData
//Получает старое значение, смещённое на SAMPLES_R_OFFSET сэмплов назад
__STATIC_FORCEINLINE
uint8_t ADS1271_GetSamplesR(int i)
{
    return samples_r_buffer[(samples_r_buffer_offset+i+(SAMPLES_R_OFFSET_SIZE-SAMPLES_R_OFFSET))&SAMPLES_R_OFFSET_MASK];
}

/*
 *  Эта функция вызывается довольно часто, каждые 0.15 милисекунды.
 *  Желательно не проводить тут слишком много времени.
*/
void ADS1271_ReceiveData(ADS1271_Data *data)
{
    //Напряжение канала тока слишком маленькое, желательно увеличить номинал шунтирующего резистора
    bool switch_up = false;
    //Напряжение канала тока слишком большое, желательно уменьшить номинал шунтирующего резистора
    bool switch_down = false;

    RESISTOR r = GetResistor();

    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        int32_t adc_I = Convert24(data[i].adc0);
        if(adc_I<I_min)
            switch_up = true;
        if(adc_I>I_max)
            switch_down = true;

        adc0_result += adc_I;
        adc1_result += Convert24(data[i].adc1);
    }
    samples_count += ADS1271_RECEIVE_DATA_SIZE;

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
            for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
            {
                int32_t adc_I = Convert24(data[i].adc0);
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
        if(size_to_write > ADS1271_RECEIVE_DATA_SIZE)
            size_to_write = ADS1271_RECEIVE_DATA_SIZE;

        volatile ADS1271_Data* ptr = big_buf + big_buf_current_offset;
        for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
        {
            //ptr[i].adc0 = data[i].adc0 | (((uint32_t)r)<<24);
            ptr[i].adc0 = data[i].adc0 | (((uint32_t)ADS1271_GetSamplesR(i))<<24);
            ptr[i].adc1 = data[i].adc1;
        }

        big_buf_current_offset += size_to_write;
    }

    ADS1271_FillSamplesR((uint8_t)r);

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
    //Сколько сэмплов после промежутка r_offset считаются невалидными
    int r_invalid = 9;

    RESISTOR prev_r = (RESISTOR)(big_buf[0].adc0>>24);
    int invalid_samples = 0;
    int32_t last_current_na = 0;

    int size_8bytes = USB_PACKET_SIZE_INTS/2;
    for(int pos = 0; pos<big_buf_required; pos += size_8bytes)
    {
        while(!CDC_IsTransmitComplete())
            HAL_Delay(1);

        int size = size_8bytes;
        if(pos+size > big_buf_required)
            size = big_buf_required-pos;

        //Transform current
        for(int i=0; i<size; i++)
        {
            int buf_idx = i + pos;
            RESISTOR r = (RESISTOR)(big_buf[buf_idx].adc0>>24);
            int32_t adc_I = Convert24(big_buf[buf_idx].adc0&0xFFFFFF);
            int32_t adc_V = Convert24(big_buf[buf_idx].adc1);


            CalcResult result;
            calculate(adc_V, adc_I,
                           GetResistorValue(r), &result);
            int32_t current_na = (int)(result.current*1e9f);
            if(r!=prev_r)
            {
                prev_r = r;
                invalid_samples = r_invalid;
            }

            if(invalid_samples>0)
                invalid_samples--;
            else
                last_current_na = current_na;

            usb_send_buffer[i*2] = last_current_na;
            usb_send_buffer[i*2+1] = r;
        }


        uint8_t result = CDC_Transmit_FS((uint8_t*)(usb_send_buffer), size*sizeof(ADS1271_Data));
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
