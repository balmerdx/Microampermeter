#include "main.h"
#include "oscilloscope.h"

#include <sys/param.h>

#include "UTFT.h"

static uint16_t osc_colors[320];
static uint16_t osc_back_color = VGA_BLACK;
static uint16_t osc_line_color = VGA_OLIVE;
static uint16_t osc_line_special_color = VGA_YELLOW; //Выделенная линия, обычно какой-то 0
static uint16_t osc_graph_color = VGA_WHITE;

void OscilloscopeDraw(OscilloscopeData* data)
{
    if(sizeof(osc_colors)/sizeof(osc_colors[0]) < data->pos.height)
        return;

    int width = data->pos.width;
    int height = data->pos.height;

    int line_special_x = (width/data->lines_dx/2)*data->lines_dx;
    int line_special_y = (height/data->lines_dy)*data->lines_dy;

    data->start(data);
    int cur_y = data->value(data, 0);
    int line_x = 0;
    for(int x=0; x<width; x++)
    {
        uint16_t back_color = osc_back_color;
        if(x==line_x)
            back_color = osc_line_color;
        if(x==line_special_x)
            back_color = osc_line_special_color;

        int line_y = 0;
        for(int y=0; y<height; y++)
        {
            uint16_t color = back_color;
            if(y==line_y)
                color = osc_line_color;
            if(y==line_special_y)
                color = osc_line_special_color;

            osc_colors[y] = color;

            if(y==line_y)
                line_y += data->lines_dy;
        }

        {//Рисуем одну строчку линии
            int next_y = data->value(data, x+1);
            int y_min = MIN(cur_y, next_y);
            int y_max = MAX(cur_y, next_y);

            if((y_min>=0 && y_min<height) || (y_max>=0 && y_max<height))
            {
                y_min = MAX(y_min, 0);
                y_max = MIN(y_max, height-1);
                for(int y=y_min; y<=y_max; y++)
                    osc_colors[y] = osc_graph_color;
            }

            cur_y = next_y;
        }

        UTFT_drawData(data->pos.x+x, data->pos.y, 1, height, osc_colors);

        if(x==line_x)
        {
            line_x += data->lines_dx;
        }
    }
}
