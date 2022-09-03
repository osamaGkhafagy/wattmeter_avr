#include "eeprom.h"

void eeprom_init()
{
    // setting the EEPROM programming mode
    // erase and write in atomic operation ( takes 3.4ms)
    EECR &= ~(1 << EEPM0) | (1 << EEPM1);
}
void eeprom_write(uint8_t* data, uint16_t addr, uint16_t data_length)
{
    for(uint16_t i = 0; i < data_length; i++)
    {
        // wait until the current write operation ends
        while(EECR & (1 << EEPE));
        // set the right address
        EEAR = addr + i;
        // write one byte
        EEDR = *(data + i);
        // set the master write enable EEMPE to 1
        EECR |= (1 << EEMPE);
        // set EEPE to 1 within 4 clock cycles
        EECR |= (1 << EEPE);
    }
}
void eeprom_read(uint8_t* data, uint16_t addr, uint16_t data_length)
{
    for(uint16_t i = 0; i < data_length; i++)
    {
        // wait if there is a current write operation
        while(EECR & (1 << EEPE));
        // set the right address
        EEAR = addr + i;
        // set EERE to 1
        EECR |= (1 << EERE);
        // read one byte
        *(data + i) = EEDR;
    }
}