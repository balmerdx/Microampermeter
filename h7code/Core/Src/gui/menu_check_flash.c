#include "main.h"
#include "menu_check_flash.h"
#include <string.h>

#include "menu_root.h"
#include "interface/interface.h"
#include "interface/menu.h"
#include "float_to_string.h"
#include "hardware/m25p16.h"

enum
{
    MC_RETURN,
    MC_READ_RAM_ID,
    MC_ERASE0_4K,
    MC_ERASE0_64K,
    MC_ERASE0_ALL,
    MC_WRITE0,
    MC_CHECK0,
    MC_WRITE_OFFSET,
    MC_CHECK_OFFSET,
    MC_WRITE_ALL,
    MC_READ_ALL,
};

static void MenuCheckFlashQuant();

#define ORIGINAL_DATA_SIZE 1024
static uint32_t original_data[ORIGINAL_DATA_SIZE];

void MenuCheckFlashStart()
{
    MenuReset2("Calibrate");
    MenuAdd("..", MC_RETURN);
    MenuAdd("ID&Status", MC_READ_RAM_ID);
    MenuAdd("Erase0 4k", MC_ERASE0_4K);
    MenuAdd("Erase0 64k", MC_ERASE0_64K);
    MenuAdd("Erase All", MC_ERASE0_ALL);
    MenuAdd("Write0", MC_WRITE0);
    MenuAdd("Check0", MC_CHECK0);
    MenuAdd("Write offs", MC_WRITE_OFFSET);
    MenuAdd("Check offs", MC_CHECK_OFFSET);
    MenuAdd("Write all", MC_WRITE_ALL);
    MenuAdd("Read all", MC_READ_ALL);

    MenuRedraw();


    bool flash_ok = m25p16_read_ram_id_and_check();
    uint16_t sectors = m25p16_sectors_count();

    //m25p16_power_up_flash_ram();
    //m25p16_write_ram_status(0); //Почемуто память была в protect. Разрешили писать в неё.

    char str[STATUSBAR_STR_LEN];
    strcpy(str, flash_ok?"OK ":"FAIL ");
    strcat(str, "sectors=");
    intToString(str+strlen(str), sectors, 6, ' ');
    StatusbarSetTextAndRedraw(str);

    InterfaceGoto(MenuCheckFlashQuant);
}

static void WriteDataExample(uint32_t offset, uint32_t magic)
{
    char str[STATUSBAR_STR_LEN];
    for(uint32_t i=0; i<ORIGINAL_DATA_SIZE; i++)
        original_data[i] = i + magic;
    uint16_t start_us = TimeUs();
    m25p16_write(offset, ORIGINAL_DATA_SIZE*4, original_data);
    strcpy(str, "Flash write time=");
    catInt(str, (uint16_t)(TimeUs()-start_us));
    strcat(str, " us offset=");
    catInt(str, offset);
    StatusbarSetTextAndRedraw(str);
}

static void CheckDataExample(uint32_t offset, uint32_t magic)
{
    char str[STATUSBAR_STR_LEN];
    uint16_t start_us = TimeUs();
    m25p16_read(offset, ORIGINAL_DATA_SIZE*4, original_data);
    uint16_t delta_us = TimeUs()-start_us;
    bool ok = true;
    int idx = 0;
    for(uint32_t i=0; i<ORIGINAL_DATA_SIZE; i++)
        if(original_data[i] != (i+magic))
        {
            idx = i;
            ok = false;
            break;
        }
    if(ok)
    {
        strcpy(str, "Check ok time=");
        catInt(str, delta_us);
        strcat(str, " us");
        StatusbarSetTextAndRedraw(str);
        return;
    }

    strcpy(str, "Fail idx=");
    catInt(str, idx);
    strcat(str, " ");
    catInt(str, original_data[idx]);
    StatusbarSetTextAndRedraw(str);
}

static bool pattern_invert = false;
static void WriteDataExampleAll(uint32_t magic)
{
    char str[STATUSBAR_STR_LEN];
    uint32_t sector_count = m25p16_sectors_count();
    uint32_t start_all_ms = TimeMs();
    for(uint32_t sector=0; sector<sector_count; sector++)
    {
        uint32_t start_ms = TimeMs();
        //Пишем о 4K и после каждого сектора 64k обновляем экран
        const uint32_t sz4 = sizeof(original_data[0]);
        uint32_t data_size_bytes = ORIGINAL_DATA_SIZE*sz4;
        uint32_t block_count = m25p16_sector_size()/data_size_bytes;
        for(uint32_t block=0; block<block_count; block++)
        {
            uint32_t offset = sector*m25p16_sector_size()+block*data_size_bytes;
            uint32_t magic_cur = magic + offset/sz4;
            for(uint32_t i=0; i<ORIGINAL_DATA_SIZE; i++)
            {
                uint32_t d = i + magic_cur;
                if(pattern_invert)
                    d = ~d;
                original_data[i] = d;
            }

            m25p16_write(offset, data_size_bytes, original_data);
        }

        strcpy(str, "Flash write sector=");
        catInt(str, sector);
        strcat(str, " t=");
        catInt(str, TimeMs()-start_ms);
        strcat(str, " ms");
        StatusbarSetTextAndRedraw(str);
    }

    strcpy(str, "Flash write all t=");
    catInt(str, TimeMs()-start_all_ms);
    strcat(str, " ms");
    StatusbarSetTextAndRedraw(str);
}

