#include "stm32h7xx_hal.h"
#include "ADS1251.h"
#include "delay.h"

//#define USE_HARD_SPI

extern SPI_HandleTypeDef hspi4;

static uint16_t last_data_us;
static uint32_t last_data;
static uint16_t data_10ns;

#define ADS1251_IRQ EXTI15_10_IRQn

void ADS1251_Init()
{
    GPIO_InitTypeDef  gpio = {};
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_HIGH;

#ifndef USE_HARD_SPI
    gpio.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio.Pin       = ADS1251_SCK_PIN;
    HAL_GPIO_Init(ADS1251_SCK_PORT, &gpio);
#endif

    gpio.Mode      = GPIO_MODE_IT_FALLING;
    gpio.Pin       = ADS1251_DATA_PIN;
    HAL_GPIO_Init(ADS1251_DATA_PORT, &gpio);

    /* Enable and set EXTI lines 15 to 10 Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(ADS1251_IRQ, 2, 0);
    HAL_NVIC_EnableIRQ(ADS1251_IRQ);
}

int32_t ADS1251_Get()
{
    if(last_data&0x800000)
        return last_data|0xFF000000u;

    return last_data;
}

//На ассемблере напишем код по тактикам
//Предполагается, что запускаться будет на 100 МГц
__attribute__((always_inline))
inline uint32_t Receive1Bit()
{
    uint32_t data;
    ADS1251_SCK_PORT->BSRR = ADS1251_SCK_PIN;
    __asm__ volatile("mov r0, r0");

    data = (ADS1251_DATA_PORT->IDR & ADS1251_DATA_PIN)?1:0;

    ADS1251_SCK_PORT->BSRR = ((uint32_t)ADS1251_SCK_PIN) << 16;
    return data;
}



uint32_t SoftwareSPI24()
{
    uint32_t data = 0;

    /*
    for(int bit=0; bit<24; bit++)
    {
        HAL_GPIO_WritePin(ADS1251_SCK_PORT, ADS1251_SCK_PIN, 1);
        for(volatile int i=0; i<10; i++);

        data = (data<<1) | (HAL_GPIO_ReadPin(ADS1251_DATA_PORT, ADS1251_DATA_PIN)?1:0);

        HAL_GPIO_WritePin(ADS1251_SCK_PORT, ADS1251_SCK_PIN, 0);
        for(volatile int i=0; i<10; i++);
    }
    */

    for(int bit=0; bit<24; bit++)
    {
        data = (data<<1) | Receive1Bit();
    }

    return data;
}

uint32_t HardwareSPI24()
{
    static uint32_t data = 0;
    HAL_SPI_Receive(&hspi4, (uint8_t*)&data, 3, 10);
    return data;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint16_t start = Time10Ns();
    uint16_t cur_us = TimeUs();
    uint16_t delta_us = cur_us-last_data_us;
    last_data_us = cur_us;

    if(delta_us<5)
    {
        HAL_NVIC_DisableIRQ(ADS1251_IRQ);
#ifdef USE_HARD_SPI
        last_data = HardwareSPI24();
#else
        last_data = SoftwareSPI24();
#endif
        HAL_NVIC_EnableIRQ(ADS1251_IRQ);
        uint16_t end = Time10Ns();
        data_10ns = end-start;
    }
}

uint16_t ADS1251_TestSPI_10Ns()
{
    return data_10ns;
}
