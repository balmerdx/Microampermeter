#include "main.h"
#include "scene_oscilloscope.h"

#include "UTFT.h"
#include "float_to_string.h"

#include "interface/interface.h"
#include "interface/statusbar.h"
#include "menu_switch_scene.h"
#include "gui_settings.h"
#include "measure/store_and_trigger.h"
#include "measure/receive_data.h"
#include "oscilloscope/oscilloscope.h"

#include <string.h>

static void SceneOscilloscopeQuant();

static RectA r_triggered;
static RectA r_buffer_samples;
static OscilloscopeData osc;

static float phase = 0;

void OscilloscopeStartTest(OscilloscopeData* data)
{

}

int OscilloscopeValueTest(OscilloscopeData* data, int x)
{
    return sinf(x/(float)data->pos.width*2*M_PI*3+phase)*data->pos.height/2.f+data->pos.height/2;
}


void SceneOscilloscopeStart()
{
    UTFT_fillScr(VGA_BLACK);//test code
    HeaderSetTextAndRedraw("Oscilloscope");
    StatusbarSetTextAndRedraw("Statusbar");

    RectA r_all = R_DisplaySizeWithoutHeaderAndStatusbar();
    R_FillRectBack(&r_all);

    RectA r_tmp, r_line;
    UTF_SetFont(g_default_font);
    R_SplitY1(&r_all, UTF_Height(), &r_line, &r_all);

    R_SplitX1(&r_line, UTF_StringWidth("Triggered")+X_MARGIN*2, &r_triggered, &r_line);
    R_SplitX1(&r_line, UTF_StringWidth("Samples=")+X_MARGIN, &r_tmp, &r_line);
    R_SplitX1(&r_line, UTF_StringWidth("000000")+X_MARGIN, &r_buffer_samples, &r_line);
    R_DrawStringJustify(&r_tmp, "Samples=", UTF_RIGHT);

    {
        osc.lines_dx = 36;
        osc.lines_dy = 36;
        osc.start = OscilloscopeStartTest;
        osc.value = OscilloscopeValueTest;

        int width = osc.lines_dx*12+1;
        int height = osc.lines_dx*6+1;
        int rest_width2 = (UTFT_getDisplayXSize() - width)/2;

        RectA r_osc;
        R_SplitY1(&r_all, height, &r_osc, &r_all);
        R_SplitX1(&r_osc, rest_width2, &r_tmp, &r_osc);
        R_SplitX1(&r_osc, width, &r_osc, &r_tmp);

        osc.pos = r_osc;
    }

    InterfaceGoto(SceneOscilloscopeQuant);
}

void SceneOscilloscopeQuant()
{
    phase += 0.01f;
    uint16_t start_us = TimeUs();
    OscilloscopeDraw(&osc);
    uint16_t delta_us = TimeUs() - start_us;

    char str[32];
    strcpy(str, "draw(us)=");
    catInt(str, delta_us);
    StatusbarSetTextAndRedraw(str);

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
