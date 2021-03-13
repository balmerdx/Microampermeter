#include "main.h"
#include "menu_calibration_zero.h"
#include "menu_calibration.h"

#include <string.h>
#include "float_to_string.h"
#include "interface/interface.h"
#include "interface/menu.h"
#include "interface/progress_bar.h"

#include "hardware/gpios.h"
#include "hardware/ADS1271_input.h"
#include "measure/receive_data.h"
#include "measure/mid_big_interval.h"
#include "measure/settings.h"

/*
 * Как происходит калибровка нуля.
 * Отключаем напряжения питания.
 * Просим отключить все устройства.
 * Выводим значения ADC0, ADC1 на экран.
 * Сначала определяем нулевой ток (суммируя за несколько секунд)
 * Потом просим соединить щупы и определяем нулевое напряжение.
*/
typedef enum
{
    CZS_BEGIN_CALIBRATE_I,
    CZS_BEGIN_CALIBRATE_V,
    CZS_BEFORE_OPEN_FIXTURE,
    CZS_OPEN_FIXTURE,
    CZS_BEFORE_SHORT_FIXTURE,
    CZS_SHORT_FIXTURE,
    CZS_COMPLETE,
} CalibrationZeroState;

static CalibrationZeroState czs_state;
static RectA r_info_str;
RectA r_calibration_progress_bar;
static RectA r_adc_I;
static RectA r_adc_V;


static volatile bool zero_offset_start = false;
static volatile bool zero_offset_in_progress = false;
static volatile bool zero_offset_complete = true;
static volatile uint32_t zero_offset_count = 0;

static volatile uint32_t zero_offset_count_to_capture;
static volatile int64_t zero_offset_sum_adc_I;
static volatile int64_t zero_offset_sum_adc_V;

static void MenuCalibrationZeroQuant();

void ReceiveDataFunc_Calibration(DataNoError* data)
{
    if(zero_offset_start)
    {
        zero_offset_start = false;
        zero_offset_in_progress = true;
        zero_offset_count_to_capture = zero_offset_count;
        zero_offset_sum_adc_I = 0;
        zero_offset_sum_adc_V = 0;
    }

    if(!zero_offset_in_progress)
        return;

    for(int i=0; i<ADS1271_RECEIVE_DATA_SIZE; i++)
    {
        if(zero_offset_count_to_capture==0)
            break;
        zero_offset_sum_adc_I += data[i].adc_I;
        zero_offset_sum_adc_V += data[i].adc_V;
        zero_offset_count_to_capture--;
    }

    if(zero_offset_count_to_capture==0)
    {
        zero_offset_in_progress = false;
        zero_offset_complete = true;
    }
}

void CalibrationStartSum(uint32_t count)
{
    if(count==0)
        return;

    zero_offset_count = count;
    zero_offset_start = true;

    ProgressInit(r_calibration_progress_bar.x, r_calibration_progress_bar.y, r_calibration_progress_bar.width, r_calibration_progress_bar.height);
    ProgressSetPos(0);
    ProgressSetVisible(true);
}

bool CalibrationIsSumCompleteQuant()
{
    if(zero_offset_in_progress || zero_offset_complete)
    {
        ProgressSetPos((zero_offset_count-zero_offset_count_to_capture)/(float)zero_offset_count);
    }

    if(zero_offset_complete)
    {
        zero_offset_complete = false;
        return true;
    }

    return false;
}

int32_t CalibrationAdcI()
{
    return zero_offset_sum_adc_I/zero_offset_count;
}

int32_t CalibrationAdcV()
{
    return zero_offset_sum_adc_V/zero_offset_count;
}

/*
static bool wait_in_progress = false;
static uint32_t wait_start_time_ms;
static uint32_t wait_delay_time_ms;
static void StartWait(uint32_t delay_time_ms)
{
    wait_start_time_ms = TimeMs();
    wait_delay_time_ms = delay_time_ms;
    wait_in_progress = true;

    ProgressInit(r_calibration_progress_bar.x, r_calibration_progress_bar.y, r_calibration_progress_bar.width, r_calibration_progress_bar.height);
    ProgressSetPos(0);
    ProgressSetVisible(true);
}

static bool IsWaitInProgressQuant()
{
    if(!wait_in_progress)
        return false;

    uint32_t dt = TimeMs() - wait_start_time_ms;
    if(dt >= wait_delay_time_ms)
        wait_in_progress = false;
    ProgressSetPos(dt/(float)wait_delay_time_ms);
    return wait_in_progress;
}
*/
static void CalibrationZeroStart()
{
    EnableAutoResistorSwitch(false);
    SetEnableV(false);
    SetReceiveDataFunc(0, ReceiveDataFunc_Calibration);
    SetResistor(RESISTOR_1_Om);
}

