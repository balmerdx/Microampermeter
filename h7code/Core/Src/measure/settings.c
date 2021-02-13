#include "main.h"

#include "settings.h"
#include "hardware/store_to_spi_flash.h"

enum
{
    SETTINGS_SECTOR = 0,
    SETTINGS_PERMANENT_SECTOR = 1,
};

Settings g_settings;
SettingsPermanent g_settings_permanent;
static float resistor_value_inv[RESISTOR_1_Om+1];

static void UpdateRinv();
static float GetResistorValueOriginal(RESISTOR r);

void InitSettings()
{
    g_settings.offset_adc_I = 0;
    g_settings.offset_adc_V = 0;
}

void InitSettingsPermanent()
{
    g_settings_permanent.mul_I = 1.f;
    g_settings_permanent.mul_V = 1.f;

    g_settings_permanent.R_100_Om = GetResistorValueOriginal(RESISTOR_100_Om);
    g_settings_permanent.R_10_Om = GetResistorValueOriginal(RESISTOR_10_Om);
    g_settings_permanent.R_1_Om = GetResistorValueOriginal(RESISTOR_1_Om);
    UpdateRinv();
}

bool LoadSettings()
{
    return SpiFlashReadFromFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
}

bool SaveSettings()
{
    return SpiFlashWriteToFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
}

bool LoadSettingsPermanent()
{
    bool ok = SpiFlashReadFromFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
    UpdateRinv();
    return ok;
}

bool SaveSettingsPermanent()
{
    return SpiFlashWriteToFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
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
