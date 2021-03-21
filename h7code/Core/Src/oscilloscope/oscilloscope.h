#pragma once
#include "interface/rect_utils.h"

typedef struct OscilloscopeData OscilloscopeData;

typedef struct OscilloscopeValue
{
    int y;
    int y_min;
    int y_max;
} OscilloscopeValue;

typedef void (*OscilloscopeStartCallback)(OscilloscopeData* data);
typedef OscilloscopeValue (*OscilloscopeValueCallback)(OscilloscopeData* data, int x);


typedef struct OscilloscopeData
{
    RectA pos;
    //Расстояние между линиями по горизонтали, вертикали.
    //Первая линия всегда в начале.
    int16_t lines_dx;
    int16_t lines_dy;

    OscilloscopeStartCallback start;
    OscilloscopeValueCallback value;

    //internal data (заполняется при start, либо раньше)
    int line_special_x;
    int line_special_y;

} OscilloscopeData;



void OscilloscopeDraw(OscilloscopeData* data);
