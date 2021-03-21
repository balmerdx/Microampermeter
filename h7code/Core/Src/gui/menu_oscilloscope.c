#include "main.h"
#include "menu_oscilloscope.h"
#include "menu_switch_scene.h"
#include "scene_oscilloscope.h"

#include "interface/interface.h"
#include "interface/menu.h"

enum
{
    MO_SWITCH_TO,
    MO_TRIGGER_START,
    MO_TRIGGER_LEVEL,
    MO_ENCODER_SCALE_CURRENT,
    MO_ENCODER_SCALE_TIME,
    MO_ENCODER_OFFSET_TIME,
};

static void MenuOscilloscopeQuant();

static int last_menu_index = 0;

void MenuOscilloscopeStart()
{
    MenuReset("Oscilloscope menu");
    MenuAdd("Switch to...", MO_SWITCH_TO);
    MenuAdd("Trigger start", MO_TRIGGER_START);
    MenuAdd("Trigger level", MO_TRIGGER_LEVEL);
    MenuAdd("Encoder scale current", MO_ENCODER_SCALE_CURRENT);
    MenuAdd("Encoder scale time", MO_ENCODER_SCALE_TIME);
    MenuAdd("Encoder offset time", MO_ENCODER_OFFSET_TIME);

    MenuSetIndex(last_menu_index);
    MenuRedraw();
    InterfaceGoto(MenuOscilloscopeQuant);
}

void MenuOscilloscopeQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;
    last_menu_index = MenuIndex();

    if(MenuIndex()==MO_SWITCH_TO)
    {
        MenuSwitchSceneStart();
        return;
    }

    if(MenuIndex()==MO_TRIGGER_START)
    {
        OscilloscopeTriggerStart();
        SceneOscilloscopeStart();
        return;
    }

    if(MenuIndex()==MO_TRIGGER_LEVEL)
    {
        return;
    }

    if(MenuIndex()==MO_ENCODER_SCALE_CURRENT)
    {
        SetOscilloscopeEncoder(ENCODER_SCALE_CURRENT);
        SceneOscilloscopeStart();
        return;
    }

    if(MenuIndex()==MO_ENCODER_SCALE_TIME)
    {
        SetOscilloscopeEncoder(ENCODER_SCALE_TIME);
        SceneOscilloscopeStart();
        return;
    }

    if(MenuIndex()==MO_ENCODER_OFFSET_TIME)
    {
        SetOscilloscopeEncoder(ENCODER_OFFSET_TIME);
        SceneOscilloscopeStart();
        return;
    }
}
