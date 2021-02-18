#include "main.h"
#include <sys/param.h>

#include "scene_histogram.h"
#include "scene_single.h"
#include "histogram/histogram_plot.h"
#include "menu_root_histogram.h"
#include "menu_bandwidth.h"
#include "menu_time_histogram.h"
#include "gui_settings.h"

#include "UTFT.h"
#include "interface/interface.h"
#include "interface/statusbar.h"
#include "images/images.h"

static void SceneHistogramQuant();

static RectA r_battery;

static float data[HISTOGRAMM_BASKETS];

static float ymin = 0, ymax = 105;

static bool enable_histogramm = false;
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
    for(int i=0; i<HISTOGRAMM_BASKETS; i++)
        data[i] = i*i;

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

    R_DrawStringJustify(&r_bottom, "Statusbar", UTF_CENTER);
    UpdateVbatLow(&r_battery);

    HistogramPlotInit(&r_hist, HISTOGRAMM_BASKETS);
    HistogramSetAxisY(ymin, ymax);
    HistogramPlot(data);

    HistogramDataClear(&g_cur_data);
    HistogramDataClear(&g_complete_data);

    samples_to_complete = GetTimeHistogram() * FilterSPS();
    samples_to_complete_inv_percent = 100.f/samples_to_complete;
    enable_histogramm = true;


    InterfaceGoto(SceneHistogramQuant);
}

float phase = 0;
void SceneHistogramQuant()
{
    if(EncButtonPressed())
    {
        MenuRootHistogramStart();
        return;
    }

    if(0)
    {
        phase += 0.01f;
        for(int i=0; i<HISTOGRAMM_BASKETS; i++)
            data[i] = (sinf(i*0.03f+phase)+1)*0.5f*(ymax-ymin)+ymin;
        HistogramPlot(data);
    } else
    {
        g_complete_data_filed = false;
        HistogramPlot(g_complete_data.data);
    }

    UpdateVbatLow(&r_battery);
}
