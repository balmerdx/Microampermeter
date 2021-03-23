#include "main.h"
#include "mul_coeff_per_line.h"

#include <sys/param.h>

MulCoeff coeff_seconds_per_line[] =
{
    {"1 ms", 1e-3f},
    {"2 ms", 2e-3f},
    {"5 ms", 5e-3f},
    {"10 ms", 1e-2f},
    {"20 ms", 2e-2f},
    {"50 ms", 5e-2f},
    {"100 ms", 1e-1f},
    {"200 ms", 2e-1f},
    {"500 ms", 5e-1f},
};

MulCoeff coeff_ampers_per_line[] =
{
    {"1 uA", 1e-6f},
    {"2 uA", 2e-6f},
    {"5 uA", 5e-6f},
    {"10 uA", 1e-5f},
    {"20 uA", 2e-5f},
    {"50 uA", 5e-5f},
    {"100 uA", 1e-4f},
    {"200 uA", 2e-4f},
    {"500 uA", 5e-4f},
    {"1 mA", 1e-3f},
    {"2 mA", 2e-3f},
    {"5 mA", 5e-3f},
    {"10 mA", 1e-2f},
    {"20 mA", 2e-2f},
};

int AmperPerLineCount()
{
    return sizeof(coeff_ampers_per_line)/sizeof(coeff_ampers_per_line[0]);
}

static void AmperPerLineClamp(int* idx)
{
    int size = AmperPerLineCount();
    *idx = MIN(*idx, size-1);
    *idx = MAX(*idx, 0);
}

float AmperPerLineCurrent(int idx)
{
    AmperPerLineClamp(&idx);
    return coeff_ampers_per_line[idx].mul;
}

char* AmperPerLineInfo(int idx)
{
    AmperPerLineClamp(&idx);
    return coeff_ampers_per_line[idx].info;
}

int SecondsPerLineCount()
{
    return sizeof(coeff_seconds_per_line)/sizeof(coeff_seconds_per_line[0]);
}

static void SecondsPerLineClamp(int* idx)
{
    int size = SecondsPerLineCount();
    *idx = MIN(*idx, size-1);
    *idx = MAX(*idx, 0);
}

float SecondsPerLineCurrent(int idx)
{
    SecondsPerLineClamp(&idx);
    return coeff_seconds_per_line[idx].mul;
}

char* SecondsPerLineInfo(int idx)
{
    SecondsPerLineClamp(&idx);
    return coeff_seconds_per_line[idx].info;
}
