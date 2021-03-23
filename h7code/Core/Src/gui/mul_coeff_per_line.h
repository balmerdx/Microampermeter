#pragma once

typedef struct
{
    char* info;
    float mul;
} MulCoeff;

int AmperPerLineCount();
float AmperPerLineCurrent(int idx);
char* AmperPerLineInfo(int idx);

int SecondsPerLineCount();
float SecondsPerLineCurrent(int idx);
char* SecondsPerLineInfo(int idx);
