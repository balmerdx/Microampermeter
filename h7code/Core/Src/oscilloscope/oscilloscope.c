#include "main.h"
#include "oscilloscope.h"

#include <sys/param.h>

#include "UTFT.h"

static uint16_t osc_colors[320];
static uint16_t osc_back_color = VGA_BLACK;
static uint16_t osc_line_color = VGA_OLIVE;
static uint16_t osc_line_special_color = UTFT_COLOR(190, 190, 0); //Выделенная линия, обычно какой-то 0
static uint16_t osc_graph_color = VGA_WHITE;
static uint16_t osc_graph_back_color = VGA_BLUE;
static int height;

static void DrawHLine(int y1, int y2, uint16_t color)
{
    int y_min = MIN(y1, y2);
    int y_max = MAX(y1, y2);

    if((y_min>=0 && y_min<height) || (y_max>=0 && y_max<height))
    {
        y_min = MAX(y_min, 0);
        y_max = MIN(y_max, height-1);
        for(int y=y_min; y<=y_max; y++)
            osc_colors[y] = color;
    }
}

void OscilloscopeDraw(OscilloscopeData* data)
{
    if(sizeof(osc_colors)/sizeof(osc_colors[0]) < data->pos.height)
        return;

    int width = data->pos.width;
    height = data->pos.height;

    //int line_special_x = (width/data->lines_dx/2)*data->lines_dx;
    //int line_special_y = (height/data->lines_dy)*data->lines_dy;

    data->start(data);
    OscilloscopeValue cur = data->value(data, 0);
    int line_x = 0;
    for(int x=0; x<width; x++)
    {
        uint16_t back_color = osc_back_color;
        if(x==line_x)
            back_color = osc_line_color;
        if(x==data->line_special_x)
            back_color = osc_line_special_color;

        int line_y = 0;
        for(int y=0; y<height; y++)
        {
            uint16_t color = back_color;
            if(y==line_y)
                color = osc_line_color;
            if(y==data->line_special_y)
                color = osc_line_special_color;

            osc_colors[y] = color;

            if(y==line_y)
                line_y += data->lines_dy;
        }

        {//Рисуем фон
            if(cur.y_max>=cur.y_min)
                DrawHLine(cur.y_min, cur.y_max, osc_graph_back_color);
        }

        {//Рисуем одну строчку линии
            OscilloscopeValue next = data->value(data, x+1);
            DrawHLine(cur.y, next.y, osc_graph_color);
            cur = next;
        }

        UTFT_drawData(data->pos.x+x, data->pos.y, 1, height, osc_colors);

        if(x==line_x)
        {
            line_x += data->lines_dx;
        }
    }
}
