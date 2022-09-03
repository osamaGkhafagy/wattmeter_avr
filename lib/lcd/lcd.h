#ifndef __LCD_H__
#define __LCD_H__

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#define LCD_DATA_DDR		DDRD
#define LCD_DATA_PORT		PORTD
#define LCD_CTRL_DDR		DDRB
#define LCD_CTRL_PORT		PORTB
#define RS					PB0
#define RW					PB6
#define EN					PB7
/* For 4-bit mode only*/
#define DB4                 PD4
#define DB5                 PD5
#define DB6                 PD6
#define DB7                 PD7
// commands
#define CLEAR_SCREEN        0X01
#define RETURN_HOME         0X02
#define SET_DDRAM_ADDR      0x80
#define MODE_4BIT           0X00
#define MODE_8BIT           0X01

void whisper();
void checkReadiness();
void LCD_send_command(unsigned char command);
void LCD_init();
void LCD_send_character(unsigned char character);
void LCD_send_string(char * data);
void LCD_goTo(uint8_t Row, uint8_t Col);
void LCD_clear();

#endif