#include "main.h"
#include <string.h>
#include <sys/param.h>

#include "scene_single.h"
#include "menu_root.h"
#include "menu_bandwidth.h"
#include "gui_settings.h"

#include "UTFT.h"
#include "float_to_string.h"
#include "fonts/font_condensed30.h"
#include "font_big_nums.h"
#include "interface/interface.h"
#include "interface/statusbar.h"
#include "images/images.h"

#include "measure/receive_data.h"
#include "measure/calculate.h"
#include "measure/mid_big_interval.h"
#include "hardware/vbat.h"

#define COLOR_BACK1 VGA_TEAL
#define COLOR_BACK2 VGA_NAVY

const bool enable_percent_view = true;

static RectA r_battery;
static bool battery_is_low;

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

    RectA r_tmp;
    DrawHeaderAndGetVbatRect("Microampermeter", &r_tmp);

    {
        //Нижняя строка.
        //Разместим пока на ней
        //SHUNT=1 KOm  - сопротивление текущего используемого шунта
        //Vout=1.222 V - напряжение выходное
        //11% - процент времени проведённый в прерывании
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

void catFloatFixed(char* outstr, int outstr_size, float value, int digits)
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
    char st[27];
    st[0] = 0;
    catFloatFixed(st, sizeof(st), value, digits);
    R_DrawStringJustify(in, st, justify);
}

char* CurrentSuffix(float current, float* mul)
{
    *mul = 1;
    char* suffix = "";
    current = fabsf(current);
    if(current > 1e-3f)
    {
        *mul = 1e3f;
        suffix = "mA";
    } else
    if(current > 1e-6f)
    {
        *mul = 1e6f;
        suffix = "uA";
    } else
    {
        *mul = 1e9f;
        suffix = "nA";
    }

    return suffix;
}

void DrawResult()
{
    char buf[50];

    UTFT_setColor(VGA_WHITE);
    UTF_SetFont(g_default_font);

    MidData d = GetMidData();
    CalcResult calc_result;
    calculateRV(d.voltage, d.current,
                   GetResistorValue(GetResistor()), &calc_result);

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
        float mul;
        char* suffix = CurrentSuffix(calc_result.current, &mul);
        R_DrawStringJustify(&r_param_current, suffix, UTF_LEFT);
        UTF_SetFont(font_big_nums);
        PrintFixedSizeFloat(&r_num_current, calc_result.current*mul, places, UTF_RIGHT);
        UTF_SetFont(g_default_font);

        if(line2_type == LINE2_CURRENT_MIN_MAX)
        {
            strcpy(buf, "min = ");
            catFloatFixed(buf, sizeof(buf), d.current_min*mul, places);
            strcat(buf, " ");
            strcat(buf, suffix);
            R_DrawStringJustify(&r_current_min, buf, UTF_CENTER);

            strcpy(buf, "max = ");
            catFloatFixed(buf, sizeof(buf), d.current_max*mul, places);
            strcat(buf, " ");
            strcat(buf, suffix);
            R_DrawStringJustify(&r_current_max, buf, UTF_CENTER);
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

    UpdateVbatLow();
}

void SceneSingleQuant()
{
    uint32_t cur_time = TimeMs();
    if( (uint32_t)(cur_time-prev_draw_time) > 500)
    {
        DrawResult();
        prev_draw_time = cur_time;
    }

    if(EncButtonPressed())
    {
        //EnableCapturingTrigger();
        MenuRootStart();
        return;
    }
}

static void UpdateVbatLowInternal(bool redraw)
{
    bool is_low = VBatIsLow();
    if(!redraw && battery_is_low == is_low)
        return;
    battery_is_low = is_low;
    UTFT_drawBitmap4(r_battery.x, r_battery.y, battery_is_low?&batery_empty_img:&batery_full_img);
}

void UpdateVbatLow()
{
    UpdateVbatLowInternal(false);
}

void DrawHeaderAndGetVbatRect(const char *header_name, RectA *r_screen)
{
    UTF_SetFont(g_default_font);
    RectA r_tmp = R_DisplaySize();
    RectA r_top;
    R_SplitY1(&r_tmp, UTF_Height(), &r_top, r_screen);

    RectA r_header;
    RectA r_bandwidth;
    int width = 0;
    for(int i=FilterX_1; i<=FilterX_1024 ;i++)
    {
        width = MAX(width, UTF_StringWidth(FilterXBandwidth(i))+X_MARGIN*2);
    }

    r_top.back_color = STATUSBAR_BACKGROUND;
    R_SplitX2(&r_top, batery_full_img.width, &r_top, &r_battery);
    R_SplitX2(&r_top, width, &r_header, &r_bandwidth);

    UTFT_setColor(VGA_WHITE);
    R_DrawStringJustify(&r_header, header_name, UTF_CENTER);

    //r_bandwidth.back_color = COLOR_BACK1;
    R_DrawStringJustify(&r_bandwidth, FilterXBandwidth(g_filterX), UTF_CENTER);

    UpdateVbatLowInternal(true);
}
