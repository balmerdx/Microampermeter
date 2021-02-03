#pragma once
#include "hardware/gpios.h"

typedef struct
{
    int32_t adc_I;
    int32_t adc_V;
    RESISTOR r;
} DataNoError;


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
