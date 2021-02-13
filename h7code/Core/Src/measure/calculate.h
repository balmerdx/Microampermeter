#pragma once

typedef struct
{
    float Vout;
    float current;
    float resistance;
    bool infinity_resistance;
} CalcResult;


void CorrectionInit();

float calculateCurrent(int32_t measureI, float RshuntInv);

float calculateVoltage(int32_t measureV);

void calculateRV(int32_t measureV, float current,
               float Rshunt, CalcResult* calc_result);

//Временно, потом в настройки перенесём.
extern float voltage_mul_original;
extern float voltage_mul;

extern float current_mul_original;
extern float current_mul;
