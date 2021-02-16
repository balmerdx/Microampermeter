#pragma once
#include "UTFT.h"
#include "interface/rect_utils.h"
#include "histogram_data.h"

//Желательно, что-бы rect делился на HISTOGRAMM_BASKETS
void HistogramPlotInit(RectA* rect, int baskets);

//Вызывать после HistogramPlotInit но до HistogramPlot
//Рисует внутри себя ось Y
//Можно вызывать часто, она нацеленна на красивую перерисовку без мерцания
void HistogramSetAxisY(float in_min_y, float in_max_y);

void HistogramPlot(float* data);
