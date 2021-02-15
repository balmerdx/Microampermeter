#include "main.h"

#include <math.h>
#include <string.h>
#include "float_to_string.h"
#include "histogram_plot.h"
#include "fonts/font_8x15.h"
#include "interface/colors.h"

static uint16_t g_front_color = VGA_WHITE;
static uint16_t g_back_color = VGA_BLACK;
static uint16_t g_outline_color = COLOR_FOREGROUND_DARK_GRAY;

static RectA g_rect;
static RectA g_left_tick;
static RectA g_bottom_tick;
static RectA g_inner_rect;
static const int tick_len = 5;
static uint16_t g_left_axe_chars = 3;

static int g_baskets;
static int mul_x = 1;
static float mul_y = 1;

static const char* const str_tick_bottom[] = {
    "10 nA",
    "100 nA",
    "1 uA",
    "10 uA",
    "100 uA",
    "1 mA",
    "10 mA",
    "100", //mA не влезает, не пишем
};

static void DrawHorizontalTicks();
static void DrawVerticalTicks(float min_y, float max_y);

static void HistogramPlotSetFont()
{
    UTF_SetFont(font_8x15);
}


void HistogramPlotInit(RectA* rect, int baskets)
{
    g_rect = *rect;
    g_inner_rect = *rect;
    HistogramPlotSetFont();
    R_SplitY2(rect, UTF_Height()+tick_len+1, &g_inner_rect, &g_bottom_tick);
    R_SplitX1(&g_inner_rect, UTF_StringWidth("0")*g_left_axe_chars+tick_len+1, &g_left_tick, &g_inner_rect);

    g_baskets = baskets;
    mul_x = g_inner_rect.width / g_baskets;

    g_left_tick.back_color = VGA_NAVY;
    R_FillRectBack(&g_left_tick);
    R_FillRectBack(&g_bottom_tick);

    DrawHorizontalTicks();
}

void HistogramPlot(float* data)
{
    float max_y = 1e-9f;
    float min_y = 0;
    for(int i=0; i<g_baskets; i++)
    {
        float y = data[i];
        if(y > max_y)
            max_y = y;
    }

    mul_y = g_inner_rect.height/(max_y-min_y);

    HistogramPlotSetFont();
    UTFT_setColor(g_front_color);
    UTFT_setBackColor(g_back_color);
    int y2 = g_inner_rect.y + g_inner_rect.height - 1;
    for(int i=0; i<g_baskets; i++)
    {
        int x1 = g_inner_rect.x + i * mul_x;
        //int x2 = x1 + mul_x - 1;
        int x2 = x1 + mul_x - 2;
        float yf = data[i];
        int y = g_inner_rect.y + g_inner_rect.height  - lroundf((yf-min_y)*mul_y);

        if(y > g_inner_rect.y)
            UTFT_fillRectBack(x1, g_inner_rect.y, x2, y-1);
        if(y < y2)
            UTFT_fillRect(x1, y, x2, y2);
    }


    int xlast = g_inner_rect.x + g_baskets * mul_x;
    UTFT_fillRectBack(xlast, g_inner_rect.y, g_inner_rect.x+g_inner_rect.width-1, y2);

    DrawVerticalTicks(min_y, max_y);
}

void DrawHorizontalTicks()
{
    UTFT_setColor(g_outline_color);
    UTFT_drawLine(g_bottom_tick.x, g_bottom_tick.y, g_bottom_tick.y+g_bottom_tick.width-1, g_bottom_tick.y);
    int y1 = g_bottom_tick.y+1;
    int y2 = y1 + tick_len - 1;
    int ystr = y2 + 1;

    for(int i=0; i<=g_baskets; i+=HISTOGRAMM_BASKET_PER_DECADE)
    {
        int x = g_inner_rect.x + i * mul_x;

        UTFT_setColor(g_outline_color);
        UTFT_drawLine(x, y1, x, y2);

        UTFT_setColor(g_outline_color);
        UTFT_setBackColor(g_back_color);

        int str_bottom_count = sizeof(str_tick_bottom)/sizeof(str_tick_bottom[0]);
        int str_idx = i/HISTOGRAMM_BASKET_PER_DECADE;
        if(str_idx < str_bottom_count)
        {
            const char* str = str_tick_bottom[str_idx];
            int width = UTF_StringWidth(str);
            if(i==g_baskets)
            {
                UTF_DrawStringJustify(x-width, ystr, str, width, UTF_LEFT);
            } else
            {
                UTF_DrawStringJustify(x-width/2, ystr, str, width, UTF_LEFT);
            }
        }
    }
}

