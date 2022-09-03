#ifndef _ADC328p_h_
#define _ADC328p_h_

#include <avr/io.h>
#include <util/delay.h>

void adc_init(uint8_t bit_mode, uint8_t div_factor);
volatile uint16_t adc_reading(uint8_t channel, uint8_t bit_mode);
void adc_disable();

/*----------------------*/

#endif