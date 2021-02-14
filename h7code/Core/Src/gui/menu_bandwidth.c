#include "main.h"
#include "menu_bandwidth.h"
#include "scene_single.h"

#include "interface/interface.h"
#include "interface/menu.h"
#include "measure/settings.h"

static void MenuBandwidthQuant();

const char* FilterXBandwidth(FilterX fx)
{
    switch(fx)
    {
    case FilterX_1: return "52 KHz";
    case FilterX_4: return "10 KHz";
    case FilterX_16: return "2.6 KHz";
    case FilterX_64: return "660 Hz";
    case FilterX_256: return "160 Hz";
    case FilterX_1024: return "40 Hz";
    }
    return "";
}

void MenuBandwidthStart()
{
    MenuReset("Select bandwidth");
    for(int i=FilterX_1; i<=FilterX_1024; i++)
    {
        MenuAdd(FilterXBandwidth(i), i);
    }

    MenuSetIndex(g_filterX);
    MenuRedraw();
    InterfaceGoto(MenuBandwidthQuant);
}

void MenuBandwidthQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;

    g_filterX = MenuData();
    SaveSettings();

    SceneSingleStart();
}
