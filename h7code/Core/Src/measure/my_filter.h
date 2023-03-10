#pragma once

void MyFilterInit();

typedef enum
{
    FilterX_1 = 0, //27000000/256 = 105 KSPS
    FilterX_4, //26 KSPS
    FilterX_16, //6.6 KSPS
    FilterX_64, //1.6 KSPS
    FilterX_256, //412 SPS
    FilterX_1024, //103 SPS
} FilterX;

//Следующий сэмпл, получаемый от фильтра.
//В зависимости от частоты это может быть
typedef void (*FilterNextSampleCallback)(float current, float voltage);

extern FilterX g_filterX;
//В зависимости от g_filterX может вызываться не каждый квант.
void FilterNextSample(float sampleI, int32_t adcV, FilterNextSampleCallback callback);

//Количество сэмплов в секунду (вызовов функции FilterNextSampleCallback)
float FilterSPS();

float FilterSPSLocal(FilterX filter);
