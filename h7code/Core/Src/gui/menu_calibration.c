#include "main.h"
#include "menu_root.h"
#include "menu_calibration.h"
#include "menu_calibration_zero.h"
#include "menu_calibration_hard.h"

#include <string.h>
#include "float_to_string.h"
#include "interface/interface.h"
#include "interface/menu.h"
#include "measure/settings.h"

enum
{
    MC_RETURN,
    MC_OFFSET_ZERO_I,
    MC_OFFSET_ZERO_V,
    MC_CALIBRATION_HARD,
    MC_VIEW_CALIBRATION,
};

static void MenuCalibrationQuant();
static void MenuViewCalibrationStart();
static void MenuViewCalibrationQuant();

void MenuCalibrationStart()
{
    MenuReset("Calibration");
    MenuAdd("..", MC_RETURN);
    MenuAdd("Offset zero current", MC_OFFSET_ZERO_I);
    MenuAdd("Offset zero voltage", MC_OFFSET_ZERO_V);
    MenuAdd("System calibration", MC_CALIBRATION_HARD);
    MenuAdd("View last calibration", MC_VIEW_CALIBRATION);
    MenuRedraw();

    InterfaceGoto(MenuCalibrationQuant);
}

void MenuCalibrationQuant()
{
    MenuQuant();
    if(!EncButtonPressed())
        return;

    if(MenuData()==MC_RETURN)
    {
        MenuRootStart();
        return;
    }

    if(MenuData()==MC_OFFSET_ZERO_I)
    {
        MenuCalibrationZeroStart(true);
        return;
    }

    if(MenuData()==MC_OFFSET_ZERO_V)
    {
        MenuCalibrationZeroStart(false);
        return;
    }

    if(MenuData()==MC_CALIBRATION_HARD)
    {
        MenuCalibrationHardStart();
        return;
    }

    if(MenuData()==MC_VIEW_CALIBRATION)
    {
        MenuViewCalibrationStart();
        return;
    }

}

//View Calibration

//Откусываем по маленьку от rectAll
static void DrawNext(RectA* rectAll, const char* str)
{
    RectA r;
    R_SplitY1(rectAll, UTF_Height(), &r, rectAll);
    R_DrawStringJustify(&r, str, UTF_LEFT);
}

void MenuViewCalibrationStart()
{
    HeaderSetTextAndRedraw("View calibration coeff");
    StatusbarSetTextAndRedraw("Press key to exit");

    int margin = 10;
    RectA r_cur = R_DisplaySizeWithoutHeaderAndStatusbar();
    UTF_SetFont(g_default_font);
    RectA r_left, r_right, r_tmp;
    R_SplitX1(&r_cur, r_cur.width/2, &r_left, &r_right);
    r_left.back_color = UTFT_COLOR(0, 0, 32);
    r_right.back_color = UTFT_COLOR(32, 0, 0);
    R_SplitX1(&r_left, margin, &r_tmp, &r_left);
    R_FillRectBack(&r_tmp);
    R_SplitX2(&r_right, margin, &r_right, &r_tmp);
    R_FillRectBack(&r_tmp);

    char str[STATUSBAR_STR_LEN];
    strcpy(str, "offset I=");
    catInt(str, g_settings.offset_adc_I);
    DrawNext(&r_left, str);

    strcpy(str, "offset V=");
    catInt(str, g_settings.offset_adc_V);
    DrawNext(&r_right, str);

    strcpy(str, "mul I=");
    catFloat(str, g_settings_permanent.mul_I/mul_I_original, 4);
    DrawNext(&r_left, str);

    strcpy(str, "mul V=");
    catFloat(str, g_settings_permanent.mul_V/mul_V_original, 4);
    DrawNext(&r_right, str);

    strcpy(str, "R_100_Om=");
    catFloat(str, g_settings_permanent.R_100_Om, 2);
    DrawNext(&r_left, str);

    strcpy(str, "R_10_Om=");
    catFloat(str, g_settings_permanent.R_10_Om, 3);
    DrawNext(&r_left, str);

    strcpy(str, "R_1_Om=");
    catFloat(str, g_settings_permanent.R_1_Om, 4);
    DrawNext(&r_left, str);

    R_FillRectBack(&r_left);
    R_FillRectBack(&r_right);

    InterfaceGoto(MenuViewCalibrationQuant);
}

void MenuViewCalibrationQuant()
{
    if(!EncButtonPressed())
        return;
    MenuCalibrationStart();
}
