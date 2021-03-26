#include "main.h"
#include "menu_trigger_level.h"
#include "measure/settings.h"
#include "interface/interface.h"
#include "interface/menu.h"
#include "scene_oscilloscope.h"

static void MenuTriggerLevelQuant();

void MenuTriggerLevelStart()
{
    MenuReset2("Trigger level");
    MenuAdd("1 uA", TRIGGER_LEVEL_1uA);
    MenuAdd("2 uA", TRIGGER_LEVEL_2uA);
    MenuAdd("5 uA", TRIGGER_LEVEL_5uA);
    MenuAdd("10 uA", TRIGGER_LEVEL_10uA);
    MenuAdd("20 uA", TRIGGER_LEVEL_20uA);
    MenuAdd("50 uA", TRIGGER_LEVEL_50uA);
    MenuAdd("100 uA", TRIGGER_LEVEL_100uA);
    MenuAdd("200 uA", TRIGGER_LEVEL_200uA);
    MenuAdd("500 uA", TRIGGER_LEVEL_500uA);
    MenuAdd("1 mA", TRIGGER_LEVEL_1mA);
    MenuAdd("2 mA", TRIGGER_LEVEL_2mA);
    MenuAdd("5 mA", TRIGGER_LEVEL_5mA);
    MenuAdd("10 mA", TRIGGER_LEVEL_10mA);

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
