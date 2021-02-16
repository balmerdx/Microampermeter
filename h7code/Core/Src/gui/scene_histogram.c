#include "main.h"
#include "scene_histogram.h"
#include "histogram/histogram_plot.h"

#include "UTFT.h"
#include "interface/interface.h"
#include "interface/statusbar.h"

static void SceneHistogramQuant();

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
        g_cur_data.data[HistogramCurrentToBasketIndex(current)] += samples_to_complete_inv_percent;
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

    RectA r_hist = R_DisplaySizeWithoutHeaderAndStatusbar();

    HistogramPlotInit(&r_hist, HISTOGRAMM_BASKETS);
    HistogramSetAxisY(ymin, ymax);
    HistogramPlot(data);

    HeaderSetTextAndRedraw("Histogram");
    StatusbarSetTextAndRedraw("Statusbar");

    HistogramDataClear(&g_cur_data);
    HistogramDataClear(&g_complete_data);

    samples_to_complete_inv_percent = 100.f/samples_to_complete;
    enable_histogramm = true;


    InterfaceGoto(SceneHistogramQuant);
}

float phase = 0;
void SceneHistogramQuant()
{
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
}
