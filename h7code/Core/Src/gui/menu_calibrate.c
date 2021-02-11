#include "main.h"
#include "menu_calibrate.h"
#include <string.h>

#include "menu_root.h"
#include "interface/interface.h"
#include "interface/menu.h"
#include "float_to_string.h"
#include "hardware/m25p16.h"
#include "hardware/flash_out_of_bound.h"

//#define ADDR_FLASH_SECTOR_1_BANK1     ((uint32_t)0x08020000)
//#define ADDR_FLASH_SECTOR_1_BANK1  (FLASH_BANK1_BASE+FLASH_SECTOR_SIZE*0)
#define ADDR_FLASH_SECTOR_1_BANK1  (FLASH_BANK1_BASE+FLASH_SECTOR_SIZE*1-1000)

enum
{
    MC_RETURN,
    MC_WRITE,
    MC_CHECK,
    MC_ERASE,
    MC_READ_RAM_ID,
    MC_RAM_STATUS,

};

static void MenuCalibrateQuant();

#define ORIGINAL_DATA_SIZE 32
static uint32_t original_data[ORIGINAL_DATA_SIZE];

void MenuCalibrateStart()
{
    MenuReset("Calibrate");
    MenuAdd("..", MC_RETURN);
    MenuAdd("Write", MC_WRITE);
    MenuAdd("Check", MC_CHECK);
    MenuAdd("Erase", MC_ERASE);
    MenuAdd("Ram ID", MC_READ_RAM_ID);
    MenuAdd("Ram status", MC_RAM_STATUS);

    MenuRedraw();


    bool flash_ok = m25p16_read_ram_id_and_check();
    uint16_t sectors = m25p16_sectors_count();

    //m25p16_power_up_flash_ram();
    m25p16_write_ram_status(0); //Почемуто память была в protect. Разрешили писать в неё.

    char str[STATUSBAR_STR_LEN];
    strcpy(str, flash_ok?"OK ":"FAIL ");
    strcat(str, "sectors=");
    intToString(str+strlen(str), sectors, 6, ' ');
    StatusbarSetTextAndRedraw(str);

    InterfaceGoto(MenuCalibrateQuant);
}

void MenuCalibrateQuant()
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
    if(MenuData()==MC_WRITE)
    {
        for(uint32_t i=0; i<ORIGINAL_DATA_SIZE; i++)
            original_data[i] = i + magic;
        m25p16_write(0, ORIGINAL_DATA_SIZE*4, original_data);
        StatusbarSetTextAndRedraw("Flash write complete");
        return;
    }

    if(MenuData()==MC_CHECK)
    {
        m25p16_read(0, ORIGINAL_DATA_SIZE*4, original_data);
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
            StatusbarSetTextAndRedraw("Check ok");
            return;
        }

        char str[STATUSBAR_STR_LEN];
        strcpy(str, "Fail idx=");
        catInt(str, idx);
        strcat(str, " ");
        catInt(str, original_data[idx]);
        StatusbarSetTextAndRedraw(str);
        return;
    }

    if(MenuData()==MC_ERASE)
    {
        m25p16_ram_erase4k(0);
        StatusbarSetTextAndRedraw("Erase complete");
        return;
    }

    if(MenuData()==MC_READ_RAM_ID)
    {
        uint8_t id[3];
        m25p16_read_ram_id(id);
        char str[STATUSBAR_STR_LEN];
        strcpy(str, "id=");
        catInt(str, id[0]);
        strcat(str, ",");
        catInt(str, id[1]);
        strcat(str, ",");
        catInt(str, id[2]);

        StatusbarSetTextAndRedraw(str);
    }

    if(MenuData()==MC_RAM_STATUS)
    {
        char str[STATUSBAR_STR_LEN];
        strcpy(str, "status=");
        catInt(str, m25p16_read_ram_status());
        StatusbarSetTextAndRedraw(str);
    }
}
