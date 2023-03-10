#pragma once

//Точность +-1 uS
void DelayInit();
void DelayDeinit();

#define DELAY_US_MAX 0xFFFFF
void DelayUs(uint16_t countUs);

__STATIC_FORCEINLINE
uint16_t TimeUs() { return TIM15->CNT; }

void DelayMs(uint32_t ms);
uint32_t TimeMs();
