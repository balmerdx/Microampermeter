#include "main.h"
#include "qspi.h"

#define QSPI_TIMEOUT_DEFAULT_VALUE 100

extern QSPI_HandleTypeDef hqspi;
extern volatile bool Qspi_RxCplt;
extern volatile bool Qspi_TxCplt;


bool Qspi1Line(uint8_t instruction, const void* data, uint32_t data_size, bool send,
               uint32_t address24, bool enable_address)
{
    QSPI_CommandTypeDef sCommand;

    sCommand.Instruction = instruction;
    sCommand.Address = address24;
    sCommand.AlternateBytes = 0;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = enable_address?QSPI_ADDRESS_1_LINE:QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = data_size?QSPI_DATA_1_LINE:QSPI_DATA_NONE;
    sCommand.NbData = data_size;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_HALF_CLK_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    if(HAL_QSPI_Command(&hqspi, &sCommand, QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    if(data_size)
    {
        if(send)
        {
            Qspi_TxCplt = false;
            if(HAL_QSPI_Transmit_DMA(&hqspi, (uint8_t*)data) != HAL_OK)
            {
                return false;
            }


            uint32_t start_time = TimeMs();
            while(!Qspi_TxCplt)
            {
                if( (uint32_t)(TimeMs()-start_time) > QSPI_TIMEOUT_DEFAULT_VALUE)
                    return false;
            }
        } else
        {
            Qspi_RxCplt = false;
            if(HAL_QSPI_Receive_DMA(&hqspi, (uint8_t*)data) != HAL_OK)
            {
                return false;
            }

            uint32_t start_time = TimeMs();
            while(!Qspi_RxCplt)
            {
                if( (uint32_t)(TimeMs()-start_time) > QSPI_TIMEOUT_DEFAULT_VALUE)
                    return false;
            }
        }
    }

    return true;
}

bool QspiSend1Line(uint8_t instruction, const void* data, uint32_t data_size)
{
    return Qspi1Line(instruction, data, data_size, true, 0, false);
}

bool QspiReceive1Line(uint8_t instruction, void* data, uint32_t data_size)
{
    return Qspi1Line(instruction, data, data_size, false, 0, false);
}

bool QspiSend1LineAddr24(uint8_t instruction, uint32_t addr, const void* data, uint32_t data_size)
{
    return Qspi1Line(instruction, data, data_size, true, addr, true);
}

bool QspiReceive1LineAddr24(uint8_t instruction, uint32_t addr, void* data, uint32_t data_size)
{
    return Qspi1Line(instruction, data, data_size, false, addr, true);
}
