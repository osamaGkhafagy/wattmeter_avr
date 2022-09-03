#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

void eeprom_init();
void eeprom_write(uint8_t* data, uint16_t addr, uint16_t data_length);
void eeprom_read(uint8_t* data, uint16_t addr, uint16_t data_length);

#endif