static void CheckDataExampleAll(uint32_t magic)
{
    char str[STATUSBAR_STR_LEN];
    uint32_t sector_count = m25p16_sectors_count();
    uint32_t start_all_ms = TimeMs();
    for(uint32_t sector=0; sector<sector_count; sector++)
    {
        const uint32_t sz4 = sizeof(original_data[0]);
        uint32_t data_size_bytes = ORIGINAL_DATA_SIZE*sz4;
        uint32_t block_count = m25p16_sector_size()/data_size_bytes;
        for(uint32_t block=0; block<block_count; block++)
        {
            uint32_t offset = sector*m25p16_sector_size()+block*data_size_bytes;
            uint32_t magic_cur = magic + offset/sz4;
            m25p16_read(offset, data_size_bytes, original_data);

            for(uint32_t i=0; i<ORIGINAL_DATA_SIZE; i++)
            {
                uint32_t d = i + magic_cur;
                if(pattern_invert)
                    d = ~d;
                if(original_data[i] != d)
                {
                    strcpy(str, "Fail offset=");
                    catInt(str, i*sz4+offset);
                    strcat(str, " ");
                    catInt(str, original_data[i]);
                    StatusbarSetTextAndRedraw(str);
                    return;
                }
            }

        }
    }

    strcpy(str, "Flash read all t=");
    catInt(str, TimeMs()-start_all_ms);
    strcat(str, " ms");
    StatusbarSetTextAndRedraw(str);

}

void MenuCheckFlashQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;

    if(MenuData()==MC_RETURN)
    {
        MenuRootStart();
        return;
    }

    uint32_t magic = 200;
    uint32_t offset0 = 0;
    uint32_t offset1 = 70;
    if(MenuData()==MC_WRITE0)
    {
        WriteDataExample(offset0, magic);
        return;
    }

    if(MenuData()==MC_CHECK0)
    {
        CheckDataExample(offset0, magic);
        return;
    }

    if(MenuData()==MC_WRITE_OFFSET)
    {
        WriteDataExample(offset1, magic);
        return;
    }

    if(MenuData()==MC_CHECK_OFFSET)
    {
        CheckDataExample(offset1, magic);
        return;
    }

    if(MenuData()==MC_ERASE0_4K)
    {
        char str[STATUSBAR_STR_LEN];
        uint16_t start_us = TimeMs();
        m25p16_ram_erase4k(0);
        uint16_t delta_us = TimeMs()-start_us;
        strcpy(str, "Erase4k time=");
        catInt(str, delta_us);
        strcat(str, " ms");
        StatusbarSetTextAndRedraw(str);
        return;
    }

    if(MenuData()==MC_ERASE0_64K)
    {
        char str[STATUSBAR_STR_LEN];
        uint16_t start_us = TimeMs();
        m25p16_ram_erase64k(0);
        uint16_t delta_us = TimeMs()-start_us;
        strcpy(str, "Erase64k time=");
        catInt(str, delta_us);
        strcat(str, " ms");
        StatusbarSetTextAndRedraw(str);
        return;
    }

    if(MenuData()==MC_ERASE0_ALL)
    {
        char str[STATUSBAR_STR_LEN];
        StatusbarSetTextAndRedraw("Start erase all");
        uint16_t start_us = TimeMs();
        m25p16_ram_bulk_erase();
        uint16_t delta_us = TimeMs()-start_us;
        strcpy(str, "Erase all time=");
        catInt(str, delta_us);
        strcat(str, " ms");
        StatusbarSetTextAndRedraw(str);
        return;
    }

    if(MenuData()==MC_READ_RAM_ID)
    {
        char str[STATUSBAR_STR_LEN];
        uint8_t id[3];
        m25p16_read_ram_id(id);
        strcpy(str, "id=");
        catInt(str, id[0]);
        strcat(str, ",");
        catInt(str, id[1]);
        strcat(str, ",");
        catInt(str, id[2]);
        strcat(str, " status=");
        catInt(str, m25p16_read_ram_status());

        StatusbarSetTextAndRedraw(str);
    }

    if(MenuData()==MC_WRITE_ALL)
    {
        pattern_invert = !pattern_invert;
        WriteDataExampleAll(magic);
        return;
    }

    if(MenuData()==MC_READ_ALL)
    {
        CheckDataExampleAll(magic);
        return;
    }
}
