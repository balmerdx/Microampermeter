#include "main.h"

#include "settings.h"
#include "hardware/store_to_spi_flash.h"
#include "my_filter.h"
#include "gui/scene_single.h"
#include "gui/menu_trigger_level.h"

enum
{
    SETTINGS_SECTOR = 0,
    SETTINGS_PERMANENT_SECTOR = 1,
};

Settings g_settings;
SettingsPermanent g_settings_permanent;

float mul_V_original;
float mul_I_original;
static float resistor_value_inv[RESISTOR_1_Om+1];

static void UpdateRinv();
static float GetResistorValueOriginal(RESISTOR r);

void InitSettings()
{
    g_settings.offset_adc_I = 0;
    g_settings.offset_adc_V = 0;

    g_settings.line2_type = line2_type;
    g_settings.filterX = g_filterX;
    g_settings.scene_id = 0;
    g_settings.sum_histogramm_time = 0;
    g_settings.seconds_per_line_idx = 0;
    g_settings.ampers_per_line_idx = 0;
    g_settings.oscilloscope_encoder = 0;
    g_settings.trigger_level = TRIGGER_LEVEL_10uA;
    g_settings.trigger_rising = 1;
    g_settings.dummy0 = 0;
    g_settings.dummy1 = 0;
    g_settings.dummy2 = 0;
}

void InitSettingsPermanent()
{
    float ku = 10;
    float Vref = 2.0;
    //float korr = 1/0.9625f; //Коррекция тока
    mul_I_original = Vref/(1<<23)/ku;//*korr;

    mul_V_original = 1/ku*//ОУ
        1/0.027f*//Резистивный делитель
        Vref*//Напряжение Vref
        1.f/(1<<23);

    g_settings_permanent.mul_I = mul_I_original;
    g_settings_permanent.mul_V = mul_V_original;

    g_settings_permanent.R_100_Om = GetResistorValueOriginal(RESISTOR_100_Om);
    g_settings_permanent.R_10_Om = GetResistorValueOriginal(RESISTOR_10_Om);
    g_settings_permanent.R_1_Om = GetResistorValueOriginal(RESISTOR_1_Om);
    UpdateRinv();
}

bool LoadSettings()
{
    bool ok = SpiFlashReadFromFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
    if(ok)
    {
        if(g_settings.filterX > FilterX_1024)
            g_settings.filterX = FilterX_1;

        if(g_settings.trigger_level >= TRIGGER_LEVEL_COUNT)
            g_settings.trigger_level = TRIGGER_LEVEL_10uA;

        line2_type = g_settings.line2_type;
        g_filterX = g_settings.filterX;
    }
    return ok;
}

bool SaveSettings()
{
    g_settings.line2_type = line2_type;
    g_settings.filterX = g_filterX;
    return SpiFlashWriteToFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
}

bool LoadSettingsPermanent()
{
    bool ok = SpiFlashReadFromFlash(SETTINGS_PERMANENT_SECTOR, sizeof(g_settings_permanent), &g_settings_permanent);
    UpdateRinv();
    return ok;
}

bool SaveSettingsPermanent()
{
    return SpiFlashWriteToFlash(SETTINGS_PERMANENT_SECTOR, sizeof(g_settings_permanent), &g_settings_permanent);
}

static float PAR(float R1, float R2)
{
    return R1*R2/(R1+R2);
}

float GetResistorValueOriginal(RESISTOR r)
{
    float R1K = 1000;
    float R100 = 100;
    float R10 = 10;
    float R1 = 1.2f;

    switch(r)
    {
    default:
    case RESISTOR_1_Kom: return R1K;
    case RESISTOR_100_Om: return PAR(R1K, R100);
    case RESISTOR_10_Om: return PAR(R1K, R10);
    case RESISTOR_1_Om: return PAR(R1K, R1);
    }
}

float GetResistorValue(RESISTOR r)
{
    float R1K = 1000;

    switch(r)
    {
    default:
    case RESISTOR_1_Kom: return R1K;
    case RESISTOR_100_Om: return g_settings_permanent.R_100_Om;
    case RESISTOR_10_Om: return g_settings_permanent.R_10_Om;
    case RESISTOR_1_Om: return g_settings_permanent.R_1_Om;
    }
}

float GetResistorValueInv(RESISTOR r)
{
    return resistor_value_inv[r];
}

void UpdateRinv()
{
    resistor_value_inv[RESISTOR_1_Kom] = 1.f/GetResistorValue(RESISTOR_1_Kom);
    resistor_value_inv[RESISTOR_100_Om] = 1.f/g_settings_permanent.R_100_Om;
    resistor_value_inv[RESISTOR_10_Om] = 1.f/g_settings_permanent.R_10_Om;
    resistor_value_inv[RESISTOR_1_Om] = 1.f/g_settings_permanent.R_1_Om;
}

float TriggerLevelAmpers()
{
    switch(g_settings.trigger_level)
    {
    case TRIGGER_LEVEL_1uA: return 1e-6f;
    case TRIGGER_LEVEL_2uA: return 2e-6f;
    case TRIGGER_LEVEL_5uA: return 5e-6f;
    case TRIGGER_LEVEL_10uA: return 10e-6f;
    case TRIGGER_LEVEL_20uA: return 20e-6f;
    case TRIGGER_LEVEL_50uA: return 50e-6f;
    case TRIGGER_LEVEL_100uA: return 100e-6f;
    case TRIGGER_LEVEL_200uA: return 200e-6f;
    case TRIGGER_LEVEL_500uA: return 500e-6f;
    case TRIGGER_LEVEL_1mA: return 1e-3f;
    case TRIGGER_LEVEL_2mA: return 2e-3f;
    case TRIGGER_LEVEL_5mA: return 5e-3f;
    case TRIGGER_LEVEL_10mA: return 10e-3f;
    }

    return 10e-6f;
}
