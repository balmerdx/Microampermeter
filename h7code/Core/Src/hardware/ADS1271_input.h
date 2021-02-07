#pragma once

void ADS1271_Init();
void ADS1271_Start();

//Количество сэмплов, которые мы получаем в функции ADS1271_ReceiveData
#define ADS1271_RECEIVE_DATA_SIZE 16

typedef struct
{
    //Младшие 24 бита - данные. Старшие 8 бит - пустые
    uint32_t adc0;
    uint32_t adc1;
} ADS1271_Data;
//callback from interrupt
//Тут надо быстро перекинуть данные и выйти
//Четный uint32_t - ADC0
//Нечетный uint32_t - ADC1
//Данные всегда размером ADS1271_RECEIVE_DATA_SIZE
void ADS1271_ReceiveData(ADS1271_Data* data);
void ADS1271_ReceiveDataInit();

//Преобразуем наши 24-х битные данные в 32-х битные
__STATIC_FORCEINLINE
int32_t Convert24(uint32_t data24)
{
    if(data24&0x800000u)
    {
        data24 |= 0xFF000000u;
    }

    return (int32_t)data24;
}

/*
__STATIC_FORCEINLINE
int32_t Convert24(uint32_t data24)
{
    int32_t d = data24;
    if(d < 0x800000)
        return d;
    return d-0x1000000;
}
*/
