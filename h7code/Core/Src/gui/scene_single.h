#pragma once
#include "interface/rect_utils.h"

typedef enum
{
    LINE2_RESISTANCE,
    LINE2_CURRENT_MIN_MAX,
} LINE2_TYPE;

//Заполнять перед вызовом SceneSingleStart
extern LINE2_TYPE line2_type;

//Показывает на экране одно значение усреднённое
//ток или сопротивление. И кучу дебаговой информации по краям.
void SceneSingleStart();

void UpdateVbatLow(RectA* r);
