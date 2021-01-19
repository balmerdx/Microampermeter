#pragma once

typedef struct
{
    int32_t adc0_mid;
    int32_t adc1_mid;
    uint32_t samples_count;
} MidData;

MidData GetMidData();

void StartAdcBufferFilling();
bool SendAdcBuffer();
bool IsAdcBufferFull();
