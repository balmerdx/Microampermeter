#pragma once

#define QSPI_MEM_BITS 23 //8 MB = 23 bits address

bool QspiMemInit(QSPI_HandleTypeDef* hqspi);
bool QspiMemRead(QSPI_HandleTypeDef* hqspi, uint32_t address, uint32_t count, uint8_t* buffer);
bool QspiMemWrite(QSPI_HandleTypeDef* hqspi, uint32_t address, uint32_t count, uint8_t* buffer);
