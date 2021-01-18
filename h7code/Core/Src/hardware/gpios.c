#include "stm32h7xx_hal.h"
#include "gpios.h"

#define RESISTOR_PORT GPIOA
#define RESISTOR_PIN_100_Om GPIO_PIN_3
#define RESISTOR_PIN_10_Om  GPIO_PIN_2
#define RESISTOR_PIN_1_Om   GPIO_PIN_1

#define EN_V_PORT GPIOA
#define EN_V_PIN  GPIO_PIN_0

static RESISTOR last_r;

void GpiosInit()
{
    GPIO_InitTypeDef  gpio = {};
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_HIGH;

    gpio.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio.Pin       = RESISTOR_PIN_100_Om|RESISTOR_PIN_10_Om|RESISTOR_PIN_1_Om;
    HAL_GPIO_Init(RESISTOR_PORT, &gpio);

    gpio.Pin       = EN_V_PIN;
    HAL_GPIO_Init(EN_V_PORT, &gpio);

    last_r = RESISTOR_1_Kom;

    SetEnableV(true);
}

void SetResistor(RESISTOR r)
{
    last_r = r;
    HAL_GPIO_WritePin(RESISTOR_PORT, RESISTOR_PIN_100_Om, r==RESISTOR_100_Om);
    HAL_GPIO_WritePin(RESISTOR_PORT, RESISTOR_PIN_10_Om, r==RESISTOR_10_Om);
    HAL_GPIO_WritePin(RESISTOR_PORT, RESISTOR_PIN_1_Om, r==RESISTOR_1_Om);
}

void SetEnableV(bool enable)
{
    HAL_GPIO_WritePin(EN_V_PORT, EN_V_PIN, enable);
}

RESISTOR GetResistor()
{
   return last_r;
}

static float PAR(float R1, float R2)
{
    return R1*R2/(R1+R2);
}

float GetResistorValue()
{
    float R1K = 1000;
    float R100 = 100;
    float R10 = 10;
    float R1 = 1.2f;

    switch(last_r)
    {
    default:
    case RESISTOR_1_Kom: return R1K;
    case RESISTOR_100_Om: return PAR(R1K, R100);
    case RESISTOR_10_Om: return PAR(R1K, R10);
    case RESISTOR_1_Om: return PAR(R1K, R1);
    }
}

