#pragma once

#define W25Q32
//#define M25P16

void m25p16_init();


/** \brief		Read 3 bytes of ID from the M26P16. These should always be 0x20,0x20,0x15
 *	\param		*mem_ptr	    Pointer to location to store values returned.
 *
 */
void m25p16_read_ram_id(uint8_t* mem_ptr);


//Read ID and check validity
//and determine m25p16_sectors_count
bool m25p16_read_ram_id_and_check();

uint16_t m25p16_sectors_count();

#define m25p16_sector_shift() 16
#define m25p16_sector_size() (1<<m25p16_sector_shift())

#define m25p16_size_bytes() (((uint32_t)m25p16_sectors_count()) <<m25p16_sector_shift())



/** \brief		Select the M25P16 and return 1 byte from the Status register.
 *	\return					returns device status byte.
 *
 */
uint8_t m25p16_read_ram_status(void);
uint8_t m25p16_read_ram_status2(void);
uint8_t m25p16_read_ram_status3(void);



/** \brief		Erase the M25P16.
 *
 *				This function issues an erase command, then blocks until the command is complete as shown by
 *				 the status register being zero.
 *				Note that the erase actually sets all bits to 1. The page program can set bits to 0, but NOT to 1.
 *				Therefore each page should be considered 'write once' between erase cycles.
 */
void m25p16_ram_bulk_erase(void);



/** \brief		Erase 64 kb data
 *	\param      addr - must be align on 64 kb block
 *
 *				This function issues a sector erase command, then blocks until the command is complete as shown by
 *				 the status register being zero
 *				For future expansion, no masking of the sector byte to ensure that it does not contain values >31 takes place
 *				Note that the erase actually sets all bits to 1. The page program can set bits to 0, but NOT to 1.
 *				Therefore each page should be considered 'write once' between erase cycles.
 */
void m25p16_ram_erase64k(uint32_t addr);

/** \brief		Erase 4 kb data
 *	\param      addr - must be align on 4 kb block
 * !!!!!Work only on Winbond memory!!!!! Not work on M25P16!!!!!
 */
void m25p16_ram_erase4k(uint32_t addr);

//Более удобный функции чтения/записи в сектор данных.
//Разбиваем на flash_page команды внутри
//offset - смещение относительно начала flash = [0..m25p16_size_bytes())
//size - количество данных для записи size=(0-65535)- (offset%m25p16_sector_size())
//Т.е. пересекать границу сектора нельзя.
void m25p16_read(uint32_t addr, uint16_t size, void* mem_ptr);
void m25p16_write(uint32_t addr, uint16_t size, const void *mem_ptr);

/** \brief		Write to the status register on the M25P16.
 *	\param		status		Value to write
 *
 *				This function blocks until the bottom bit of the status register is clear = device ready.
 */
void m25p16_write_ram_status(uint8_t status);



/** \brief		Issue the command to bring the M25P16 out of power down mode.
 *
 *				This function has no effect if the device is currently in one of the erase modes.
 *				At power up the deice will be in standby mode, there is no need to issue the power_up_flash_ram() command after a power up.
 */
void m25p16_power_up_flash_ram(void);



/** \brief		Issue the command to put the M25P16 into power down mode.
 *
 *				In Power down mode the device ignores all erase and program instructions.
 *
 *				In this mode the device draws 1uA typically.
 *				Use the power_up_flash_ram() command to bring the device out of power down mode.
 *				Removing power completely will also cancel the Deep power down mode - it will power up again in standby mode.
 */
void m25p16_power_down_flash_ram(void);

