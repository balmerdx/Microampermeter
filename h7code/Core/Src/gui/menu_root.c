#include "main.h"
#include "menu_root.h"
#include "menu_bandwidth.h"
#include "scene_single.h"

#include "interface/interface.h"
#include "interface/menu.h"

static void MenuLine2Start();
static void MenuLine2Quant();
static void MenuRootQuant();

enum
{
    MR_RETURN,
    MR_BANDWIDTH,
    MR_LINE2,
};

static int last_menu_index = 0;


void MenuRootStart()
{
    MenuReset("Microampermeter menu");
    MenuAdd("..", MR_RETURN);
    MenuAdd("Bandwidth", MR_BANDWIDTH);
    MenuAdd("Line2", MR_LINE2);
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
    SceneSingleStart();
}
