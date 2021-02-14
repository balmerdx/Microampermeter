#include "main.h"
#include "scene_histogram.h"
#include "histogram/histogram_plot.h"

#include "UTFT.h"
#include "interface/interface.h"
#include "interface/statusbar.h"

static void SceneHistogramQuant();

static float data[HISTOGRAMM_BASKETS];

void SceneHistogramStart()
{
    for(int i=0; i<HISTOGRAMM_BASKETS; i++)
        data[i] = i*i;

    RectA r_hist = R_DisplaySizeWithoutHeaderAndStatusbar();

    HistogramPlotInit(&r_hist, HISTOGRAMM_BASKETS);
    HistogramPlot(data);

    HeaderSetTextAndRedraw("Histogram");
    StatusbarSetTextAndRedraw("Statusbar");

    InterfaceGoto(SceneHistogramQuant);
}

float phase = 0;
void SceneHistogramQuant()
{
    phase += 0.01f;

    for(int i=0; i<HISTOGRAMM_BASKETS; i++)
        data[i] = sinf(i*0.03f+phase)+1;
    HistogramPlot(data);
}
