#ifndef __ZMPT101_H__
#define __ZMPT101_H__

#include <avr/io.h>
#include <stdint.h>
#include "adc.h"
#include "timer1.h"
#include "i2c_lcd.h"
#include <stdlib.h>

#define     ZMPT101_DDR     DDRC
#define     ZMPT101_PORT    PORTC
#define     ZMPT101_PIN     PC0

volatile double zmpt101_get_ac_voltage();
uint16_t zmpt101_calibrate();

#endif