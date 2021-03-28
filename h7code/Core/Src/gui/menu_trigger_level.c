#include "main.h"
#include "menu_trigger_level.h"
#include "measure/settings.h"
#include "interface/interface.h"
#include "interface/menu.h"
#include "scene_oscilloscope.h"

static void MenuTriggerLevelQuant();

const char* TrigerLevelStr(TriggerLevelEnum v)
{
    switch(v)
    {
    case TRIGGER_LEVEL_1uA: return "1 uA";
    case TRIGGER_LEVEL_2uA: return "2 uA";
    case TRIGGER_LEVEL_5uA: return "5 uA";
    case TRIGGER_LEVEL_10uA: return "10 uA";
    case TRIGGER_LEVEL_20uA: return "20 uA";
    case TRIGGER_LEVEL_50uA: return "50 uA";
    case TRIGGER_LEVEL_100uA: return "100 uA";
    case TRIGGER_LEVEL_200uA: return "200 uA";
    case TRIGGER_LEVEL_500uA: return "500 uA";
    case TRIGGER_LEVEL_1mA: return "1 mA";
    case TRIGGER_LEVEL_2mA: return "2 mA";
    case TRIGGER_LEVEL_5mA: return "5 mA";
    case TRIGGER_LEVEL_10mA: return "10 mA";
    default: return "";
    }
}

void MenuTriggerLevelStart()
{
    MenuReset2("Trigger level");
    for(int i=0; i<TRIGGER_LEVEL_COUNT; i++)
        MenuAdd(TrigerLevelStr(i), i);

    MenuSetIndex(g_settings.trigger_level);
    MenuRedraw();
    InterfaceGoto(MenuTriggerLevelQuant);
}

void MenuTriggerLevelQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;
    g_settings.trigger_level = MenuIndex();
    SaveSettings();
    SceneOscilloscopeStart();
}
