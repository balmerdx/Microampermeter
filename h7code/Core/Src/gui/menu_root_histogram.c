#include "main.h"
#include "menu_root_histogram.h"
#include "menu_bandwidth.h"
#include "menu_time_histogram.h"

#include "interface/interface.h"
#include "interface/menu.h"
#include "measure/settings.h"
#include "scene_single.h"
#include "scene_histogram.h"

static void MenuRootHistogramQuant();

enum
{
    MR_RETURN,
    MR_SWITCH_TO_MICROAMPERMETER,
    MR_BANDWIDTH,
    MR_SUM_TIME,
};

static int last_menu_index = 0;

void MenuRootHistogramStart()
{
    MenuReset("Histogram menu");
    MenuAdd("..", MR_RETURN);
    MenuAdd("Switch to microampermeter", MR_SWITCH_TO_MICROAMPERMETER);
    MenuAdd("Bandwidth", MR_BANDWIDTH);
    MenuAdd("Sum time", MR_SUM_TIME);
    MenuSetIndex(last_menu_index);
    MenuRedraw();
    InterfaceGoto(MenuRootHistogramQuant);
}

void MenuRootHistogramQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;

    last_menu_index = MenuIndex();

    if(MenuData()==MR_RETURN)
    {
        SceneHistogramStart();
        return;
    }

    if(MenuData()==MR_SWITCH_TO_MICROAMPERMETER)
    {
        g_settings.is_histogramm = 0;
        SaveSettings();
        SceneSingleStart();
        return;
    }

    if(MenuData()==MR_BANDWIDTH)
    {
        MenuBandwidthStart();
        return;
    }

    if(MenuData()==MR_SUM_TIME)
    {
        MenuTimeHistogramStart();
        return;
    }
}
