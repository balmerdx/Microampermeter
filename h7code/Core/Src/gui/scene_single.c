#include "main.h"
#include <string.h>
#include <sys/param.h>

#include "scene_single.h"

#include "UTFT.h"
#include "float_to_string.h"
#include "fonts/font_condensed30.h"
#include "font_big_nums.h"
#include "interface/interface.h"
#include "interface/rect_utils.h"
#include "interface/statusbar.h"

#include "measure/receive_data.h"
#include "measure/calculate.h"
#include "measure/mid_big_interval.h"

typedef enum
{
    LINE2_RESISTANCE,
    LINE2_CURRENT_MIN_MAX,
} LINE2_TYPE;

#define COLOR_BACK1 VGA_TEAL
#define COLOR_BACK2 VGA_NAVY
const int X_MARGIN = 2; //Столько места желательно оставлять справа/слева от надписи

const bool enable_percent_view = true;

LINE2_TYPE line2_type = LINE2_CURRENT_MIN_MAX;//LINE2_RESISTANCE;

static RectA r_shunt;
static RectA r_vout;
static RectA r_num_current;
static RectA r_param_current;

static RectA r_percent; //Процент использования времени в IRQ

//Только если line2_type == LINE2_RESISTANCE
static RectA r_num_resistance;
static RectA r_param_resistance;

//Только если line2_type == LINE2_CURRENT_MIN_MAX
static RectA r_current_min;
static RectA r_current_max;

static void SceneSingleQuant();

static uint32_t prev_draw_time;

void SceneSingleStart()
{
    UTFT_setColor(VGA_WHITE);
    UTF_SetFont(g_default_font);

    RectA r_tmp = R_DisplaySize();
    RectA r_top;
    R_SplitY1(&r_tmp, HeaderYEnd(), &r_top, &r_tmp);

    HeaderSetTextAndRedraw("Microampermeter");

    {
        //Нижняя строка.
        //Разместим пока на ней
        //SHUNT=1 KOm  - сопротивление текущего используемого шунта
        //Vout=1.222 V - напряжение выходное
        RectA r_bottom;
        R_SplitY2(&r_tmp, UTF_Height(), &r_tmp, &r_bottom);

        if(enable_percent_view)
        {
            R_SplitX2(&r_bottom, UTF_StringWidth("99.9%"), &r_bottom, &r_percent);
            r_percent.back_color = COLOR_BACKGROUND_DARK_GRAY;
        }

        R_SplitX1(&r_bottom, r_bottom.width/2, &r_shunt, &r_vout);
        r_shunt.back_color = COLOR_BACK1;
        r_vout.back_color = COLOR_BACK2;

    }

    {
        //В центре пускай будет две строки.
        //Ток и сопротивление большими буквами.
        //Сверху и снизу, справа и слева закрасим.
        int width_param = UTF_StringWidth("mA");
        width_param = MAX(width_param, UTF_StringWidth("KOm"));
        width_param = MAX(width_param, UTF_StringWidth("MOm"));

        int width_spaces = UTF_StringWidth("  ");

        UTF_SetFont(font_big_nums);
        int width_number = UTF_StringWidth("-0.0000");

        RectA r_fill;
        //Обрезаем по вертикали
        int sub_height = (r_tmp.height-UTF_Height()*2)/2;
        R_SplitY1(&r_tmp, sub_height, &r_fill, &r_tmp);
        //r_fill.back_color = VGA_MAROON;
        R_FillRectBack(&r_fill);
        R_SplitY2(&r_tmp, sub_height, &r_tmp, &r_fill);
        //r_fill.back_color = VGA_MAROON;
        R_FillRectBack(&r_fill);

        //Обрезаем по горизонтали.
        int sub_width = (r_tmp.width-width_param-width_spaces-width_number)/2;
        R_SplitX1(&r_tmp, sub_width, &r_fill, &r_tmp);
        R_FillRectBack(&r_fill);
        R_SplitX2(&r_tmp, sub_width, &r_tmp, &r_fill);
        R_FillRectBack(&r_fill);

        RectA r_line1, r_line2;
        //Делим по горизонтали на две линии
        R_SplitY1(&r_tmp, UTF_Height(), &r_line1, &r_line2);

        R_SplitX1(&r_line1, width_number, &r_num_current, &r_tmp);
        R_SplitX1(&r_tmp, width_spaces, &r_fill, &r_param_current);
        R_FillRectBack(&r_fill);

        //r_num_current.back_color = VGA_GREEN;
        R_FillRectBack(&r_num_current);
        //r_param_current.back_color = VGA_OLIVE;
        R_FillRectBack(&r_param_current);

        if(line2_type == LINE2_RESISTANCE)
        {
            R_SplitX1(&r_line2, width_number, &r_num_resistance, &r_tmp);
            R_SplitX1(&r_tmp, width_spaces, &r_fill, &r_param_resistance);
            R_FillRectBack(&r_fill);

            //r_num_resistance.back_color = VGA_BLUE;
            R_FillRectBack(&r_num_resistance);
            //r_param_resistance.back_color = VGA_MAROON;
            R_FillRectBack(&r_param_resistance);
        }

        if(line2_type == LINE2_CURRENT_MIN_MAX)
        {
            UTF_SetFont(g_default_font);
            R_SplitY1(&r_line2, UTF_Height(), &r_current_min, &r_line2);
            R_SplitY1(&r_line2, UTF_Height(), &r_current_max, &r_line2);
            //r_line2.back_color = VGA_OLIVE;
            R_FillRectBack(&r_line2);
            //r_current_min.back_color = VGA_BLUE;
            R_FillRectBack(&r_current_min);
            //r_current_max.back_color = VGA_MAROON;
            R_FillRectBack(&r_current_max);
        }
    }

    prev_draw_time = TimeMs();
    InterfaceGoto(SceneSingleQuant);
}