static void CalibrationZeroComplete()
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

static void SaveAndDraw()
{
    if(SaveSettings())
    {
        UTFT_setColor(VGA_WHITE);
        R_DrawStringJustify(&r_info_str, "Saved. Press key to exit.", UTF_CENTER);
    } else
    {
        UTFT_setColor(VGA_RED);
        R_DrawStringJustify(&r_info_str, "Save fail. Press key to exit.", UTF_CENTER);
    }
}

void MenuCalibrationZeroStart(bool calibrateI)
{
    HeaderSetTextAndRedraw("Calibration zero offset");
    StatusbarSetTextAndRedraw("Calibration zero offset");

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

    R_FillRectBack(&r_calibration_progress_bar);
    R_DrawStringJustify(&r_info_str, "Waiting...", UTF_CENTER);
    R_FillRectBack(&r_tmp);
    R_FillRectBack(&r_cur);

    //r_adc_I.back_color = VGA_TEAL;
    //r_adc_V.back_color = VGA_NAVY;
    R_FillRectBack(&r_adc_I);
    R_FillRectBack(&r_adc_V);

    //Запретить переключать пределы измерения автоматически.
    //Переключить на резистор 1 Ом
    //Вызывать функцию суммирования специальную.
    CalibrationZeroStart();

    czs_state = calibrateI ? CZS_BEGIN_CALIBRATE_I : CZS_BEGIN_CALIBRATE_V;

    InterfaceGoto(MenuCalibrationZeroQuant);
}

static void MenuCalibrationZeroQuant()
{
    bool pressed = EncButtonPressed();
    bool is_capture_complete = CalibrationIsSumCompleteQuant();

    if(czs_state == CZS_BEGIN_CALIBRATE_I)
    {
        UTFT_setColor(VGA_WHITE);
        R_DrawStringJustify(&r_info_str, "Open fixture and press key", UTF_CENTER);
        czs_state = CZS_BEFORE_OPEN_FIXTURE;
        return;
    }

    if(czs_state == CZS_BEGIN_CALIBRATE_V)
    {
        UTFT_setColor(VGA_WHITE);
        R_DrawStringJustify(&r_info_str, "Short Vout & Isense fixture and press key.", UTF_CENTER);
        czs_state = CZS_BEFORE_OPEN_FIXTURE;
        return;
    }

    if(czs_state==CZS_BEFORE_OPEN_FIXTURE && pressed)
    {
        UTFT_setColor(VGA_WHITE);
        R_DrawStringJustify(&r_info_str, "Waiting...", UTF_CENTER);

        czs_state = CZS_OPEN_FIXTURE;
        CalibrationStartSum(CALIBRATION_CAPTURE_COUNT);
        return;
    }

    if(czs_state==CZS_OPEN_FIXTURE && is_capture_complete)
    {
        g_settings.offset_adc_I = CalibrationAdcI();
        DrawAdcIV();
        SaveAndDraw();
        czs_state = CZS_COMPLETE;
        return;
    }

    if(czs_state==CZS_BEFORE_SHORT_FIXTURE && pressed)
    {
        UTFT_setColor(VGA_WHITE);
        R_DrawStringJustify(&r_info_str, "Waiting...", UTF_CENTER);

        czs_state = CZS_SHORT_FIXTURE;
        CalibrationStartSum(CALIBRATION_CAPTURE_COUNT);
        return;
    }

    if(czs_state==CZS_SHORT_FIXTURE && is_capture_complete)
    {
        g_settings.offset_adc_V = CalibrationAdcV();
        DrawAdcIV();
        SaveAndDraw();
        czs_state = CZS_COMPLETE;
        return;
    }

    if(czs_state==CZS_COMPLETE && pressed)
    {
        CalibrationZeroComplete();
        MenuCalibrationStart();
        return;
    }
}
