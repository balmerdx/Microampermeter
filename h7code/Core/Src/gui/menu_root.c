#include "main.h"
#include "menu_root.h"
#include "menu_bandwidth.h"
#include "scene_single.h"

#include "interface/interface.h"
#include "interface/menu.h"
enum
{
    MR_RETURN,
    MR_BANDWIDTH,
    MR_LINE2,
};

static int last_menu_index = 0;

static void MenuRootQuant();

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
        return;
    }
}
