#pragma once
#include "hardware/gpios.h"

// Настройки, которые могут часто изменяться.
typedef struct Settings
{
    //Настройки смещения нуля adc
    //Предполагаем, что их можно перекалибровывать достаточно часто
    int32_t offset_adc_I;
    int32_t offset_adc_V;
    uint8_t line2_type;
    uint8_t filterX;
    uint8_t is_histogramm; //0 - microampermeter, 1 - histogramm
    uint8_t sum_histogramm_time; //See menu_time_histogram.c
} Settings;


//Настройки, которые могут изменяться крайне редко
typedef struct SettingsPermanent
{
    float mul_I;
    float mul_V;
    //Эти значения сразу учитывают то, что
    //они в параллели с резистором 1 КОм стоят
    float R_100_Om;
    float R_10_Om;
    float R_1_Om;
} SettingsPermanent;

extern Settings g_settings;
extern SettingsPermanent g_settings_permanent;

void InitSettings();
bool LoadSettings();
bool SaveSettings();

void InitSettingsPermanent();
bool LoadSettingsPermanent();
bool SaveSettingsPermanent();

float GetResistorValue(RESISTOR r);
float GetResistorValueInv(RESISTOR r);

//Пара констант коэффициентов усиления.
extern float mul_V_original;
extern float mul_I_original;
