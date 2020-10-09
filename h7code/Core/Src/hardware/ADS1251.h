#pragma once

#define ADS1251_SCK_PIN GPIO_PIN_12
#define ADS1251_SCK_PORT GPIOE

#define ADS1251_DATA_PIN GPIO_PIN_14
#define ADS1251_DATA_PORT GPIOE

void ADS1251_Init();

//Меджленная функция, исключительно для теста
int32_t ADS1251_Get();

uint16_t ADS1251_TestSPI_10Ns();

