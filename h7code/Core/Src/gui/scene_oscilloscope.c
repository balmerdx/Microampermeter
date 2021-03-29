#include "main.h"
#include "scene_oscilloscope.h"
#include "mul_coeff_per_line.h"

#include "UTFT.h"
#include "float_to_string.h"

#include "interface/interface.h"
#include "interface/statusbar.h"
#include "interface/progress_bar.h"
#include "menu_oscilloscope.h"
#include "gui_settings.h"
#include "measure/store_and_trigger.h"
#include "measure/receive_data.h"
#include "measure/settings.h"
#include "oscilloscope/oscilloscope.h"
#include "scene_single.h"

#include <string.h>
#include <sys/param.h>

static void SceneOscilloscopeQuant();
static void OscilloscopeEncoderQuant(int delta);
static void UpdateTriggerColor();

static RectA r_triggered;
static RectA r_buffer_samples;
static RectA r_ampers_per_line;
static RectA r_seconds_per_line;
static RectA r_seconds_offset;
//Два квадрата триггера, которые заливаем цветом.
//Желтый - триггер не стартовал
//Зелёный - стартовал
//Черный - триггер окончил работу
static RectA r_trigger_left;
static RectA r_trigger_right;
static OscilloscopeData osc;

static bool enable_redraw = false;
static bool completed_redrawed = true;

//Коэффициент масштабирования по оси Y (ампер на lines_dy)
static float amper_per_line = 1e-3f;
//Сдвиг графика по времени относительно времени срабатывения триггера.
static float seconds_trigger_offset = 0;


static int half_oscillograph_width = 1;
static float samples_per_pixel = 1;// ось X
static int samples_trigger_offset = 1;
static float y_mul = 1; //ampers per pixel

static bool use_progressbar = true;

static void TestFill()
{
    uint32_t size = STBufferCapacity();
    for(uint32_t i=0; i<size; i++)
    {
        STTestAdd(sinf(i*0.01f)*50e-6f);
    }

    STTestSetCaptureCompleted();
}

static int PixelToBufferOffset(int x)
{
    return lroundf(STTriggerOffset() + (x-half_oscillograph_width)*samples_per_pixel + samples_trigger_offset);
}

static int BufferOffsetToPixel(int offs)
{
    return (offs - samples_trigger_offset - (int)STTriggerOffset())/samples_per_pixel + half_oscillograph_width;
}

static float CalcSecondsPerPixel()
{
    //Коэффициент масштабирования по оси X (секунд на lines_dx)
    float seconds_per_line = SecondsPerLineCurrent(g_settings.seconds_per_line_idx);
    return seconds_per_line / osc.lines_dx;
}

static float CalcSamplesPerPixel()
{
    return STFilterSPS() * CalcSecondsPerPixel();
}

static void OscilloscopeStartSum(OscilloscopeData* data)
{
    amper_per_line = AmperPerLineCurrent(g_settings.ampers_per_line_idx);

    half_oscillograph_width = (data->pos.width/data->lines_dx/2)*data->lines_dx;
    samples_per_pixel = CalcSamplesPerPixel();
    samples_trigger_offset = STFilterSPS() * seconds_trigger_offset;
    y_mul = osc.lines_dy / amper_per_line;

    data->line_special_x = BufferOffsetToPixel(STTriggerOffset());
    data->line_special_y = (data->pos.height/data->lines_dy)*data->lines_dy;
}

static float cur_value;
static float cur_min;
static float cur_max;
static int cur_count;
static void IterateIntervalCallback(float sample, void* param)
{
    if(cur_count==0)
    {
        cur_min = cur_max = sample;
    } else
    {
        if(sample < cur_min)
            cur_min = sample;
        if(sample > cur_max)
            cur_max = sample;
    }

    cur_value += sample;
    cur_count++;
}


static int YValueToPixel(OscilloscopeData* data, float y)
{
    y  *= y_mul;
    if(y<0)
        y = -1;
    if(y>data->pos.height)
        y = data->pos.height+1;

    return (data->pos.height-1) - y;
}

