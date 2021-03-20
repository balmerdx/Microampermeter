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
uint32_t STSamplesCaptured();

void OnFilterNextSampleCircleBuffer(float current, float voltage);

void STSendInfo();
void STSendData();
