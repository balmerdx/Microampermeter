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

//Добавляем ещё один сэмпл
void CircleBufferAdd(CircleBuffer* buffer, const void * const sample);
bool CircleBufferGetSample(CircleBuffer* buffer, uint32_t index, void* sample);