static OscilloscopeValue OscilloscopeValueSum(OscilloscopeData* data, int x)
{
    cur_value = 0;
    cur_count = 0;
    cur_min = cur_max = 0;
    int istart = PixelToBufferOffset(x);
    int iend = PixelToBufferOffset(x+1);
    istart = MAX(istart, 0);
    iend = MAX(iend, istart+1);
    iend = MIN(iend, (int)STSamplesCaptured());

    STIterate(istart, iend, IterateIntervalCallback, 0);
    float y = cur_count ? (cur_value/cur_count) : 0;

    OscilloscopeValue ret;
    ret.y = YValueToPixel(data, y);
    ret.y_min = YValueToPixel(data, cur_max);
    ret.y_max = YValueToPixel(data, cur_min);
    return ret;
}

void OscilloscopeTriggerStart()
{
    seconds_trigger_offset = 0;
    STCaptureStart();
    EnableCapturingTrigger(); //test
    enable_redraw = true;
    completed_redrawed = false;
}

void SceneOscilloscopeStart()
{
    RectA r_all, r_tmp, r_line;
    DrawHeaderAndGetVbatRect("Oscilloscope", &r_all);
    R_SplitY2(&r_all, UTF_Height(), &r_all, &r_line);
    R_FillRectBack(&r_all);

    r_line.back_color = COLOR_BACKGROUND_BLUE;
    R_SplitX1(&r_line, UTFT_getDisplayXSize()/3, &r_ampers_per_line, &r_line);
    R_SplitX1(&r_line, UTFT_getDisplayXSize()/3, &r_seconds_per_line, &r_seconds_offset);
    R_FillRectBack(&r_ampers_per_line);
    R_FillRectBack(&r_seconds_per_line);
    R_FillRectBack(&r_seconds_offset);


    R_SplitY2(&r_all, UTF_Height(), &r_all, &r_line);

    if(use_progressbar)
    {
        ProgressInit(r_line.x+X_MARGIN, r_line.y+X_MARGIN, r_line.width-X_MARGIN*2, X_MARGIN*2);
        ProgressSetPos(0);
        ProgressSetVisible(true);
    } else
    {
        R_SplitX1(&r_line, UTF_StringWidth("Triggered")+X_MARGIN*2, &r_triggered, &r_line);
        R_SplitX1(&r_line, UTF_StringWidth("Samples=")+X_MARGIN, &r_tmp, &r_line);
        R_SplitX1(&r_line, UTF_StringWidth("000000")+X_MARGIN, &r_buffer_samples, &r_line);
        R_DrawStringJustify(&r_tmp, "Samples=", UTF_RIGHT);
    }

    {
        osc.lines_dx = 36;
        osc.lines_dy = 36;
\
        {
            static bool first = true;
            if(first)
            {
                first = false;
                //TestFill();
            }
        }

        osc.start = OscilloscopeStartSum;
        osc.value = OscilloscopeValueSum;

        int width = osc.lines_dx*12+1;
        int height = osc.lines_dx*6+1;
        int rest_width2 = (UTFT_getDisplayXSize() - width)/2;

        RectA r_osc;
        R_SplitY2(&r_all, height, &r_all, &r_osc);
        R_SplitX1(&r_osc, rest_width2, &r_trigger_left, &r_osc);
        R_SplitX1(&r_osc, width, &r_osc, &r_trigger_right);

        osc.pos = r_osc;

        enable_redraw = true;
    }

    InterfaceGoto(SceneOscilloscopeQuant);
}

