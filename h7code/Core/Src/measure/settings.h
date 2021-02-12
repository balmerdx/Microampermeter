#pragma once

// Настройки, которые могут часто изменяться.
typedef struct Settings
{
    //Настройки смещения нуля adc
    //Предполагаем, что их можно перекалибровывать достаточно часто
    int32_t offset_adc_I;
    int32_t offset_adc_V;
} Settings;


//Настройки, которые могут изменяться крайне редко
typedef struct SettingsPermanent
{
    float mul_I;
    float mul_V;
} SettingsPermanent;

extern Settings g_settings;
extern SettingsPermanent g_settings_permanent;

void InitSettings();
bool LoadSettings();
bool SaveSettings();

void InitSettingsPermanent();
bool LoadSettingsPermanent();
bool SaveSettingsPermanent();

