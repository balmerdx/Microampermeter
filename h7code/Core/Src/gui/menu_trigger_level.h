#pragma once

typedef enum TriggerLevelEnum
{
    TRIGGER_LEVEL_1uA,
    TRIGGER_LEVEL_2uA,
    TRIGGER_LEVEL_5uA,
    TRIGGER_LEVEL_10uA,
    TRIGGER_LEVEL_20uA,
    TRIGGER_LEVEL_50uA,
    TRIGGER_LEVEL_100uA,
    TRIGGER_LEVEL_200uA,
    TRIGGER_LEVEL_500uA,
    TRIGGER_LEVEL_1mA,
    TRIGGER_LEVEL_2mA,
    TRIGGER_LEVEL_5mA,
    TRIGGER_LEVEL_10mA,
    TRIGGER_LEVEL_COUNT,
} TriggerLevelEnum;

const char* TrigerLevelStr(TriggerLevelEnum v);

void MenuTriggerLevelStart();