static void SceneOscilloscopeUpdateInfo()
{
    UTFT_setColor(VGA_WHITE);
    char str[64];
    r_ampers_per_line.back_color = (g_settings.oscilloscope_encoder==ENCODER_SCALE_CURRENT)?COLOR_BACKGROUND_RED:COLOR_BACKGROUND_BLUE;
    r_seconds_per_line.back_color = (g_settings.oscilloscope_encoder==ENCODER_SCALE_TIME)?COLOR_BACKGROUND_RED:COLOR_BACKGROUND_BLUE;
    r_seconds_offset.back_color = (g_settings.oscilloscope_encoder==ENCODER_OFFSET_TIME)?COLOR_BACKGROUND_RED:COLOR_BACKGROUND_BLUE;

    strcpy(str, AmperPerLineInfo(g_settings.ampers_per_line_idx));
    strcat(str, "/div");
    R_DrawStringJustify(&r_ampers_per_line, str, UTF_CENTER);

    strcpy(str, SecondsPerLineInfo(g_settings.seconds_per_line_idx));
    strcat(str, "/div");
    R_DrawStringJustify(&r_seconds_per_line, str, UTF_CENTER);

    int offset_ms = lroundf(seconds_trigger_offset*1e3f);
    strcpy(str, "offs:");
    catInt(str, offset_ms);
    strcat(str, " ms");
    R_DrawStringJustify(&r_seconds_offset, str, UTF_CENTER);
}

static void SceneOscilloscopeQuant()
{

    if(enable_redraw)
    {
        bool enable = STCaptureCompleted();
        if(!enable && STTriggerTriggered())
        {
            float samples_per_pixel = CalcSamplesPerPixel();
            uint32_t half_samples = samples_per_pixel * osc.pos.width/2;
            if(STTriggerOffset() + half_samples < STSamplesCaptured())
                enable = true;
        }

        if(enable)
        {
            enable_redraw = false;
            OscilloscopeDraw(&osc);
            SceneOscilloscopeUpdateInfo();
        }
    }

    if(STCaptureCompleted() && !completed_redrawed)
    {
        enable_redraw = true;
        completed_redrawed = true;
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

    UpdateTriggerColor();

    if(use_progressbar)
    {
        if(STCaptureCompleted())
        {
            ProgressSetPos((float)(samples_trigger_offset+STTriggerOffset())/(float)STBufferCapacity());
        } else
        {
            ProgressSetPos((float)STSamplesCaptured()/(float)STBufferCapacity());
        }
    } else
    {
        char* str = "Off";
        if(STCaptureCompleted())
            str = "Completed";
        else if(STTriggerTriggered())
            str = "Triggered";
        else if(STCaptureStarted())
            str = "Started";

        R_DrawStringJustify(&r_triggered, str, UTF_CENTER);
        R_printNumI(&r_buffer_samples, STSamplesCaptured(), UTF_LEFT);
    }

    UpdateVbatLow();
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

    if(g_settings.oscilloscope_encoder==ENCODER_OFFSET_TIME)
    {
        float seconds_per_pixel = CalcSecondsPerPixel();
        float seconds_border_left = half_oscillograph_width*seconds_per_pixel;
        float seconds_border_right = osc.pos.width*seconds_per_pixel;
        float seconds_min = -(float)STTriggerOffset() / STFilterSPS() + seconds_border_left;
        seconds_min = MIN(seconds_min, 0);
        float seconds_max = seconds_min + STSamplesCaptured() / STFilterSPS() -  seconds_border_right;
        seconds_max = MAX(seconds_max, 0);

        if(seconds_min < seconds_max)
        {
            seconds_trigger_offset += delta * seconds_per_pixel * osc.lines_dx;
            if(seconds_trigger_offset < seconds_min)
                seconds_trigger_offset = seconds_min;
            if(seconds_trigger_offset > seconds_max)
                seconds_trigger_offset = seconds_max;
        }
    }

    SceneOscilloscopeUpdateInfo();

    enable_redraw = true;
}

static void UpdateTriggerColor()
{
    int16_t new_color = VGA_BLACK;
    if(STCaptureCompleted())
        new_color = VGA_BLACK;
    else if(STTriggerTriggered())
        new_color = VGA_GREEN;
    else if(STCaptureStarted())
        new_color = VGA_RED;

    if(r_trigger_left.back_color == new_color)
        return;

    r_trigger_left.back_color = new_color;
    r_trigger_right.back_color = new_color;

    R_FillRectBack(&r_trigger_left);
    R_FillRectBack(&r_trigger_right);
}
