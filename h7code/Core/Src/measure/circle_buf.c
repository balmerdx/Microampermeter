#include <stdint.h>
#include <stdbool.h>
#include "circle_buf.h"
#include <memory.h>

void CircleBufferInit(CircleBuffer* buffer, uint8_t* data, uint32_t data_size, uint32_t sample_size)
{
    buffer->data = data;
    buffer->sample_size = sample_size;
    buffer->data_size_in_samples = data_size / sample_size;
    buffer->data_size = buffer->data_size_in_samples * sample_size;

    CircleBufferClear(buffer);
}

void CircleBufferClear(CircleBuffer* buffer)
{
    buffer->samples_stored = 0;
    buffer->first_sample_offset = 0;
    buffer->end_sample_offset = 0;
}


uint32_t CircleBufferSamples(CircleBuffer* buffer)
{
    return buffer->samples_stored;
}

uint32_t CircleBufferCapacity(CircleBuffer* buffer)
{
    return buffer->data_size_in_samples;
}

void CircleBufferAdd(CircleBuffer* buffer, const void * const sample)
{
    memcpy(buffer->data + buffer->end_sample_offset, sample, buffer->sample_size);
    buffer->end_sample_offset += buffer->sample_size;
    if(buffer->end_sample_offset >= buffer->data_size)
        buffer->end_sample_offset = 0;

    if(buffer->samples_stored < buffer->data_size_in_samples)
    {
        buffer->samples_stored++;
    } else
    {
        buffer->first_sample_offset += buffer->sample_size;
        if(buffer->first_sample_offset >= buffer->data_size)
            buffer->first_sample_offset = 0;
    }
}

bool CircleBufferGetSample(CircleBuffer* buffer, uint32_t index, void* sample)
{
    if(index >= buffer->data_size_in_samples)
        return false;

    uint32_t offset = buffer->first_sample_offset + index * buffer->sample_size;
    if(offset >= buffer->data_size)
        offset -= buffer->data_size;
    memcpy(sample, buffer->data + offset, buffer->sample_size);
    return true;
}

uint32_t CircleBufferGetAbsoluteOffset(CircleBuffer* buffer, uint32_t index)
{
    return (buffer->first_sample_offset/buffer->sample_size + index) % buffer->data_size_in_samples;
}

uint32_t CircleBufferGetRelativeOffset(CircleBuffer* buffer, uint32_t abs_index)
{
    if(abs_index >= buffer->data_size_in_samples)
        return 0;

    uint32_t first_offset_in_samples = buffer->first_sample_offset/buffer->sample_size;
    if(abs_index < first_offset_in_samples)
        return abs_index + buffer->data_size_in_samples - first_offset_in_samples;
    return abs_index - first_offset_in_samples;
}

uint32_t CircleBufferGetAbsoluteEndOffset(CircleBuffer* buffer)
{
    return CircleBufferGetAbsoluteOffset(buffer, buffer->samples_stored);
}
