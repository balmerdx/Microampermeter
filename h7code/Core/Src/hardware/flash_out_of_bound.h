#pragma once

//Для STM32F750 FLASH_SECTOR_TOTAL = 1, но фактически там 1 МБ памяти, который возможно частично битый.
//т.е. всего секторов 8, в нулевом у нас хранится программа
//FLASH_BANK1_BASE - начало нулевого банка
//FLASH_SECTOR_SIZE - размер секторв ==128 KB

//Вариант HAL_FLASH_Program, но без дополнительных проверок, чтобы писать за пределы первого сектора flash
//Это ненадёжно, но нам сойдёт.
//Пишет сразу по FLASH_NB_32BITWORD_IN_FLASHWORD=8 слова (слово=4 байта)
HAL_StatusTypeDef HAL_FLASH_ProgramBank1(uint32_t FlashAddress, uint32_t DataAddress);


//#define ADDR_FLASH_SECTOR_1_BANK1  (FLASH_BANK1_BASE+FLASH_SECTOR_SIZE)
//См параметры FLASH_Erase_Sector
void FLASH_Erase_SectorBank1(uint32_t Sector, uint32_t VoltageRange);

//Очищает память.
//Пишет паттерн в память.
//Заполняет весь сектор.
int TestAndCompareFlashPattern(uint32_t Sector, bool pattetn_invert);
