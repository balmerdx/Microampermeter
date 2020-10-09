#include "stm32h7xx_hal.h"
#include "gpios.h"

#define RESISTOR_PORT GPIOE
#define RESISTOR_PIN_100_Om GPIO_PIN_7
#define RESISTOR_PIN_10_Om  GPIO_PIN_8
#define RESISTOR_PIN_1_Om   GPIO_PIN_9

#define EN_V_PORT GPIOE
#define EN_V_PIN  GPIO_PIN_0

void GpiosInit()
{
    GPIO_InitTypeDef  gpio = {};
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_LOW;

    gpio.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio.Pin       = RESISTOR_PIN_100_Om|RESISTOR_PIN_10_Om|RESISTOR_PIN_1_Om;
    HAL_GPIO_Init(RESISTOR_PORT, &gpio);

    gpio.Pin       = EN_V_PIN;
    HAL_GPIO_Init(EN_V_PORT, &gpio);

    SetEnableV(true);
}

void SetResistor(RESISTOR r)
{
    HAL_GPIO_WritePin(RESISTOR_PORT, RESISTOR_PIN_100_Om, r==RESISTOR_100_Om);
    HAL_GPIO_WritePin(RESISTOR_PORT, RESISTOR_PIN_10_Om, r==RESISTOR_10_Om);
    HAL_GPIO_WritePin(RESISTOR_PORT, RESISTOR_PIN_1_Om, r==RESISTOR_1_Om);
}

void SetEnableV(bool enable)
{
    HAL_GPIO_WritePin(EN_V_PORT, EN_V_PIN, enable);

}
