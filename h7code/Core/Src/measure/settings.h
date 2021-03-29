#pragma once
#include "hardware/gpios.h"

// Настройки, которые могут часто изменяться.
typedef struct Settings
{
    //Настройки смещения нуля adc
    //Предполагаем, что их можно перекалибровывать достаточно часто
    int32_t offset_adc_I;
    int32_t offset_adc_V;

    //Объединяем uint8 по 4 байта для выравнивания
    uint8_t line2_type;
    uint8_t filterX;
    uint8_t scene_id; //0-microampermeter, 1-histogramm, 2-oscilloscope
    uint8_t sum_histogramm_time; //See menu_time_histogram.c

    uint8_t seconds_per_line_idx; //See coeff_seconds_per_line
    uint8_t ampers_per_line_idx; //See coeff_ampers_per_line
    uint8_t oscilloscope_encoder;//See OscilloscopeEncoderEnum
    uint8_t trigger_level;//See TriggerLevelEnum

    uint8_t trigger_rising;//1 - rising, 0 - falling
    uint8_t dummy0;
    uint8_t dummy1;
    uint8_t dummy2;
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

//Помечаем для записи. Записываем не очень часто.
//Через несколько секунд.
void SaveSettingsLazy();
void SettingsLazyQuant();

extern int debug_save_settings_count;

void InitSettingsPermanent();
bool LoadSettingsPermanent();
bool SaveSettingsPermanent();

float GetResistorValue(RESISTOR r);
float GetResistorValueInv(RESISTOR r);

//Пара констант коэффициентов усиления.
extern float mul_V_original;
extern float mul_I_original;

float TriggerLevelAmpers();
