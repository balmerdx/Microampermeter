#pragma once

typedef struct
{
    float Vcurrent;
    float Vout;
    float current;
    float resistance;
    bool infinity_resistance;
} CalcResult;

void calculate(int32_t measureV, int32_t measureI,
               float Rshunt, CalcResult* calc_result);
