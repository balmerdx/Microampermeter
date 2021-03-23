#pragma once

void SceneOscilloscopeStart();

typedef enum OscilloscopeEncoderEnum
{
    ENCODER_SCALE_CURRENT,
    ENCODER_SCALE_TIME,
    ENCODER_OFFSET_TIME
} OscilloscopeEncoderEnum;

void SetOscilloscopeEncoder(OscilloscopeEncoderEnum type);

void OscilloscopeTriggerStart();
