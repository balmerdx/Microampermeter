#include "stm32h7xx_hal.h"
#include "qspi_mem.h"

#define QSPI_TIMEOUT_DEFAULT_VALUE 100

bool use_qspi = true;

static volatile bool RxCplt = false;
static volatile bool TxCplt = false;

void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    RxCplt = true;
}

void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    TxCplt = true;
}


bool QspiMemInit(QSPI_HandleTypeDef* hqspi)
{

    QSPI_CommandTypeDef sCommand;
    if(use_qspi)
    {
        sCommand.Instruction = 0x35 ; //Enter QSPI mode

        sCommand.Address = 0;
        sCommand.AlternateBytes = 0;
        sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
        sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
        sCommand.DummyCycles = 0;
        sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
        sCommand.AddressMode = QSPI_ADDRESS_NONE;
        sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        sCommand.DataMode = QSPI_DATA_NONE;
        sCommand.NbData = 0;
        sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
        sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
        sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
        if(HAL_QSPI_Command(hqspi, &sCommand, QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return false;
        }
    }

    return true;
}

bool QspiMemRead(QSPI_HandleTypeDef* hqspi, uint32_t address, uint32_t count, uint8_t* buffer)
{
    if(count==0)
        return true;

    QSPI_CommandTypeDef sCommand;
    if(use_qspi)
    {
        sCommand.Instruction = 0xEB; //Quad spi read
        sCommand.Address = address;
        sCommand.AlternateBytes = 0;
        sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
        sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
        sCommand.DummyCycles = 6;
        sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
        sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
        sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        sCommand.DataMode = QSPI_DATA_4_LINES;
        sCommand.NbData = count;
        sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
        sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
        sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    } else
    {
        sCommand.Instruction = 0x03; //Slow spi read
        sCommand.Address = address;
        sCommand.AlternateBytes = 0;
        sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
        sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
        sCommand.DummyCycles = 0;
        sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
        sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
        sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        sCommand.DataMode = QSPI_DATA_1_LINE;
        sCommand.NbData = count;
        sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
        sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
        sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    }

    if(HAL_QSPI_Command(hqspi, &sCommand, QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    //if(HAL_QSPI_Receive(hqspi, buffer, QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) return false;

    RxCplt = false;
    if(HAL_QSPI_Receive_DMA(hqspi, buffer) != HAL_OK)
    {
        return false;
    }

    while(!RxCplt);

    return true;
}

bool QspiMemWrite(QSPI_HandleTypeDef* hqspi, uint32_t address, uint32_t count, uint8_t* buffer)
{
    if(count==0)
        return true;
    QSPI_CommandTypeDef sCommand;
    if(use_qspi)
    {
        sCommand.Instruction = 0x38; //Quad spi write
        sCommand.Address = address;
        sCommand.AlternateBytes = 0;
        sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
        sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
        sCommand.DummyCycles = 0;
        sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
        sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
        sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        sCommand.DataMode = QSPI_DATA_4_LINES;
        sCommand.NbData = count;
        sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
        sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
        sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    } else
    {
        sCommand.Instruction = 0x02; //Spi write
        sCommand.Address = address;
        sCommand.AlternateBytes = 0;
        sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
        sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
        sCommand.DummyCycles = 0;
        sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
        sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
        sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        sCommand.DataMode = QSPI_DATA_1_LINE;
        sCommand.NbData = count;
        sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
        sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
        sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    }

    if(HAL_QSPI_Command(hqspi, &sCommand, QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    //if(HAL_QSPI_Transmit_DMA(hqspi, buffer, QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) return false;

    TxCplt = false;
    if(HAL_QSPI_Transmit_DMA(hqspi, buffer) != HAL_OK)
    {
        return false;
    }


    while(!TxCplt);

    return true;

}
