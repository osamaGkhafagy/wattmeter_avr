#ifndef __ACS712_H__
#define __ACS712_H__

#include <avr/io.h>
#include <stdint.h>
#include "adc.h"
#include "timer1.h"

#define     ACS712_DDR     DDRC
#define     ACS712_PORT    PORTC
#define     ACS712_PIN     PC1

volatile double acs712_get_ac_current();
uint16_t acs712_calibrate();

#endif