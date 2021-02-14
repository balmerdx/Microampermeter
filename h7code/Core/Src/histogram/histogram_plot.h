#pragma once
#include "UTFT.h"
#include "interface/rect_utils.h"
#include "histogram_data.h"

//Желательно, что-бы rect делился на HISTOGRAMM_BASKETS
void HistogramPlotInit(RectA* rect, int baskets);
void HistogramPlot(float* data);
