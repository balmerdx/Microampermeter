#include "main.h"
#include "menu_root.h"
#include "menu_bandwidth.h"
#include "menu_check_flash.h"
#include "menu_calibration.h"
#include "menu_switch_scene.h"
#include "scene_single.h"
#include "scene_histogram.h"
#include "scene_about.h"

#include "interface/interface.h"
#include "interface/menu.h"
#include "measure/settings.h"
#include <string.h>
#include "float_to_string.h"

static void MenuLine2Start();
static void MenuLine2Quant();
static void MenuRootQuant();

enum
{
    MR_RETURN,
    MR_SWITCH_TO,
    MR_BANDWIDTH,
    MR_LINE2,
    MR_CHECK_FLASH,
    MR_CALIBRATION,
    MR_ABOUT,
};

static int last_menu_index = 0;

void MenuRootStart()
{
    MenuReset("Microampermeter menu");
    MenuAdd("..", MR_RETURN);
    MenuAdd("Switch to...", MR_SWITCH_TO);
    MenuAdd("Bandwidth", MR_BANDWIDTH);
    MenuAdd("Line2", MR_LINE2);
    //MenuAdd("Check flash", MR_CHECK_FLASH);
    MenuAdd("Calibration", MR_CALIBRATION);
    MenuAdd("About", MR_ABOUT);

    MenuSetIndex(last_menu_index);
    MenuRedraw();
    InterfaceGoto(MenuRootQuant);
}

void MenuRootQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;
    last_menu_index = MenuIndex();

    if(MenuData()==MR_RETURN)
    {
        SceneSingleStart();
        return;
    }

    if(MenuData()==MR_SWITCH_TO)
    {
        MenuSwitchSceneStart();
        return;
    }

    if(MenuData()==MR_BANDWIDTH)
    {
        MenuBandwidthStart();
        return;
    }

    if(MenuData()==MR_LINE2)
    {
        MenuLine2Start();
        return;
    }

    if(MenuData()==MR_CHECK_FLASH)
    {
        MenuCheckFlashStart();
        return;
    }

    if(MenuData()==MR_CALIBRATION)
    {
        MenuCalibrationStart();
        return;
    }

    if(MenuData()==MR_ABOUT)
    {
        SceneAboutStart(MenuRootStart);
        return;
    }

}


void MenuLine2Start()
{
    MenuReset("Details view");
    MenuAdd("Current Min/Max", LINE2_CURRENT_MIN_MAX);
    MenuAdd("Resistance", LINE2_RESISTANCE);

    MenuSetIndex(MenuIndexByData(line2_type));
    MenuRedraw();
    InterfaceGoto(MenuLine2Quant);
}

void MenuLine2Quant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;

    line2_type = MenuData();
    SaveSettings();
    SceneSingleStart();
}
