#pragma once
/* Хранит данные о токе (и возможно напряжении).
 * Хранит g_filterX с которым данные сэмплироваться начали.
 * Хранит момент срабатывания триггера и условия окончания сэмплинга.
 */

void STInit();

void STCaptureStart();
bool STCaptureStarted();
bool STCaptureCompleted();
bool STTriggerTriggered();

void OnFilterNextSampleCircleBuffer(float current, float voltage);

void STSendInfo();
void STSendData();

//Сэмплов за секунду в отсэмплированных данных
float STFilterSPS();

//Общее количество сэмплов
uint32_t STSamplesCaptured();
//Сэмпл на котором сработал триггер.
uint32_t STTriggerOffset();

typedef void (*STIterateIntervalCallback)(float sample, void* param);
//Итерируемся по полученным данным. В пределах [0..STSamplesCaptured)
//Пока только после STCaptureCompleted()==true
bool STIterate(uint32_t istart, uint32_t iend, STIterateIntervalCallback callback, void* param);
