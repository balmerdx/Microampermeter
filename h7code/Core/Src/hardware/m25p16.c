#include "main.h"
#include "m25p16.h"

/*
 * Initial rev 0 - only m25p16 micron support
 * Balmer rev 1 - add W25Q32 winbond support
*/

/* Support for M25P16 2Mbyte flash RAM 


BEWARE  Programming can only set bits to zero. IT CANNOT CHANGE A ZERO TO A ONE
You must use the Bulk or sector erase to set bits back to a one

and RAM registers

	RAM_RDID
	RAM_WREN
	RAM_BE
	RAM_PP
	RAM_RDSR

Additionally, RAM_CS must be defined as an output on the appropriate DDR

*/

// Register definitions for M25P16 flash ram
#define RAM_WREN 0x06	// write enable
#define RAM_WRDI 0x04	// write disable
#define RAM_RDID 0x9F	// read id
#define RAM_RDSR 0x05	// read status
#define RAM_RDSR2 0x35	// read status
#define RAM_RDSR3 0x15	// read status

#define RAM_WRSR 0x01	// write status
#define RAM_READ 0x03	// read data
#define RAM_FASTREAD 0x0B
#define RAM_PP 0x02		// page program
#define RAM_ERASE64K 0xD8	// erase 64 kilobyte data
#define RAM_ERASE4K 0x20	// erase 64 kilobyte data
#define RAM_BE 0xC7		// bulk erase
#define RAM_DP 0xB9		// deep power down
#define RAM_RES 0xAB	// release from power down, read electronic signature

#include "qspi.h"

static uint16_t sectors_count = 32;

#define ID0_WINBOND	0xEF
#define ID0_MICRON	0x20

void m25p16_init()
{
}

uint16_t m25p16_sectors_count()
{
    return sectors_count;
}

static uint32_t m25p16_capacity(const uint8_t *id)
{
    uint32_t n = 1048576; // unknown chips, default to 1 MByte

    if (id[2] >= 16 && id[2] <= 31) {
        n = 1ul << id[2];
    } else
    if (id[2] >= 32 && id[2] <= 37) {
        n = 1ul << (id[2] - 6);
    } else
    if ((id[0]==0 && id[1]==0 && id[2]==0) ||
        (id[0]==255 && id[1]==255 && id[2]==255)) {
        n = 0;
    }
    //Serial.printf("capacity %lu\n", n);
    return n;
}


void m25p16_read_ram_id(uint8_t* mem_ptr) {

    QspiReceive1Line(RAM_RDID, mem_ptr, 3);
}

bool m25p16_read_ram_id_and_check()
{
    uint8_t id[3];
    m25p16_read_ram_id(id);

    sectors_count = m25p16_capacity(id) >> m25p16_sector_shift();

    if(!(id[0]==ID0_WINBOND || id[0]==ID0_MICRON))
        return false;

    return (id[1]==0x20 || id[1]==0x40) &&
           (id[2]>=16 && id[2]<=37);
}


uint8_t m25p16_read_ram_status(void) {
	uint8_t status;
    QspiReceive1Line(RAM_RDSR, &status, 1);
	return status;
}

uint8_t m25p16_read_ram_status2(void) {
    uint8_t status;
    QspiReceive1Line(RAM_RDSR2, &status, 1);
    return status;
}

uint8_t m25p16_read_ram_status3(void) {
    uint8_t status;
    QspiReceive1Line(RAM_RDSR3, &status, 1);
    return status;
}

bool m25p16_write_in_progress(void) {

    uint8_t status;
    QspiReceive1Line(RAM_RDSR, &status, 1);
    return (status&1)?true:false;
}

static void WriteEnable()
{
    QspiReceive1Line(RAM_WREN, 0, 0);// write enable instruction
    DelayUs(2);
}

static void WaitWriteComplete(uint16_t delay_us)
{
    DelayUs(delay_us);
    while (m25p16_write_in_progress())
        DelayUs(delay_us);
}

void m25p16_ram_bulk_erase(void) {
    WriteEnable();
    QspiReceive1Line(RAM_BE, 0, 0);// bulk erase instruction
    WaitWriteComplete(100);
}


static void m25p16_ram_eraseXk(uint32_t addr, uint8_t command)
{
    WriteEnable();
    QspiSend1LineAddr24(command, addr, 0, 0);

    DelayUs(10000);
    WaitWriteComplete(100);
}

void m25p16_ram_erase64k(uint32_t addr)
{
    m25p16_ram_eraseXk(addr, RAM_ERASE64K);
}

void m25p16_ram_erase4k(uint32_t addr)
{
    m25p16_ram_eraseXk(addr, RAM_ERASE4K);
}

static void m25p16_read_write_flash_ram(bool read, uint16_t bytes_to_readwrite, uint32_t addr24,
                                 uint8_t* mem_ptr)
{
    if(read)
    {
        QspiReceive1LineAddr24(RAM_READ, addr24, mem_ptr, bytes_to_readwrite);
        DelayUs(2);
    } else
    {
        WriteEnable();
        QspiSend1LineAddr24(RAM_PP, addr24, mem_ptr, bytes_to_readwrite);
        DelayUs(2000);
        WaitWriteComplete(100);
    }
}


// write to the RAM status byte. 0 in bottom bit position = ready 
void m25p16_write_ram_status(uint8_t status) {
    WriteEnable();
    QspiSend1Line(RAM_WRSR, &status, 1);
    WaitWriteComplete(2);
}


void m25p16_power_up_flash_ram(void) {

    QspiReceive1Line(RAM_RES, 0, 0);
    DelayUs(30);
}


void m25p16_power_down_flash_ram(void) {

    QspiReceive1Line(RAM_DP, 0, 0);
}

static void m25p16_read_or_write_flash(bool read, uint32_t addr, uint16_t size,
                                       uint8_t* mem_ptr)
{
    if(size==0)
        return;
    int page_size = 256;
    uint8_t offset_first = addr%page_size;
    uint16_t bytes_to_readwrite;

    //Можно писать только в пределах страницы. Т.е. не пересекать границу 256-ти байт.
    //Первая страница может быть как неполной вначале, так и в конце
    //Остальные страницы всегда начинаются сначала
    bytes_to_readwrite = page_size-offset_first;

    while(size>0)
    {
        if(bytes_to_readwrite>size)
            bytes_to_readwrite = size;
        m25p16_read_write_flash_ram(read, bytes_to_readwrite, addr, mem_ptr);
        size -= bytes_to_readwrite;
        mem_ptr+= bytes_to_readwrite;
        addr += bytes_to_readwrite;
        bytes_to_readwrite = page_size;
    }

}

void m25p16_read(uint32_t addr, uint16_t size, void* mem_ptr)
{
    m25p16_read_or_write_flash(true, addr, size, (uint8_t*)mem_ptr);
}

void m25p16_write(uint32_t addr, uint16_t size, const void* mem_ptr)
{
    m25p16_read_or_write_flash(false, addr, size, (uint8_t*)mem_ptr);
}
