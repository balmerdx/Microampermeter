#pragma once
#include "hardware/gpios.h"

typedef struct
{
    int32_t adc_I;
    int32_t adc_V;
    RESISTOR r;
} DataNoError;

#define USB_PACKET_SIZE_INTS 500
extern int32_t usb_send_buffer[USB_PACKET_SIZE_INTS];

void StartAdcBufferFilling();
bool SendAdcBuffer();
bool SendAdcCurrentNanoampers();
bool IsAdcBufferFull();

void EnableCapturingTrigger();
bool IsEnabledCapturingTrigger();

float GetPercentInInterrupt();

//Функция, которая обрабатывает данные прямо в обработчике прерывания
typedef void (*ReceiveDataFunc)(DataNoError* data);

enum
{
    RECEIVE_DATA_FUNC_COUNT = 3,
};

void SetReceiveDataFunc(int idx, ReceiveDataFunc fn);
bool ContainsReceiveDataFunc(ReceiveDataFunc fn);

//Разрешаем/запрещаем автоматическое переключение
//диапазонов измерения в прерывании.
void EnableAutoResistorSwitch(bool enable);
bool IsEnabledAutoResistorSwitch();
