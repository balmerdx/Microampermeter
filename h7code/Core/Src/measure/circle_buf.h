#pragma once

// Циклический буффер.
// После того, как он полностью записан, начинает постепенно стирать старые данные.

typedef struct CircleBuffer
{
    uint8_t* data;
    uint32_t data_size;
    uint32_t sample_size;

    //
    uint32_t data_size_in_samples;
    uint32_t samples_stored;
    uint32_t first_sample_offset; //offset in bytes
    uint32_t end_sample_offset; //offset in bytes, сюда мы пишем следующий сэмпл
} CircleBuffer;

//Создаёт и очищает буфер
void CircleBufferInit(CircleBuffer* buffer, uint8_t* data, uint32_t data_size, uint32_t sample_size);

//Очищает буфер
void CircleBufferClear(CircleBuffer* buffer);

//Количество сэмплов в буфере
uint32_t CircleBufferSamples(CircleBuffer* buffer);

//Максимально возможное количество сэмплов в буфере
uint32_t CircleBufferCapacity(CircleBuffer* buffer);

//Добавляем ещё один сэмпл
void CircleBufferAdd(CircleBuffer* buffer, const void * const sample);
bool CircleBufferGetSample(CircleBuffer* buffer, uint32_t index, void* sample);

//Интерфейс для триггеров. Более медленная функция, с остатком от деления внутри.
//index может выходить немного за пределы буфера текущего, тогда выдаст позицию в
//пределах буфера, по остатку от деления ра размер.
//Возвращает значение в сэмплах от начала буфера.
uint32_t CircleBufferGetAbsoluteOffset(CircleBuffer* buffer, uint32_t index);
uint32_t CircleBufferGetRelativeOffset(CircleBuffer* buffer, uint32_t abs_index);
//То-же, что и для предыдущего случая, но для конца буффера
uint32_t CircleBufferGetAbsoluteEndOffset(CircleBuffer* buffer);
