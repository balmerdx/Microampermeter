#include "main.h"
#include <sys/param.h>
#include <string.h>

#include "scene_histogram.h"
#include "scene_single.h"
#include "histogram/histogram_plot.h"
#include "menu_root_histogram.h"
#include "menu_bandwidth.h"
#include "menu_time_histogram.h"
#include "gui_settings.h"
#include "measure/mid_big_interval.h"
#include "measure/calculate.h"

#include "UTFT.h"
#include "float_to_string.h"
#include "interface/interface.h"
#include "interface/statusbar.h"
#include "images/images.h"

static void SceneHistogramQuant();

static RectA r_battery;
static RectA r_current;
static RectA r_voltage;

static float ymin = 0, ymax = 105;

bool enable_histogramm = false;
static int samples_to_complete = 1000;
static float samples_to_complete_inv_percent = 0;
static int samples_current = 0;
static HistogramData g_cur_data;
static HistogramData g_complete_data;
static bool g_complete_data_filed = false;


//Не забываем, что эта функция вызывается из обработчика прерывания
void OnFilterNextSampleHistogramm(float current)
{
    if(!enable_histogramm)
        return;

    if(samples_current < samples_to_complete)
    {
        g_cur_data.data[HistogramCurrentToBasketIndex(fabsf(current))] += samples_to_complete_inv_percent;
        samples_current++;
    }

    if(samples_current>=samples_to_complete && !g_complete_data_filed)
    {
        g_complete_data = g_cur_data;
        HistogramDataClear(&g_cur_data);
        g_complete_data_filed = true;
        samples_current = 0;
    }
}

void SceneHistogramStart()
{
    {
        float sum_time = GetTimeHistogram();
        samples_to_complete = lroundf(FilterSPS() * sum_time);
    }

    RectA r_tmp = R_DisplaySize();
    RectA r_hist;
    RectA r_top, r_bottom;
    R_SplitY1(&r_tmp, UTF_Height(), &r_top, &r_tmp);
    R_SplitY2(&r_tmp, UTF_Height(), &r_hist, &r_bottom);

    UTF_SetFont(g_default_font);
    {
        RectA r_header;
        RectA r_bandwidth;
        int width = 0;
        for(int i=FilterX_1; i<=FilterX_1024 ;i++)
        {
            width = MAX(width, UTF_StringWidth(FilterXBandwidth(i))+X_MARGIN*2);
        }

        r_top.back_color = STATUSBAR_BACKGROUND;
        R_SplitX2(&r_top, batery_full_img.width, &r_top, &r_battery);
        R_SplitX2(&r_top, width, &r_header, &r_bandwidth);

        UTFT_setColor(VGA_WHITE);
        R_DrawStringJustify(&r_header, "Histogram", UTF_CENTER);

        R_DrawStringJustify(&r_bandwidth, FilterXBandwidth(g_filterX), UTF_CENTER);
    }

    R_SplitX1(&r_bottom, r_bottom.width/2, &r_current, &r_voltage);

    R_DrawStringJustify(&r_current, "Current", UTF_CENTER);
    R_DrawStringJustify(&r_voltage, "Voltage", UTF_CENTER);
    UpdateVbatLow(&r_battery);

    HistogramPlotInit(&r_hist, HISTOGRAMM_BASKETS);
    HistogramSetAxisY(ymin, ymax);

    HistogramDataClear(&g_cur_data);
    HistogramDataClear(&g_complete_data);

    samples_to_complete = GetTimeHistogram() * FilterSPS();
    samples_to_complete_inv_percent = 100.f/samples_to_complete;
    samples_current = 0;

    g_complete_data_filed = false;
    enable_histogramm = true;

    InterfaceGoto(SceneHistogramQuant);
}

void DrawMid()
{
    MidData d = GetMidData();
    CalcResult calc_result;
    calculateRV(d.voltage, d.current,
                   GetResistorValue(GetResistor()), &calc_result);

    int places = 4;
    char st[50];
    st[0] = 0;
    float mul;
    char* suffix = CurrentSuffix(calc_result.current, &mul);
    catFloatFixed(st, sizeof(st), calc_result.current*mul, places);
    strcat(st, " ");
    strcat(st, suffix);

    UTF_SetFont(g_default_font);
    R_DrawStringJustify(&r_current, st, UTF_CENTER);


    floatToString(st, 27, calc_result.Vout, 4, 0, false);
    strcat(st, " V");
    R_DrawStringJustify(&r_voltage, st, UTF_CENTER);
}

void SceneHistogramQuant()
{
    if(EncButtonPressed())
    {
        MenuRootHistogramStart();
        return;
    }

    if(g_complete_data_filed)
    {
        g_complete_data_filed = false;
        HistogramPlot(g_complete_data.data);
        DrawMid();
    }

    UpdateVbatLow(&r_battery);
}
