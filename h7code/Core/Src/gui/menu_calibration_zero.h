#pragma once
#include "measure/receive_data.h"
#include "interface/rect_utils.h"

void MenuCalibrationZeroStart();

//Интерфейс для калибровочной функции
enum {
    //Количество сэмплов, которые мы суммируем
    CALIBRATION_CAPTURE_COUNT = 0x40000
};

//Прямоугодльник, в котором будет рисоваться progress_bar суммирования сэмплов
extern RectA r_calibration_progress_bar;
//Функция, которую надо выставлять в SetReceiveDataFunc
void ReceiveDataFunc_Calibration(DataNoError* data);
//Запускаем долгое суммирование
void CalibrationStartSum(uint32_t count);
//Только один раз после завершения возвращает true, потом опять false
bool CalibrationIsSumCompleteQuant();
//После того как CalibrationIsSumCompleteQuant вернула true, можно пользоваться этими функциями
int32_t CalibrationAdcI();
int32_t CalibrationAdcV();