void DrawResultOld()
{
    UTF_SetFont(font_condensed30);
    int x, y;
    int xstart = 10;
    int yoffset = 30;
    y = 0;

    MidData d = GetMidData();
    CalcResult calc_result;
    calculate(d.adc_V, d.adc_I,
                   GetResistorValue(d.r), &calc_result);

    x = UTF_DrawString(xstart, y, "count=");
    x = UTF_printNumI(d.samples_count, x, y, 100, UTF_RIGHT);
    y += yoffset;
/*
    x = UTF_DrawString(xstart, y, "ADC0=");
    x = UTF_printNumI(d.adc0_mid, x, y, 100, UTF_RIGHT);
    y += yoffset;

    x = UTF_DrawString(xstart, y, "ADC1=");
    x = UTF_printNumI(d.adc1_mid, x, y, 100, UTF_RIGHT);
    y += yoffset;
*/
    x = UTF_DrawString(xstart, y, "interrupt=");
    x = UTF_printNumF(GetPercentInInterrupt(), x, y, 2, UTF_StringWidth("00.00"), UTF_RIGHT);
    x = UTF_DrawString(x, y, "%");
    y += yoffset;

    x = UTF_DrawString(xstart, y, "Vcurrent(mV)=");
    x = UTF_printNumF(calc_result.Vcurrent*1000, x, y, 5, 100, UTF_RIGHT);
    y += yoffset;

    x = UTF_DrawString(xstart, y, "Vout(mV)=");
    x = UTF_printNumF(calc_result.Vout*1000, x, y, 5, 100, UTF_RIGHT);
    y += yoffset;

    if(calc_result.infinity_resistance)
    {
        x = UTF_DrawString(xstart, y, "Infinity            ");

    } else
    {
        x = UTF_DrawString(xstart, y, "R(KOm)=");
        x = UTF_printNumF(calc_result.resistance*1e-3f, x, y, 5, 100, UTF_RIGHT);
    }
    y += yoffset;

    x = UTF_DrawString(xstart, y, "I(uA)=");
    x = UTF_printNumF(calc_result.current*1e6, x, y, 3, 100, UTF_RIGHT);
    y += yoffset;

    RESISTOR r = GetResistor();
    if(r==RESISTOR_1_Om)
    {
        x = UTF_DrawString(xstart, y, "SHUNT=1 Om  ");
    } else
    if(r==RESISTOR_10_Om)
    {
        x = UTF_DrawString(xstart, y, "SHUNT=10 Om  ");
    } else
    if(r==RESISTOR_100_Om)
    {
        x = UTF_DrawString(xstart, y, "SHUNT=100 Om  ");
    } else
    {
        x = UTF_DrawString(xstart, y, "SHUNT=1 KOm  ");
    }
    y += yoffset;


    //Capture info
    char cap[100];
    strcpy(cap, "  ");
    if(IsEnabledCapturingTrigger())
    {
        strcat(cap, "Capture started");
    } else
    {
        if(IsAdcBufferFull())
            strcat(cap, "Capture completed");
        else
            strcat(cap, "No capture");
    }

    x = UTF_DrawStringJustify(0, y, cap, UTFT_getDisplayXSize(), UTF_LEFT);
    y += yoffset;
}

void catFloat(char* outstr, int outstr_size, float value, int digits)
{
    //digits - количество цифр, как до, так и после запятой.
    //Предполагается, что fabsf(value)<9999
    int places = 0;
    if(value >= 1000)
        places = MAX(digits-4, 0);
    else
    if(value >= 100)
        places = MAX(digits-3, 0);
    else
    if(value >= 10)
        places = MAX(digits-2, 0);
    else
    if(value >= 1)
        places = MAX(digits-1, 0);
    else
        places = digits;

    int len = strlen(outstr);
    floatToString(outstr+len, outstr_size-len, value, places, 0, false);
    if(places==0)
        strcat(outstr, ".");
}

