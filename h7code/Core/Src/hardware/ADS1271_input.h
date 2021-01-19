#pragma once

void ADS1271_Init();
void ADS1271_Start();

//callback from interrupt
//Тут надо быстро перекинуть данные и выйти
//Четный uint32_t - ADC0
//Нечетный uint32_t - ADC1
//size - гарантированно кратно двум
void ADS1271_ReceiveData(uint32_t* data, int size);

//Преобразуем наши 24-х битные данные в 32-х битные
/*
static inline int32_t Convert24(uint32_t data24)
{
    if(data24&0x800000u)
    {
        data24 |= 0xFF000000u;
    }

    return (int32_t)data24;
}
*/

static inline int32_t Convert24(uint32_t data24)
{
    int32_t d = data24;
    if(d < 0x800000)
        return d;
    return d-0x1000000;
}
