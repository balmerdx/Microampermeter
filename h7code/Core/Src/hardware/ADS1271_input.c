#include "stm32h7xx_hal.h"
#include "ADS1271_input.h"
#include "delay.h"

#define ADS1271_MODE_PORT GPIOE
#define ADS1271_MODE_PIN GPIO_PIN_3

#define ADS1271_NOT_PDWN_PORT GPIOE
#define ADS1271_NOT_PDWN_PIN GPIO_PIN_1


#define AUDIO_BUFFER_SIZE 64
uint32_t audio_buf[AUDIO_BUFFER_SIZE] __attribute__((section(".d2_data"))) __attribute__ ((aligned (32)));

extern SAI_HandleTypeDef hsai_BlockA1;

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    (void)hsai;
    uint32_t* data = audio_buf;
    SCB_InvalidateDCache_by_Addr(data, AUDIO_BUFFER_SIZE/2*sizeof(audio_buf[0]));
    ADS1271_ReceiveData(data, AUDIO_BUFFER_SIZE/2);
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    (void)hsai;
    uint32_t* data = audio_buf;
    SCB_InvalidateDCache_by_Addr(data+AUDIO_BUFFER_SIZE/2, AUDIO_BUFFER_SIZE/2*sizeof(audio_buf[0]));
    ADS1271_ReceiveData(audio_buf+AUDIO_BUFFER_SIZE/2, AUDIO_BUFFER_SIZE/2);
}

void ADS1271_Init()
{
    GPIO_InitTypeDef  gpio = {};
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_LOW;

    gpio.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio.Pin       = ADS1271_MODE_PIN;
    HAL_GPIO_Init(ADS1271_MODE_PORT, &gpio);

    gpio.Pin       = ADS1271_NOT_PDWN_PIN;
    HAL_GPIO_Init(ADS1271_NOT_PDWN_PORT, &gpio);

    HAL_GPIO_WritePin(ADS1271_MODE_PORT, ADS1271_MODE_PIN, 0);
    HAL_GPIO_WritePin(ADS1271_NOT_PDWN_PORT, ADS1271_NOT_PDWN_PIN, 0);
    DelayMs(200);
    HAL_GPIO_WritePin(ADS1271_NOT_PDWN_PORT, ADS1271_NOT_PDWN_PIN, 1);
    DelayMs(10);
}

void ADS1271_Start()
{
    HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t*)&audio_buf[0], AUDIO_BUFFER_SIZE);
}
