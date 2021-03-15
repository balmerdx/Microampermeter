#include "main.h"

#include "menu_switch_scene.h"
#include "scene_single.h"
#include "scene_histogram.h"
#include "scene_oscilloscope.h"

#include "interface/interface.h"
#include "interface/menu.h"
#include "measure/settings.h"

static void MenuSwitchSceneQuant();

void MenuSwitchSceneStart()
{
    MenuReset("Switch scene");
    MenuAdd("Switch to microampermeter", SWITCH_TO_MICROAMPERMETER);
    MenuAdd("Switch to histogram", SWITCH_TO_HISTOGRAM);
    MenuAdd("Switch to oscilloscope", SWITCH_TO_OSCILLOSCOPE);

    MenuRedraw();

    InterfaceGoto(MenuSwitchSceneQuant);
}

void MenuSwitchSceneQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;

    SwitchToScene((SwitchToEnum)MenuData());

}

void SwitchToScene(SwitchToEnum data)
{
    if(g_settings.scene_id != data)
    {
        g_settings.scene_id = data;
        SaveSettings();
    }

    if(data==SWITCH_TO_MICROAMPERMETER)
    {
        SceneSingleStart();
        return;
    }

    if(data==SWITCH_TO_HISTOGRAM)
    {
        SceneHistogramStart();
        return;
    }

    if(data==SWITCH_TO_OSCILLOSCOPE)
    {
        SceneOscilloscopeStart();
        return;
    }

}
