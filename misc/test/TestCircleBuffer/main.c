#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "circle_buf.h"

void Test0(CircleBuffer* buffer)
{
    int32_t sample1, sample2, sample_out;
    sample1 = 1;
    CircleBufferAdd(buffer, &sample1);
    sample2 = 32;
    CircleBufferAdd(buffer, &sample2);

    int32_t samples = CircleBufferSamples(buffer);
    assert(samples==2);
    CircleBufferGetSample(buffer, 0, &sample_out);
    assert(sample1==sample_out);
    CircleBufferGetSample(buffer, 1, &sample_out);
    assert(sample2==sample_out);
}

void Test1(CircleBuffer* buffer)
{
    CircleBufferClear(buffer);

    int32_t offset = 1234;
    for(uint32_t i=0; i<buffer->data_size_in_samples; i++)
    {
        int32_t sample = i+offset;
        CircleBufferAdd(buffer, &sample);
    }

    for(uint32_t i=0; i<buffer->data_size_in_samples; i++)
    {
        int32_t sample = i+offset;
        int32_t sample_out;
        CircleBufferGetSample(buffer, i, &sample_out);
        assert(sample_out==sample);
    }
}

void Test2(CircleBuffer* buffer, uint32_t initial_write_count, uint32_t samples_write_count)
{
    CircleBufferClear(buffer);

    for(uint32_t i=0; i<initial_write_count; i++)
    {
        int32_t sample = 1;
        CircleBufferAdd(buffer, &sample);
    }

    int32_t offset = 1234;
    for(uint32_t i=0; i<samples_write_count; i++)
    {
        int32_t sample = i+offset;
        CircleBufferAdd(buffer, &sample);
    }

    uint32_t samples = CircleBufferSamples(buffer);
    uint32_t first_sample = samples>samples_write_count ? samples-samples_write_count : 0;

    for(uint32_t i=0; i<samples_write_count; i++)
    {
        int32_t sample = i+offset;
        int32_t sample_out;
        CircleBufferGetSample(buffer, i+first_sample, &sample_out);
        assert(sample_out==sample);
    }
}

int main()
{
    printf("Circle buffer test\n");

    const uint32_t data_size = 128;
    uint8_t data[data_size];
    CircleBuffer buffer;
    CircleBufferInit(&buffer, data, data_size, 4);

    Test0(&buffer);
    Test1(&buffer);
    Test2(&buffer, 0, buffer.data_size_in_samples);
    Test2(&buffer, 2, buffer.data_size_in_samples);
    Test2(&buffer, 12, buffer.data_size_in_samples);
    Test2(&buffer, 200, buffer.data_size_in_samples);
    Test2(&buffer, 12345, buffer.data_size_in_samples);
    Test2(&buffer, 12345, 18);

    return 0;
}
