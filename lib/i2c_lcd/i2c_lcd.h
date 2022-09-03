/* TWI Interface with LCD using PCF8574 IC */

/* 
	
	PCF8574 has 8-Bit Port for I/O operation 
	P0 = RS;		//--- Register Select
	P1 = RW;		//--- Read / Write Operation Select
	P2 = EN;		//--- Latch to Data Register Enable Pin
	P3 = Backlight;	//--- LCD Backlight Control
	P4 = D4;		//--- LCD pin D4
	P5 = D5;		//--- LCD pin D5
	P6 = D6;		//--- LCD pin D6
	P7 = D7;		//--- LCD pin D7	

*/
#ifndef __I2C_LCD_H__
#define __I2C_LCD_H__
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"

#define RS_BIT          0
#define RW_PIN          1
#define EN_BIT          2
#define LED_K_BIT       3
#define PCF8574_SLA     0X4e
#define CLEAR_CMD       0X01
#define RETURN_HOME     0X02
#define SET_DDRAM_ADDR  0x80
#define	LCD_ROWS		4
#define LCD_COLUMNS		16

// functions prototypes
void i2c_lcd_write(uint8_t data, bool is_command);
void i2c_lcd_write_str(char * data);
void i2c_lcd_clear();
void i2c_lcd_clear_row(uint8_t row);
void i2c_lcd_go_to(uint8_t row, uint8_t col);
void i2c_lcd_init();


#endif