//places - общее количество символов
void PrintFixedSizeFloat(const RectA* in, float value, int digits, UTF_JUSTIFY justify)
{
/*
    char st[27];
    floatToString(st, 27, value, places, 0, false);
    st[places] = 0;
*/
    char st[27];
    st[0] = 0;
    catFloat(st, sizeof(st), value, digits);

    R_DrawStringJustify(in, st, justify);
}

void DrawResult()
{
    char buf[50];

    UTFT_setColor(VGA_WHITE);
    UTF_SetFont(g_default_font);

    MidData d = GetMidData();
    CalcResult calc_result;
    calculate(d.adc_V, d.adc_I,
                   GetResistorValue(d.r), &calc_result);

    int places = 4;

    if(line2_type == LINE2_RESISTANCE)
    {
        if(calc_result.infinity_resistance)
        {
            UTF_SetFont(font_big_nums);
            R_DrawStringJustify(&r_num_resistance, "-----", UTF_RIGHT);
            UTF_SetFont(g_default_font);
            R_DrawStringJustify(&r_param_resistance, "--", UTF_LEFT);
        } else
        {
            float mul = 1;
            if(calc_result.resistance > 9e6f)
            {
                mul = 1e-6f;
                R_DrawStringJustify(&r_param_resistance, "MOm", UTF_LEFT);
            } else
            if(calc_result.resistance > 9e3f)
            {
                mul = 1e-3f;
                R_DrawStringJustify(&r_param_resistance, "KOm", UTF_LEFT);
            } else
            {
                mul = 1;
                R_DrawStringJustify(&r_param_resistance, "Om", UTF_LEFT);
            }

            UTF_SetFont(font_big_nums);
            PrintFixedSizeFloat(&r_num_resistance, calc_result.resistance*mul, places, UTF_RIGHT);
            UTF_SetFont(g_default_font);
        }
    }

    {
        float mul = 1;
        char* suffix = "";
        if(calc_result.current > 1e-3f)
        {
            mul = 1e3f;
            suffix = "mA";
        } else
        if(calc_result.current > 1e-6f)
        {
            mul = 1e6f;
            suffix = "uA";
        } else
        {
            mul = 1e9f;
            suffix = "nA";
        }

        R_DrawStringJustify(&r_param_current, suffix, UTF_LEFT);
        UTF_SetFont(font_big_nums);
        PrintFixedSizeFloat(&r_num_current, calc_result.current*mul, places, UTF_RIGHT);
        UTF_SetFont(g_default_font);

        if(line2_type == LINE2_CURRENT_MIN_MAX)
        {
            calculate(d.adc_V, d.adc_I_min,
                           GetResistorValue(d.r), &calc_result);
            strcpy(buf, "min = ");
            catFloat(buf, sizeof(buf), calc_result.current*mul, places);
            strcat(buf, " ");
            strcat(buf, suffix);
            R_DrawStringJustify(&r_current_min, buf, UTF_CENTER);

            calculate(d.adc_V, d.adc_I_max,
                           GetResistorValue(d.r), &calc_result);
            strcpy(buf, "min = ");
            catFloat(buf, sizeof(buf), calc_result.current*mul, places);
            strcat(buf, " ");
            strcat(buf, suffix);
            R_DrawStringJustify(&r_current_max, buf, UTF_CENTER);

            //R_FillRectBack(&r_current_min);
            //R_FillRectBack(&r_current_max);
        }

    }

    strcpy(buf, "Vout = ");
    floatToString(buf+strlen(buf), 27, calc_result.Vout, 4, 0, false);
    strcat(buf, " Volts");
    R_DrawStringJustify(&r_vout, buf, UTF_CENTER);

    RESISTOR r = GetResistor();
    const char* shunt_str = "";
    if(r==RESISTOR_1_Om)
    {
        shunt_str = "SHUNT=1 Om  ";
    } else
    if(r==RESISTOR_10_Om)
    {
        shunt_str = "SHUNT=10 Om  ";
    } else
    if(r==RESISTOR_100_Om)
    {
        shunt_str = "SHUNT=100 Om  ";
    } else
    {
        shunt_str = "SHUNT=1 KOm  ";
    }

    R_DrawStringJustify(&r_shunt, shunt_str, UTF_CENTER);

    if(enable_percent_view)
    {
        float value = fmin(GetPercentInInterrupt(), 99.9f);
        floatToString(buf, sizeof(buf), value, 1, 0, false);
        strcat(buf, "%");
        R_DrawStringJustify(&r_percent, buf, UTF_RIGHT);
    }
}

void SceneSingleQuant()
{
    uint32_t cur_time = TimeMs();
    if( (uint32_t)(cur_time-prev_draw_time) > 500)
    {
        //DrawResultOld();
        DrawResult();
        prev_draw_time = cur_time;
    }

    if(EncButtonPressed())
    {
        EnableCapturingTrigger();
    }
}
