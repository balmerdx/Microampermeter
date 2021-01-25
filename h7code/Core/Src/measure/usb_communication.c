#include "main.h"
#include "usb_communication.h"
#include "receive_data.h"
#include "UTFT.h"
#include "fonts/font_condensed30.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

//Начинает заполнение буфера ADC
#define USB_COMMAND_ADC_BUFFER_START 0x6BDE0001u
//Получает весь ADC буфер
#define USB_COMMAND_ADC_BUFFER_GET 0x6BDE0002u
//Проверяет, насколько буфер заполнен
#define USB_COMMAND_ADC_BUFFER_STATUS 0x6BDE0003u

#define USB_COMMAND_NANOAMPERS 0x6BDE0004u


#define USB_RECEIVED_DATA_SIZE 32
//Количество слов в буфере g_usb_received_data
static volatile uint32_t g_usb_received_words = 0;
//Смещение в буфере, до которого дочитали
static volatile uint32_t g_usb_received_offset = 0;
static uint32_t g_usb_received_data[USB_RECEIVED_DATA_SIZE];

static uint32_t last_command = 0;

void OnReceiveUsbData(uint8_t* Buf, uint32_t Len)
{
    const uint32_t sz = sizeof(g_usb_received_data[0]);
    while(g_usb_received_words < USB_RECEIVED_DATA_SIZE && Len>=sz)
    {
        memcpy(g_usb_received_data+g_usb_received_words, Buf, sz);

        Buf += sz;
        Len -= sz;
        g_usb_received_words++;
    }
}
\
bool IsUsbCommand()
{
    if(g_usb_received_offset<g_usb_received_words)
        return true;
    return last_command!=0;
}

void UsbCommandsQuant()
{
    if(!IsUsbCommand())
        return;

    if(g_usb_received_offset < g_usb_received_words)
    {
        uint32_t command = g_usb_received_data[g_usb_received_offset];
        g_usb_received_offset++;

        /*
        UTF_SetFont(font_condensed30);
        int x, y;
        int xstart = 300;
        int yoffset = 30;
        y = 0;
        UTF_printNumI(command, xstart, y, 130, UTF_RIGHT);
        y += yoffset;
        */

        if(command == USB_COMMAND_ADC_BUFFER_START)
        {
            StartAdcBufferFilling();
            static uint32_t result = 0x12345;
            CDC_Transmit_FS((uint8_t*)(&result), sizeof(result));
            return;
        }

        if(command == USB_COMMAND_ADC_BUFFER_GET)
        {
            SendAdcBuffer();
            return;
        }

        if(command == USB_COMMAND_ADC_BUFFER_STATUS)
        {
            static uint32_t result;
            result = IsAdcBufferFull()?1:0;
            CDC_Transmit_FS((uint8_t*)(&result), sizeof(result));
            return;
        }

        if(command == USB_COMMAND_NANOAMPERS)
        {
            SendAdcCurrentNanoampers();
        }
    }

    g_usb_received_offset = g_usb_received_words = 0;
    last_command = 0;
}
