#pragma once

void GpiosInit();

typedef enum RESISTOR
{
    RESISTOR_1_Kom = 0,
    RESISTOR_100_Om,
    RESISTOR_10_Om,
    RESISTOR_1_Om,
} RESISTOR;

void SetResistor(RESISTOR r);

void SetEnableV(bool enable);
