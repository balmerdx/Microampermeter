#include "main.h"
#include "menu_calibration_hard.h"
#include "menu_calibration_zero.h"
#include "menu_calibration.h"

#include <string.h>
#include "float_to_string.h"
#include "interface/interface.h"
#include "interface/menu.h"
#include "interface/progress_bar.h"
#include "interface/scene_get_float.h"
#include "measure/mid_big_interval.h"
#include "measure/calculate.h"
#include "measure/settings.h"

/*
 * Как проходит калибровка коэффициентов усиления и номиналов резисторов.
 * Перед этой калибровкой требуется offset откалибровать.
 * Сначала отсоединяем все от прибора и выставляем напряжение питания в интервале 2.5-3 вольта.
 * Измеряем его прибором и вводим энкодером.
 * Коэффициент усиления напряжения откалиброван.
 * Подключаем 100 КОм точный резистор. Предполагаем, что 1 КОм у нас достаточно точен.
 * Получаем коэффициент усиления по току.
 * Потом подключаем точный резистор 10 КОм и калибруем сопротивление 100 Ом.
 * Потом подключаем точный резистор 1 КОм и калибруем сопротивление 10 Ом.
 * Потом подключаем точный резистор 100 Ом и калибруем сопротивление 1 Ом.
 * Всё, калибровка завершена.
 */
typedef enum
{
    CHS_BEGIN,
    CHS_BEFORE_GET_VOLTAGE,
    CHS_GET_VOLTAGE,
    CHS_BEFORE_GET_CURRENT_1K,
    CHS_GET_CURRENT_1K,
    CHS_COMPLETE,
} CalibrationHartdState;

static CalibrationHartdState chs_state;
static RectA r_info_str;
static RectA r_adc_I;
static RectA r_adc_V;
static float measured_voltage = 1.f;

static void MenuCalibrationHardQuant();

static void CalibrationHardStart()
{
    EnableAutoResistorSwitch(false);
    SetEnableV(true);
    SetReceiveDataFunc(0, ReceiveDataFunc_Calibration);
    SetResistor(RESISTOR_1_Kom);
}

static void CalibrationHardComplete()
{
    EnableAutoResistorSwitch(true);
    SetReceiveDataFunc(0, ReceiveDataFunc_Mid);
    DelayMs(2);
    SetEnableV(true);//!!!! Тут надо потом настройку подтянуть !!!!
}

static void DrawAdcIV()
{
    char str[60];
    UTFT_setColor(VGA_WHITE);
    strcpy(str, "adc_I=");
    catInt(str, CalibrationAdcI());
    R_DrawStringJustify(&r_adc_I, str, UTF_CENTER);

    strcpy(str, "adc_V=");
    catInt(str, CalibrationAdcV());
    R_DrawStringJustify(&r_adc_V, str, UTF_CENTER);
}

void MenuCalibrationHardRestart()
{
    RectA r_cur = R_DisplaySizeWithoutHeaderAndStatusbar();
    RectA r_tmp;
    //Выделяем строку в центре.
    int height_top = (r_cur.height - UTF_Height()*3)/2;

    UTF_SetFont(g_default_font);
    R_SplitY1(&r_cur, height_top, &r_tmp, &r_cur);
    R_FillRectBack(&r_tmp);
    R_SplitY1(&r_cur, UTF_Height(), &r_info_str, &r_cur);
    R_SplitY1(&r_cur, UTF_Height(), &r_calibration_progress_bar, &r_cur);
    R_SplitY1(&r_cur, UTF_Height(), &r_tmp, &r_cur);
    R_SplitX1(&r_tmp, r_tmp.width/2, &r_adc_I, &r_adc_V);

    HeaderSetTextAndRedraw("Calibration hard");
    StatusbarSetTextAndRedraw("Calibration hard");

    R_FillRectBack(&r_calibration_progress_bar);
    R_DrawStringJustify(&r_info_str, "Press to continue. Rotate to cancel.", UTF_CENTER);
    R_FillRectBack(&r_tmp);
    R_FillRectBack(&r_cur);
    R_FillRectBack(&r_adc_I);
    R_FillRectBack(&r_adc_V);

    InterfaceGoto(MenuCalibrationHardQuant);
}

void MenuCalibrationHardStart()
{
    CalibrationHardStart();
    chs_state = CHS_BEGIN;

    MenuCalibrationHardRestart();
}

void MenuCalibrationHardQuant()
{
    bool pressed = EncButtonPressed();
    bool is_capture_complete = CalibrationIsSumCompleteQuant();

    if(chs_state == CHS_BEGIN && pressed)
    {
        SceneGetFloatStart("Enter Vout voltage (2.5-3.0 V)", 0, 5, 4, 0, MenuCalibrationHardRestart);
        chs_state = CHS_BEFORE_GET_VOLTAGE;
        return;
    }

    if(chs_state == CHS_BEFORE_GET_VOLTAGE)
    {
        measured_voltage = SceneGetFloat();
        CalibrationStartSum(CALIBRATION_CAPTURE_COUNT);
        chs_state = CHS_GET_VOLTAGE;
        return;
    }

    if(chs_state == CHS_GET_VOLTAGE && is_capture_complete)
    {
        //Считаем калибровку для коэффициента усиления напряжения
        voltage_mul = measured_voltage/(CalibrationAdcV()-g_settings.offset_adc_V);

        DrawAdcIV();
        char str[STATUSBAR_STR_LEN];
        strcpy(str, "corr voltage=");
        catFloat(str, voltage_mul/voltage_mul_original, 3);
        StatusbarSetTextAndRedraw(str);

        R_DrawStringJustify(&r_info_str, "Connect resiztor 100Kom. Press key.", UTF_CENTER);
        SetResistor(RESISTOR_1_Kom);
        chs_state = CHS_BEFORE_GET_CURRENT_1K;
        return;
    }

    if(chs_state == CHS_BEFORE_GET_CURRENT_1K && pressed)
    {
        CalibrationStartSum(CALIBRATION_CAPTURE_COUNT);
        chs_state = CHS_GET_CURRENT_1K;
        return;
    }

    if(chs_state == CHS_GET_CURRENT_1K && is_capture_complete)
    {
        //Считаем калибровку для коэффициента усиления тока
        float Vout = calculateVoltage(CalibrationAdcV());
        int32_t adc_I = CalibrationAdcI();

        //Считаем, что резистор 1 КОм достаточно точен
        float Rlow = GetResistorValue(GetResistor());
        float Rup = 100e3f;//100 KOm
        float I = Vout/(Rlow+Rup);

        current_mul = I / (adc_I-g_settings.offset_adc_I)*Rlow;

        DrawAdcIV();
        char str[STATUSBAR_STR_LEN];
        strcpy(str, "corr current=");
        catFloat(str, current_mul/current_mul_original, 3);
        StatusbarSetTextAndRedraw(str);

        R_DrawStringJustify(&r_info_str, "Press key to exit", UTF_CENTER);
        chs_state = CHS_COMPLETE;
        return;
    }

    if(chs_state == CHS_COMPLETE && pressed)
    {
        CalibrationHardComplete();
        MenuCalibrationStart();
        return;
    }

}
