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
    MC_WRITE0,
    MC_CHECK0,
    MC_ERASE0_4K,
    MC_ERASE0_64K,
    MC_ERASE0_ALL,
    MC_READ_RAM_ID,
    MC_RAM_STATUS,

};

static void MenuCheckFlashQuant();

#define ORIGINAL_DATA_SIZE 1024
static uint32_t original_data[ORIGINAL_DATA_SIZE];

void MenuCheckFlashStart()
{
    MenuReset2("Calibrate");
    MenuAdd("..", MC_RETURN);
    MenuAdd("ID&Status", MC_READ_RAM_ID);
    MenuAdd("Ram status", MC_RAM_STATUS);
    MenuAdd("Write0", MC_WRITE0);
    MenuAdd("Check0", MC_CHECK0);
    MenuAdd("Erase0 4k", MC_ERASE0_4K);
    MenuAdd("Erase0 64k", MC_ERASE0_64K);
    MenuAdd("Erase0 All", MC_ERASE0_ALL);

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

void MenuCheckFlashQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;

    char str[STATUSBAR_STR_LEN];

    if(MenuData()==MC_RETURN)
    {
        MenuRootStart();
        return;
    }

    uint32_t magic = 200;
    if(MenuData()==MC_WRITE0)
    {
        for(uint32_t i=0; i<ORIGINAL_DATA_SIZE; i++)
            original_data[i] = i + magic;
        uint16_t start_us = TimeUs();
        m25p16_write(0, ORIGINAL_DATA_SIZE*4, original_data);
        strcpy(str, "Flash write time=");
        catInt(str, (uint16_t)(TimeUs()-start_us));
        strcat(str, " µs");
        StatusbarSetTextAndRedraw(str);
        return;
    }

    if(MenuData()==MC_CHECK0)
    {
        uint16_t start_us = TimeUs();
        m25p16_read(0, ORIGINAL_DATA_SIZE*4, original_data);
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
        return;
    }

    if(MenuData()==MC_ERASE0_4K)
    {
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
}
