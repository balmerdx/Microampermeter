#include "main.h"
#include "menu_root.h"
#include "menu_calibration.h"
#include "menu_calibration_zero.h"
#include "menu_calibration_hard.h"

#include "interface/interface.h"
#include "interface/menu.h"

enum
{
    MC_RETURN,
    MC_OFFSET_ZERO,
    MC_CALIBRATION_HARD,
};

static void MenuCalibrationQuant();

void MenuCalibrationStart()
{
    MenuReset("Calibration");
    MenuAdd("..", MC_RETURN);
    MenuAdd("Offset zero", MC_OFFSET_ZERO);
    MenuAdd("System calibration", MC_CALIBRATION_HARD);
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

    if(MenuData()==MC_OFFSET_ZERO)
    {
        MenuCalibrationZeroStart();
        return;
    }

    if(MenuData()==MC_CALIBRATION_HARD)
    {
        MenuCalibrationHardStart();
        return;
    }
}

