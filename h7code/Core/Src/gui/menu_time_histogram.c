#include "main.h"
#include "menu_time_histogram.h"

#include "interface/interface.h"
#include "interface/menu.h"
#include "measure/settings.h"
#include "scene_histogram.h"

enum
{
    TH_100MS,
    TH_200MS,
    TH_500MS,
    TH_1S,
    TH_2S,
    TH_5S,
    TH_10S,
    TH_20S,
};

static void MenuTimeHistogramQuant();

void MenuTimeHistogramStart()
{
    MenuReset2("Histogram sum time");

    MenuAdd("100 ms", TH_100MS);
    MenuAdd("200 ms", TH_200MS);
    MenuAdd("500 ms", TH_500MS);
    MenuAdd("1 s", TH_1S);
    MenuAdd("2 s", TH_2S);
    MenuAdd("5 s", TH_5S);
    MenuAdd("10 s", TH_10S);
    MenuAdd("20 s", TH_20S);

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
    case TH_200MS: return 0.2f;
    case TH_500MS: return 0.5f;
    case TH_1S: return 1.f;
    case TH_2S: return 2.f;
    case TH_5S: return 5.f;
    case TH_10S: return 10.f;
    case TH_20S: return 20.f;
    }

    return 1.f;
}
