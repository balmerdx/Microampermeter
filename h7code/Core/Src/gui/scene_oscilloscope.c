#include "main.h"
#include "scene_oscilloscope.h"

#include "UTFT.h"
//#include "utf_font.h"
#include "interface/interface.h"
#include "interface/statusbar.h"
#include "menu_switch_scene.h"
#include "gui_settings.h"
#include "measure/store_and_trigger.h"
#include "measure/receive_data.h"

static void SceneOscilloscopeQuant();

RectA r_triggered;
RectA r_buffer_samples;

void SceneOscilloscopeStart()
{
    UTFT_fillScr(VGA_BLACK);//test code
    HeaderSetTextAndRedraw("Oscilloscope");
    StatusbarSetTextAndRedraw("Statusbar");

    RectA r_all = R_DisplaySizeWithoutHeaderAndStatusbar();
    R_FillRectBack(&r_all);

    RectA r_tmp, r_line;
    UTF_SetFont(g_default_font);
    R_SplitY1(&r_all, UTF_Height(), &r_line, &r_tmp);

    R_SplitX1(&r_line, UTF_StringWidth("Triggered")+X_MARGIN*2, &r_triggered, &r_line);
    R_SplitX1(&r_line, UTF_StringWidth("Samples=")+X_MARGIN, &r_tmp, &r_line);
    R_SplitX1(&r_line, UTF_StringWidth("000000")+X_MARGIN, &r_buffer_samples, &r_line);
    R_DrawStringJustify(&r_tmp, "Samples=", UTF_RIGHT);

    InterfaceGoto(SceneOscilloscopeQuant);
}

void SceneOscilloscopeQuant()
{
    if(EncButtonPressed())
    {
        MenuSwitchSceneStart();
        return;
    }

    if(EncValueChanged())
    {
        int delta = EncValueDelta();
        if(delta>0)
        {
            if(!STCaptureStarted())
            {
                STCaptureStart();
                EnableCapturingTrigger(); //test
            }
        }
    }

    {
        char* str = "Off";
        if(STCaptureCompleted())
            str = "Completed";
        else if(STTriggerTriggered())
            str = "Triggered";
        else if(STCaptureStarted())
            str = "Started";

        R_DrawStringJustify(&r_triggered, str, UTF_CENTER);
    }

    R_printNumI(&r_buffer_samples, STSamplesCaptured(), UTF_LEFT);
}
