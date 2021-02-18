#include "main.h"
#include "menu_time_histogram.h"

#include "interface/interface.h"
#include "interface/menu.h"
#include "measure/settings.h"
#include "scene_histogram.h"

enum
{
    TH_100MS,
    TH_300MS,
    TH_1S,
    TH_3S,
    TH_10S,
    TH_30S,
};

static void MenuTimeHistogramQuant();

void MenuTimeHistogramStart()
{
    MenuReset2("Histogram sum time");

    MenuAdd("100 ms", TH_100MS);
    MenuAdd("300 ms", TH_300MS);
    MenuAdd("1 s", TH_1S);
    MenuAdd("3 s", TH_3S);
    MenuAdd("10 s", TH_10S);
    MenuAdd("30 s", TH_30S);

    MenuSetIndex(g_settings.sum_histogramm_time);
    MenuRedraw();

    InterfaceGoto(MenuTimeHistogramQuant);
}

void MenuTimeHistogramQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;
    g_settings.sum_histogramm_time = MenuIndex();
    SaveSettings();
    SceneHistogramStart();
}

float GetTimeHistogram()
{
    switch(g_settings.sum_histogramm_time)
    {
    case TH_100MS: return 0.1f;
    case TH_300MS: return 0.3f;
    case TH_1S: return 1.f;
    case TH_3S: return 3.f;
    case TH_10S: return 10.f;
    case TH_30S: return 30.f;
    }

    return 1.f;
}
