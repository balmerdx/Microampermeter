#include "main.h"
#include <string.h>

#include "scene_single.h"

#include "UTFT.h"
#include "fonts/font_condensed30.h"
#include "fonts/font_condensed59.h"
#include "interface/interface.h"

#include "measure/receive_data.h"
#include "measure/calculate.h"
#include "measure/mid_big_interval.h"

static void SceneSingleQuant();

static uint32_t prev_draw_time;

void SceneSingleStart()
{
    prev_draw_time = TimeMs();
    InterfaceGoto(SceneSingleQuant);
}

void DrawResult()
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
        EnableCapturingTrigger();
    }
}
