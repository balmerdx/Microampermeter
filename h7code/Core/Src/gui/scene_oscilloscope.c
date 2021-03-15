#include "main.h"
#include "scene_oscilloscope.h"

#include "UTFT.h"
#include "interface/interface.h"
#include "interface/statusbar.h"

static void SceneOscilloscopeQuant();

void SceneOscilloscopeStart()
{
    UTFT_fillScr(VGA_BLACK);//test code
    HeaderSetTextAndRedraw("Oscilloscope");

    InterfaceGoto(SceneOscilloscopeQuant);
}

void SceneOscilloscopeQuant()
{
    if(EncButtonPressed())
    {
        MenuSwitchSceneStart();
        return;
    }

}
