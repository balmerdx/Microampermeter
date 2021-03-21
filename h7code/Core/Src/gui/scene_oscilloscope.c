#include "main.h"
#include "scene_oscilloscope.h"

#include "UTFT.h"
#include "float_to_string.h"

#include "interface/interface.h"
#include "interface/statusbar.h"
#include "menu_oscilloscope.h"
#include "gui_settings.h"
#include "measure/store_and_trigger.h"
#include "measure/receive_data.h"
#include "measure/settings.h"
#include "oscilloscope/oscilloscope.h"

#include <string.h>
#include <sys/param.h>

static void SceneOscilloscopeQuant();
static void OscilloscopeEncoderQuant(int delta);

static RectA r_triggered;
static RectA r_buffer_samples;
static RectA r_ampers_per_line;
static RectA r_seconds_per_line;
static OscilloscopeData osc;

static bool enable_redraw = false;
static bool use_test = false;
static float phase = 0;

//Коэффициент масштабирования по оси X (секунд на lines_dx)
static float seconds_per_line = 1e-2f;
//Коэффициент масштабирования по оси Y (ампер на lines_dy)
static float amper_per_line = 1e-3f;

static float samples_per_pixel;// ось X
static float y_mul; //ampers per pixel


typedef struct
{
    char* info;
    float mul;
} MulCoeff;

MulCoeff coeff_seconds_per_line[] =
{
    {"1 ms", 1e-3f},
    {"2 ms", 2e-3f},
    {"5 ms", 5e-3f},
    {"10 ms", 1e-2f},
    {"20 ms", 2e-2f},
    {"50 ms", 5e-2f},
    {"100 ms", 1e-1f},
    {"200 ms", 2e-1f},
    {"500 ms", 5e-1f},
};

MulCoeff coeff_ampers_per_line[] =
{
    {"1 uA", 1e-6f},
    {"2 uA", 2e-6f},
    {"5 uA", 5e-6f},
    {"10 uA", 1e-5f},
    {"20 uA", 2e-5f},
    {"50 uA", 5e-5f},
    {"100 uA", 1e-4f},
    {"200 uA", 2e-4f},
    {"500 uA", 5e-4f},
    {"1 mA", 1e-3f},
    {"2 mA", 2e-3f},
    {"5 mA", 5e-3f},
    {"10 mA", 1e-2f},
    {"20 mA", 2e-2f},
};

int AmperPerLineCount()
{
    return sizeof(coeff_ampers_per_line)/sizeof(coeff_ampers_per_line[0]);
}

static void AmperPerLineClamp(int* idx)
{
    int size = AmperPerLineCount();
    *idx = MIN(*idx, size-1);
    *idx = MAX(*idx, 0);
}

float AmperPerLineCurrent(int idx)
{
    AmperPerLineClamp(&idx);
    return coeff_ampers_per_line[idx].mul;
}

char* AmperPerLineInfo(int idx)
{
    AmperPerLineClamp(&idx);
    return coeff_ampers_per_line[idx].info;
}

int SecondsPerLineCount()
{
    return sizeof(coeff_seconds_per_line)/sizeof(coeff_seconds_per_line[0]);
}

static void SecondsPerLineClamp(int* idx)
{
    int size = SecondsPerLineCount();
    *idx = MIN(*idx, size-1);
    *idx = MAX(*idx, 0);
}

float SecondsPerLineCurrent(int idx)
{
    SecondsPerLineClamp(&idx);
    return coeff_seconds_per_line[idx].mul;
}

char* SecondsPerLineInfo(int idx)
{
    SecondsPerLineClamp(&idx);
    return coeff_seconds_per_line[idx].info;
}

void OscilloscopeStartTest(OscilloscopeData* data)
{

}

int OscilloscopeValueTest(OscilloscopeData* data, int x)
{
    return sinf(x/(float)data->pos.width*2*M_PI*3+phase)*data->pos.height/2.f+data->pos.height/2;
}

void OscilloscopeStartSum(OscilloscopeData* data)
{
    seconds_per_line = SecondsPerLineCurrent(g_settings.seconds_per_line_idx);
    amper_per_line = AmperPerLineCurrent(g_settings.ampers_per_line_idx);

    samples_per_pixel = STFilterSPS() * seconds_per_line / osc.lines_dx;
    y_mul = osc.lines_dy / amper_per_line;

    data->line_special_x = 0;
    data->line_special_y = (data->pos.height/data->lines_dy)*data->lines_dy;
}

static float cur_value;
static int cur_count;
void IterateIntervalCallback(float sample, void* param)
{
    cur_value += sample;
    cur_count++;
}

int PixelToBufferOffset(int x)
{
    return lroundf(STTriggerOffset() + x*samples_per_pixel);
}

