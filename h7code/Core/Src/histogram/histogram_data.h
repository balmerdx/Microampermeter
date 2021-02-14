#pragma once

//Пускай у нас будет буффер для гистограмм.
//Интервал от 100 nA до 100 mA
//Начинается с того, что меньше 10 nA и кончается тем, что меньше 100 mA
//Всего десятичных разрядов 7.
//На один десятичный разряд пускай будет цельных 8 значений.
//Всего значит 7*8=56 значений.
#define HISTOGRAMM_BASKETS 56
#define HISTOGRAMM_BASKET_PER_DECADE 8
#define HISTOGRAMM_MIN_CURRENT 10e-9f
#define HISTOGRAMM_MAX_CURRENT 100e-3f
struct HistogramData
{
    float data[HISTOGRAMM_BASKETS];
};

int HistogramCurrentToBasketIndex(float current);
float HistogramBasketMaxCurrent(int basketIndex);


typedef enum
{
    TEST_LOG2_NORMAL,
    TEST_LOG2_FAST,
    TEST_LOG2_FASTER,
} TestLog2Type;

float TestFastLog2(TestLog2Type type);
int TestFastLogSpeed(TestLog2Type type);


