#include "main.h"
#include "flash_out_of_bound.h"
#include "UTFT.h"
#include <string.h>
#include "float_to_string.h"

#define FLASH_TIMEOUT_VALUE              10000U /* 10 s */

HAL_StatusTypeDef HAL_FLASH_ProgramBank1(uint32_t FlashAddress, uint32_t DataAddress)
{
    HAL_StatusTypeDef status;
    __IO uint32_t *dest_addr = (__IO uint32_t *)FlashAddress;
    __IO uint32_t *src_addr = (__IO uint32_t*)DataAddress;
    uint32_t bank;
    uint8_t row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;

    /* Process Locked */
    __HAL_LOCK(&pFlash);

    bank = FLASH_BANK_1;

    /* Reset error code */
    pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

    if(status == HAL_OK)
    {
        /* Set PG bit */
        SET_BIT(FLASH->CR1, FLASH_CR_PG);

        __ISB();
        __DSB();

        /* Program the flash word */
        do
        {
            *dest_addr = *src_addr;
            dest_addr++;
            src_addr++;
            row_index--;
        } while (row_index != 0U);

        __ISB();
        __DSB();

        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

        /* If the program operation is completed, disable the PG */
        CLEAR_BIT(FLASH->CR1, FLASH_CR_PG);
    }

    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);

    return status;
}


void FLASH_Erase_SectorBank1(uint32_t Sector, uint32_t VoltageRange)
{
#if defined (FLASH_CR_PSIZE)
  assert_param(IS_VOLTAGERANGE(VoltageRange));
#else
  UNUSED(VoltageRange);
#endif /* FLASH_CR_PSIZE */

  {
#if defined (FLASH_CR_PSIZE)
    /* Reset Program/erase VoltageRange and Sector Number for Bank1 */
    FLASH->CR1 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);

    FLASH->CR1 |= (FLASH_CR_SER | VoltageRange | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#else
    /* Reset Sector Number for Bank1 */
    FLASH->CR1 &= ~(FLASH_CR_SNB);

    FLASH->CR1 |= (FLASH_CR_SER | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#endif /* FLASH_CR_PSIZE */
  }
}

HAL_StatusTypeDef HAL_FLASHEx_EraseBank1(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t sector_index;

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));
  assert_param(IS_FLASH_BANK(pEraseInit->Banks));

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Reset error code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed on Bank1 */
  if((pEraseInit->Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  if(status == HAL_OK)
  {
    if(pEraseInit->TypeErase == FLASH_TYPEERASE_SECTORS)
    {
      /*Initialization of SectorError variable*/
      *SectorError = 0xFFFFFFFFU;

      /* Erase by sector by sector to be done*/
      for(sector_index = pEraseInit->Sector; sector_index < (pEraseInit->NbSectors + pEraseInit->Sector); sector_index++)
      {
        FLASH_Erase_SectorBank1(sector_index, pEraseInit->VoltageRange);

        if((pEraseInit->Banks & FLASH_BANK_1) == FLASH_BANK_1)
        {
          /* Wait for last operation to be completed */
          status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE, FLASH_BANK_1);

          /* If the erase operation is completed, disable the SER Bit */
          FLASH->CR1 &= (~(FLASH_CR_SER | FLASH_CR_SNB));
        }

        if(status != HAL_OK)
        {
          /* In case of error, stop erase procedure and return the faulty sector */
          *SectorError = sector_index;
          break;
        }
      }
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}


static uint32_t original_data[FLASH_NB_32BITWORD_IN_FLASHWORD];
int TestAndCompareFlashPattern(uint32_t Sector, bool pattetn_invert)
{
    FLASH_EraseInitTypeDef et;
    et.TypeErase = FLASH_TYPEERASE_SECTORS;
    et.Banks = FLASH_BANK_1;
    et.Sector = Sector;
    et.NbSectors = 1;
    et.VoltageRange = FLASH_VOLTAGE_RANGE_4;

    uint32_t SectorError;
    if(HAL_FLASHEx_EraseBank1(&et, &SectorError)!=HAL_OK)
    {
        return -1000-(int)SectorError;
    }

    FLASH_Erase_SectorBank1(Sector, FLASH_VOLTAGE_RANGE_4);

    uint32_t sector_start = FLASH_BANK1_BASE + Sector*FLASH_SECTOR_SIZE;
    uint32_t magic = 1000;//0x1234567u;
    if(HAL_FLASH_Unlock()!=HAL_OK)
    {
        return -333;
    }

    int bytes_to_write = 32;//FLASH_SECTOR_SIZE
    for(uint32_t i=0; i<bytes_to_write; i+=4)
    {
        uint32_t idx = i/4;
        uint32_t p = (magic+idx);
        if(pattetn_invert)
            p = ~p;
        original_data[idx%FLASH_NB_32BITWORD_IN_FLASHWORD] = p;

        if((idx+1)%FLASH_NB_32BITWORD_IN_FLASHWORD==0)
        {
            int y = UTFT_getDisplayYSize() - UTF_Height()*2;
            char str[60];
            strcpy(str, "idx=");
            intToString(str+strlen(str), idx, 8, ' ');
            UTFT_setColor(VGA_WHITE);
            UTFT_setBackColor(VGA_BLACK);
            UTF_DrawStringJustify(0, y, str, UTFT_getDisplayXSize(), UTF_CENTER);

            uint32_t* data = original_data;
            if(HAL_FLASH_ProgramBank1(sector_start+i, (uint32_t)data)!=HAL_OK)
            {
                //return -idx;
            }
        }
    }
    HAL_FLASH_Lock();

    int errors_count = 0;
    for(uint32_t i=0; i<bytes_to_write; i+=4)
    {
        uint32_t idx = i/4;
        uint32_t p = (magic+idx);
        if(pattetn_invert)
            p = ~p;

        uint32_t data_in_flash = *(uint32_t*)(sector_start+i);
        if(data_in_flash != p)
            errors_count++;
    }

    return errors_count;
}
