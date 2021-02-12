#pragma once
/* Основная задача методов этого файла
 * Разбиение экрана на прямоугольники и вывод данных в них.
 */
#include "UTFT.h"

//RectA - прямоугольник с указанием на какой высоте рисовать фонты.
//Если ascent==y, то на не известно на какой высоте рисовать
//Использует текущий фонт для определения ascent
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t ascent;
    int16_t back_color;
} RectA;

//Возвращает размер экрана.
RectA R_DisplaySize();
//Размер экрана без Statusbar & Header (вызывает внутри UTF_SetFont)
RectA R_DisplaySizeWithoutHeaderAndStatusbar();

//Разбивает прямоугольник in на два прямоугольника по горизонтали
//Входной и выходной параметр могут совпадать по адресу.
//Ширина out1 будет width1, а out2 - всё остальное.
void R_SplitX1(const RectA* in, int width1, RectA* out1, RectA* out2);
//Ширина out1 будет in->width-width2, а out2 - width2.
void R_SplitX2(const RectA* in, int width2, RectA* out1, RectA* out2);

//Разбивает прямоугольник in на два прямоугольника по вертикали
//Входной и выходной параметр могут совпадать по адресу.
//ascent выставляется по текущему активному фонту
//height1 - высота получившегося out1
void R_SplitY1(const RectA* in, int height1, RectA* out1, RectA* out2);
//height2 - высота получившегося out2
void R_SplitY2(const RectA* in, int height2, RectA* out1, RectA* out2);

//Рисует прямоугольник используя back_color
void R_FillRectBack(const RectA* in);

//Рисует текст, заполняющий весь прямоугольник.
//Учитывается ascent. Оставшееся сверху и снизу пространство заполняется фоном.
void R_DrawStringJustify(const RectA* in, const char* str, UTF_JUSTIFY justify);

//Работают аналогично UTF_ варианам, но используют RectA для определения положения на экране
void R_printNumI(const RectA* in, long num, UTF_JUSTIFY justify);
void R_printNumF(const RectA* in, float value, int places, UTF_JUSTIFY justify);
