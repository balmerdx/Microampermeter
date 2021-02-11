#pragma once

//Интерфейс для QSPI
//CS выставляется автоматически

bool QspiSend1Line(uint8_t instruction, const void* data, uint32_t data_size);
bool QspiReceive1Line(uint8_t instruction, void* data, uint32_t data_size);

bool QspiSend1LineAddr24(uint8_t instruction, uint32_t addr, const void* data, uint32_t data_size);
bool QspiReceive1LineAddr24(uint8_t instruction, uint32_t addr, void* data, uint32_t data_size);
