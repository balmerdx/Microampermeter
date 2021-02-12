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

void InitSettings()
{
    g_settings.offset_adc_I = 0;
    g_settings.offset_adc_V = 0;
}

void InitSettingsPermanent()
{
    g_settings_permanent.mul_I = 1.f;
    g_settings_permanent.mul_V = 1.f;
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
    return SpiFlashReadFromFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
}

bool SaveSettingsPermanent()
{
    return SpiFlashWriteToFlash(SETTINGS_SECTOR, sizeof(g_settings), &g_settings);
}

