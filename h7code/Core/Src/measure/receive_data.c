#include "main.h"
#include "receive_data.h"

#include "hardware/ADS1271_input.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "calculate.h"

//SAMPLES_R - структуры, для учета сдвига данных происходящих при переключении предела измерения.
//На столько сэмплов запаздывают данные относительно
#define SAMPLES_R_OFFSET (38+9)
//Размер буфера должен быть степенью двойки и больше SAMPLES_R_OFFSET
#define SAMPLES_R_OFFSET_SIZE 64
#define SAMPLES_R_OFFSET_MASK (SAMPLES_R_OFFSET_SIZE-1)
//Столько сэмплов до переключения невалидные (естественно уже после сдвига на R_OFFSET)
#define SAMPLES_R_ERROR 9
static uint8_t samples_r_buffer[SAMPLES_R_OFFSET_SIZE];
static int samples_r_buffer_offset = 0;
static uint32_t g_prev_adc0 = 0; //Предыдущее значение в adc0 используемое для невалидных данных

#define BIG_BUFFER_SIZE 0x4000 //16k*8 bytes
//Большой буфер, для того, чтобы потом пересылать из него данные в
static volatile ADS1271_Data big_buf[BIG_BUFFER_SIZE] __attribute__((section(".d2_data"))) __attribute__ ((aligned (4)));
static volatile uint32_t big_buf_current_offset = 0; //Текущее записываемое положение в big_buf
static volatile uint32_t big_buf_required = 0; //требуемое количество сэмплов

#define USB_PACKET_SIZE_INTS 500
static int32_t usb_send_buffer[USB_PACKET_SIZE_INTS];

static int32_t I_max = 0x400000;
static int32_t I_min = 0x40000;//I_max/16
static const int32_t wait_before_new_switch = 3;
static int32_t wait_before_new_switch_count = 0;

//Временно захардкодили, после превышения этого порога мы начинаем заполнять большой буффер
static bool capturing_enable = false;
static int32_t capturing_I_start = 150000;
static RESISTOR capturing_r = RESISTOR_1_Kom;

//Статистика по используемому времени
static const uint16_t max_interrupt_to_stats = 1000;
static uint16_t prev_end_time_us = 0;  //Время на конец прерывания
static uint16_t count_interrupt = 0; //Количество прошедших прерываний
static uint32_t sum_time_us = 0;  //Время суммарное прошедшее
static uint32_t sum_interrupt_time_us = 0; //Время суммарно проведённое в прерывании
static float percent_in_interrupt = 0; //Процент времени, проведённого в прерывании

static ReceiveDataFunc data_func[RECEIVE_DATA_FUNC_COUNT] = {};

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
    uint16_t start_time_us = TimeUs();

    //Напряжение канала тока слишком маленькое, желательно увеличить номинал шунтирующего резистора
    bool switch_up = false;
    //Напряжение канала тока слишком большое, желательно уменьшить номинал шунтирующего резистора
    bool switch_down = false;

    RESISTOR r = GetResistor();

    //Сразу после переключения резистора SAMPLES_R_ERROR сэмплов имеют невалидное значение
    //из-за переходных процессов.
    //Заменяем их старыми сэмплами
    static DataNoError data_no_error[ADS1271_RECEIVE_DATA_SIZE];

    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        uint8_t cur_r = ADS1271_GetSamplesR(i);
        uint8_t next_r = ADS1271_GetSamplesR(i+SAMPLES_R_ERROR);

        if(cur_r!=next_r)
        {
            //invalid samples
        } else
        {
            g_prev_adc0 = data[i].adc0;
        }

        data_no_error[i].adc_I = Convert24(g_prev_adc0);
        data_no_error[i].adc_V = Convert24(data[i].adc1);
        data_no_error[i].r = cur_r;
    }

    for(int i=0; i<RECEIVE_DATA_FUNC_COUNT; i++)
    {
        if(data_func[i])
            data_func[i](data_no_error);
    }

    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        int32_t adc_I = data_no_error[i].adc_I;
        if(adc_I<I_min)
            switch_up = true;
        if(adc_I>I_max)
            switch_down = true;
    }

    if(capturing_enable)
    {
        if(GetResistor()==capturing_r)
        {
            for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
            {
                int32_t adc_I = data_no_error[i].adc_I;
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
            ptr[i].adc0 = data_no_error[i].adc_I | (((uint32_t)data_no_error[i].r)<<24);
            ptr[i].adc1 = data_no_error[i].adc_V;
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


    uint16_t cur_end_time_us = TimeUs();
    sum_time_us += (uint16_t)(cur_end_time_us-prev_end_time_us);
    sum_interrupt_time_us += (uint16_t)(cur_end_time_us-start_time_us);
    prev_end_time_us = cur_end_time_us;
    count_interrupt++;

    if(count_interrupt >=max_interrupt_to_stats )
    {
        percent_in_interrupt = (100.0f*sum_interrupt_time_us)/sum_time_us;

        count_interrupt = 0;
        sum_time_us = 0;
        sum_interrupt_time_us = 0;
    }
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
            int32_t current_na = (int32_t)(result.current*1e9f);

            usb_send_buffer[i*2] = current_na;
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

float GetPercentInInterrupt()
{
    return percent_in_interrupt;
}

void SetReceiveDataFunc(int idx, ReceiveDataFunc fn)
{
    if(idx>=0 && idx<RECEIVE_DATA_FUNC_COUNT)
        data_func[idx] = fn;
}

bool ContainsReceiveDataFunc(ReceiveDataFunc fn)
{
    for(int i=0; i<RECEIVE_DATA_FUNC_COUNT; i++)
        if(data_func[i] == fn)
            return true;
    return false;
}
