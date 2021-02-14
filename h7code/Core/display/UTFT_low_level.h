#pragma once
//Низкоуровневый интерфейс, чтобы писать прямо на экран.

#ifdef DISPLAY_ILI9481
#include "hardware/hw_ili9481.h"
#else
#include "hardware/hw_ili9341.h"
#endif

//Установить прямоугольную область и начать записть в видеопамять
//Простейший пример как это делается можно посмотреть в UTFT_fillRect
void UTFT_setXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

//Начинаем писать пиксели в память
void TFTBeginData();
//Заканчиваем писать пиксели в память
void TFTEndData();

//Пишем один пиксель в память в прямоугольник установленный UTFT_setXY
//Пиксели пишутся последовательно. Сначала по оси X, потом по Y.
#define TFTWriteData(data) HwLcdSend16NoWait(data)