//Рисуем одну рисочку с числом.
//Она рисуется всегда прямоугольником по ширине g_left_tick.width
//Если цифра выходит за пределы разрешенного прямоугольника, то она не рисуется.
static void DrawVerticalTick(int y, int number, int* out_ymin, int* out_ymax)
{
    int x2 = g_left_tick.x + g_left_tick.width - 1;
    int x1 = x2 - tick_len;
    UTFT_setColor(g_outline_color);
    UTFT_drawLine(x1, y, x2, y);

    int ymin = y - UTF_Height()/2;
    int ymax = ymin + UTF_Height()-1;
    if(ymin>=g_inner_rect.y && ymax<g_inner_rect.y+g_inner_rect.height)
    {
        //Заполняем пространство сверху и снизу от tick
        UTFT_fillRectBack(x1, ymin, x2, y-1);
        UTFT_fillRectBack(x1, y+1, x2, ymax);

        int text_width = g_left_tick.width - tick_len - 1;
        char str[8];
        intToString(str, number, 0, NUM_SPACE);
        //Тонкая линия после текста
        UTFT_fillRectBack(g_left_tick.x+text_width, ymin, g_left_tick.x+text_width, ymax);
        UTF_DrawStringJustify(g_left_tick.x, ymin, str, text_width, UTF_RIGHT);
    } else
    {
        //Однопиксельная высота
        ymax = ymin = y;
        UTFT_fillRectBack(g_left_tick.x, ymin, x1-1, ymin);
    }

    *out_ymin = ymin;
    *out_ymax = ymax;
}

void DrawVerticalTicks(float min_y, float max_y)
{
    //Предполагаем, что min_y==0
    //mul_y - определено при вызова DrawVerticalTicks
    //Эта функция должна перерисовывать всю левую сторону.
    //Причем делать это последовательно, без мерцания.
    //Стараемся нарисовать не менее двух и не более 15-ти чисел.
    //Числа от 1 до 9. На сколько они должны умножаться - пишется где-то в отдельном месте.

    float lg = log10f(max_y);
    float digits = floorf(lg);
    float dy = powf(10.f, digits);
    float ykoeff = 1.f/dy;
    if(max_y > 5*dy)
    {
        //ok
    } else
    {
        dy *= 0.1f;
        ykoeff *= 10;
        if(max_y > 20*dy)
        {
            dy *= 5;
        } else
        if(max_y > 10*dy)
        {
            dy *= 2;
        }
    }

    HistogramPlotSetFont();
    UTFT_setBackColor(g_back_color);
    int last_ymax = g_inner_rect.y + g_inner_rect.height - 1;

    for(float yf=0; yf<max_y; yf += dy)
    {
        int y = g_inner_rect.y + g_inner_rect.height  - ceilf((yf-min_y)*mul_y);
        int number = lroundf(yf*ykoeff);
        int ymin, ymax;
        DrawVerticalTick(y, number, &ymin, &ymax);

        if(ymax < last_ymax)
        {
            UTFT_fillRectBack(g_left_tick.x, ymax, g_left_tick.x+g_left_tick.width-1, last_ymax);
            last_ymax = ymin - 1;
        }
    }

    if(g_left_tick.y < last_ymax)
    {
        UTFT_fillRectBack(g_left_tick.x, g_left_tick.y, g_left_tick.x+g_left_tick.width-1, last_ymax);
    }

}