int OscilloscopeValueSum(OscilloscopeData* data, int x)
{
    cur_value = 0;
    cur_count = 0;
    int istart = PixelToBufferOffset(x);
    int iend = PixelToBufferOffset(x+1);
    istart = MAX(istart, 0);
    iend = MIN(iend, (int)STSamplesCaptured());

    STIterate(istart, iend, IterateIntervalCallback, 0);
    float y = cur_count? (cur_value/cur_count) : 0;
    y  *= y_mul;
    if(y<0)
        y = -1;
    if(y>data->pos.height)
        y = data->pos.height+1;

    return (data->pos.height-1) - y;
}

void OscilloscopeTriggerStart()
{
    STCaptureStart();
    EnableCapturingTrigger(); //test
    enable_redraw = true;
}

void SceneOscilloscopeStart()
{
    HeaderSetTextAndRedraw("Oscilloscope");
    //StatusbarSetTextAndRedraw("Statusbar");
    UTF_SetFont(g_default_font);

    RectA r_tmp, r_line;
    RectA r_all = R_DisplaySize();

    R_SplitY1(&r_all, UTF_Height(), &r_tmp, &r_all);
    R_SplitY2(&r_all, UTF_Height(), &r_all, &r_line);
    R_FillRectBack(&r_all);

    r_line.back_color = COLOR_BACKGROUND_BLUE;
    R_SplitX1(&r_line, UTFT_getDisplayXSize()/3, &r_ampers_per_line, &r_line);
    R_SplitX1(&r_line, UTFT_getDisplayXSize()/3, &r_seconds_per_line, &r_line);
    R_FillRectBack(&r_ampers_per_line);
    R_FillRectBack(&r_seconds_per_line);

    R_SplitY1(&r_all, UTF_Height(), &r_line, &r_all);

    R_SplitX1(&r_line, UTF_StringWidth("Triggered")+X_MARGIN*2, &r_triggered, &r_line);
    R_SplitX1(&r_line, UTF_StringWidth("Samples=")+X_MARGIN, &r_tmp, &r_line);
    R_SplitX1(&r_line, UTF_StringWidth("000000")+X_MARGIN, &r_buffer_samples, &r_line);
    R_DrawStringJustify(&r_tmp, "Samples=", UTF_RIGHT);

    {
        osc.lines_dx = 36;
        osc.lines_dy = 36;
        if(use_test)
        {
            osc.start = OscilloscopeStartTest;
            osc.value = OscilloscopeValueTest;
        } else
        {
            osc.start = OscilloscopeStartSum;
            osc.value = OscilloscopeValueSum;
        }

        int width = osc.lines_dx*12+1;
        int height = osc.lines_dx*6+1;
        int rest_width2 = (UTFT_getDisplayXSize() - width)/2;

        RectA r_osc;
        R_SplitY1(&r_all, height, &r_osc, &r_all);
        R_SplitX1(&r_osc, rest_width2, &r_tmp, &r_osc);
        R_SplitX1(&r_osc, width, &r_osc, &r_tmp);

        osc.pos = r_osc;

        enable_redraw = true;
    }

    InterfaceGoto(SceneOscilloscopeQuant);
}

void SceneOscilloscopeQuant()
{
    if(use_test)
    {
        phase += 0.01f;
        uint16_t start_us = TimeUs();
        OscilloscopeDraw(&osc);
        uint16_t delta_us = TimeUs() - start_us;
        char str[32];
        strcpy(str, "draw(us)=");
        catInt(str, delta_us);
        StatusbarSetTextAndRedraw(str);
    } else
    {
        if(enable_redraw)
        {
            enable_redraw = false;
            char str[32];
            strcpy(str, AmperPerLineInfo(g_settings.ampers_per_line_idx));
            strcat(str, "/div");
            R_DrawStringJustify(&r_ampers_per_line, str, UTF_CENTER);

            strcpy(str, SecondsPerLineInfo(g_settings.seconds_per_line_idx));
            strcat(str, "/div");
            R_DrawStringJustify(&r_seconds_per_line, str, UTF_CENTER);

            if(STCaptureCompleted())
                OscilloscopeDraw(&osc);
        }
    }


    if(EncButtonPressed())
    {
        MenuOscilloscopeStart();
        return;
    }

    if(EncValueChanged())
    {
        OscilloscopeEncoderQuant(EncValueDelta());
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

void SetOscilloscopeEncoder(OscilloscopeEncoderEnum type)
{
    g_settings.oscilloscope_encoder = type;
    enable_redraw = true;
}

void OscilloscopeEncoderQuant(int delta)
{
    if(g_settings.oscilloscope_encoder==ENCODER_SCALE_CURRENT)
    {
        int value = g_settings.ampers_per_line_idx;
        AddSaturated(&value, delta, AmperPerLineCount());
        g_settings.ampers_per_line_idx = value;
    }

    if(g_settings.oscilloscope_encoder==ENCODER_SCALE_TIME)
    {
        int value = g_settings.seconds_per_line_idx;
        AddSaturated(&value, delta, SecondsPerLineCount());
        g_settings.seconds_per_line_idx = value;
    }

    enable_redraw = true;
}
