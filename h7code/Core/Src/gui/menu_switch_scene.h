#pragma once
void MenuSwitchSceneStart();

typedef enum SwitchToEnum
{
    SWITCH_TO_MICROAMPERMETER = 0,
    SWITCH_TO_HISTOGRAM = 1,
    SWITCH_TO_OSCILLOSCOPE = 2,
} SwitchToEnum;

void SwitchToScene(SwitchToEnum data);
