#include "main.h"
#include "store_and_trigger.h"

#include "circle_buf.h"
#include "my_filter.h"
#include "settings.h"
#include "receive_data.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

//512 KB
static uint8_t store_buffer[0x80000] __attribute__((section(".d1_data"))) __attribute__ ((aligned (32)));

static CircleBuffer circle_buffer;
static FilterX cb_filterX;
static volatile bool cb_capture_started;
static volatile bool cb_capture_completed;
static volatile bool cb_trigger_triggered;
static bool cb_trigger_rising;
//До начала проверки условия триггера, проыеряем, что например для cb_trigger_rising значение меньше, чем cb_trigger_level_low
//и только потом проверяем, что выше, чем cb_trigger_level_high
static bool cb_trigger_before_start;
//Для некоторого гистерезиса два значения используем
static float cb_trigger_level_high;
static float cb_trigger_level_low;

static uint32_t cb_trigger_abs_value;

void STInit()
{
    memset(store_buffer, 0, sizeof(store_buffer));
    CircleBufferInit(&circle_buffer, store_buffer, sizeof(store_buffer), 4);

    cb_filterX = FilterX_1;
    cb_capture_started = false;
    cb_capture_completed = false;
    cb_trigger_triggered = false;
    cb_trigger_rising = true;
    cb_trigger_before_start = false;
    cb_trigger_level_high =
    cb_trigger_level_low = 1;
    cb_trigger_abs_value = 0;

}

void OnFilterNextSampleCircleBuffer(float current, float voltage)
{
    if(!cb_capture_started)
        return;

    if(cb_trigger_triggered)
    {
        if(CircleBufferSamples(&circle_buffer) == CircleBufferCapacity(&circle_buffer))
        {
            cb_capture_completed = true;
            cb_capture_started = false;
        }
    } else
    {
        if(cb_trigger_before_start)
        {
            bool triggered = (cb_trigger_rising && current < cb_trigger_level_low) ||
                             (!cb_trigger_rising && current > cb_trigger_level_high);
            if(triggered)
                cb_trigger_before_start = false;
        } else
        {
            bool triggered = (cb_trigger_rising && current > cb_trigger_level_high) ||
                             (!cb_trigger_rising && current < cb_trigger_level_low);
            if(triggered)
            {
                cb_trigger_triggered = true;
                cb_trigger_abs_value = CircleBufferGetAbsoluteEndOffset(&circle_buffer);

                //Количество сэмплов, которые мы запоминаем до того, как сработал триггер
                uint32_t preshoot_samples = CircleBufferCapacity(&circle_buffer)/4;
                if(circle_buffer.samples_stored >= preshoot_samples)
                {
                    circle_buffer.first_sample_offset = ((cb_trigger_abs_value + circle_buffer.data_size_in_samples - preshoot_samples)%circle_buffer.data_size_in_samples)
                                                *circle_buffer.sample_size;
                    circle_buffer.samples_stored = preshoot_samples;
                } else
                {
                    //В буфере слишком мало сэмплов, поэтому до триггера меньшее количество запомним
                }

                //first_sample_offset после этих операций не меняется
            }
        }
    }

    CircleBufferAdd(&circle_buffer, &current);
}

void STCaptureStart()
{
    cb_trigger_rising = g_settings.trigger_rising?true:false;
    float trigger_level = TriggerLevelAmpers();
    cb_trigger_level_high = trigger_level*1.05f;
    cb_trigger_level_low = trigger_level*0.95f;

    cb_trigger_before_start = true;
    cb_capture_completed = false;
    cb_trigger_triggered = false;

    cb_filterX = g_filterX;

    CircleBufferClear(&circle_buffer);

    //Последнее, т.к. это реально запускает процесс.
    cb_capture_started = true;
}

bool STCaptureStarted()
{
    return cb_capture_started;
}

bool STCaptureCompleted()
{
    return cb_capture_completed;
}

bool STTriggerTriggered()
{
    return cb_trigger_triggered;
}

uint32_t STSamplesCaptured()
{
    return CircleBufferSamples(&circle_buffer);
}

float STFilterSPS()
{
    return FilterSPSLocal(cb_filterX);
}


typedef struct {
    uint32_t status;
    float filter_sps;
    uint32_t trigger_offset;
    uint32_t samples;
} STSendInfoStruct;

void STSendInfo()
{
    static STSendInfoStruct s;
    s.status = 0;
    if(cb_capture_started)
        s.status |= 1;
    if(cb_capture_completed)
        s.status |= 2;
    if(cb_trigger_triggered)
        s.status |= 4;

    s.filter_sps = STFilterSPS();

    if(cb_capture_completed)
    {
        s.trigger_offset = CircleBufferGetRelativeOffset(&circle_buffer, cb_trigger_abs_value);
        s.samples = STSamplesCaptured();
    }
    else
    {
        s.trigger_offset = 0;
        s.samples = 0;
    }

    CDC_Transmit_FS((uint8_t*)(&s), sizeof(s));
}

void STSendData()
{
    if(!cb_capture_completed)
        return;

    uint32_t samples_captured = CircleBufferSamples(&circle_buffer);
    for(uint32_t offset = 0; offset<samples_captured;  )
    {
        while(!CDC_IsTransmitComplete())
            HAL_Delay(1);

        uint32_t data_to_end = samples_captured-offset;
        uint32_t samples_to_send = (data_to_end < USB_PACKET_SIZE_INTS)? data_to_end : USB_PACKET_SIZE_INTS;

        for(uint32_t sub_offset = 0; sub_offset<samples_to_send; sub_offset++)
            CircleBufferGetSample(&circle_buffer, offset+sub_offset, usb_send_buffer+sub_offset);
            //usb_send_buffer[sub_offset] = (offset+sub_offset)*0x1000+samples_to_send;

        uint8_t result = CDC_Transmit_FS((uint8_t*)(usb_send_buffer), samples_to_send*sizeof(float));
        if(result!=USBD_OK)
            return;

        offset += samples_to_send;
    }
}


bool STIterate(uint32_t istart, uint32_t iend, STIterateIntervalCallback callback, void* param)
{
    if(!cb_trigger_triggered)
        return false;
    if(iend > CircleBufferSamples(&circle_buffer))
        return false;

    for(uint32_t i = istart; i<iend; i++)
    {
        float sample;
        CircleBufferGetSample(&circle_buffer, i, &sample);
        callback(sample, param);
    }

    return true;
}

uint32_t STTriggerOffset()
{
    return CircleBufferGetRelativeOffset(&circle_buffer, cb_trigger_abs_value);
}

void STTestAdd(float current)
{
    CircleBufferAdd(&circle_buffer, &current);
}

uint32_t STBufferCapacity()
{
    return CircleBufferCapacity(&circle_buffer);
}

void STTestSetCaptureCompleted()
{
    cb_trigger_abs_value = STBufferCapacity()/4;
    cb_capture_completed = true;
}
