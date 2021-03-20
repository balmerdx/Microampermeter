#pragma once
#include "interface/rect_utils.h"

typedef struct OscilloscopeData OscilloscopeData;

typedef void (*OscilloscopeStartCallback)(OscilloscopeData* data);
typedef int (*OscilloscopeValueCallback)(OscilloscopeData* data, int x);


typedef struct OscilloscopeData
{
    RectA pos;
    //Расстояние между линиями по горизонтали, вертикали.
    //Первая линия всегда в начале.
    int16_t lines_dx;
    int16_t lines_dy;

    OscilloscopeStartCallback start;
    OscilloscopeValueCallback value;
} OscilloscopeData;



void OscilloscopeDraw(OscilloscopeData* data);
