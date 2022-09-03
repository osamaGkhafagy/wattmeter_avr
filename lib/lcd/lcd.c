#include "lcd.h"
#include "serial_328p.h"

void whisper()
{
	LCD_CTRL_PORT |= (1 << EN);
	asm volatile ("nop");
	asm volatile ("nop");
	LCD_CTRL_PORT &= ~(1 << EN);
}

void checkReadiness()
{
	//data direction as input
	LCD_DATA_DDR = 0x00;
	//Reading from the lcd
	LCD_CTRL_PORT |= (1 << RW);
	//Enabling the command mode
	LCD_CTRL_PORT &= ~(1 << RS);

	while (LCD_DATA_PORT >= 0x80)
	{
		whisper();
	}
	//bringing the dataPort back as output
	LCD_DATA_DDR = 0xFF;
}

void LCD_send_command(unsigned char command)
{
	checkReadiness();
	LCD_DATA_PORT = command;
	LCD_CTRL_PORT &= ~(1 << RS);	//command mode
	LCD_CTRL_PORT &= ~(1<< RW);	//write mode
	whisper();
	LCD_DATA_PORT = 0;
	_delay_ms(5);
}

void LCD_init()
{
	//Turning the control pins into outputs
	LCD_CTRL_DDR |= (1 << RS) | (1 << RW) | (1 << EN);
	// configure data port as output initially
	LCD_DATA_DDR |= 0xFF;
	_delay_ms(15);
	//Clear Display
	// initialization sequence
	LCD_send_command(0x30);
	_delay_ms(5);
	LCD_send_command(0x30);
	_delay_us(100);
	LCD_send_command(0x30);
	// 2 line display (supports up to 4 lines for LCD1604)
	LCD_send_command(0x38);
	// cursor increment, no shift
	LCD_send_command(0x06);
	// display on, cursor blinks
	LCD_send_command(0X0E);
	_delay_us(50);
	//display and cursor on,blinking off
	LCD_send_command(0X01);
	_delay_us(50);
}

void LCD_send_character(unsigned char character)
{
	checkReadiness();
	LCD_DATA_PORT = character;
	LCD_CTRL_PORT |= (1 << RS);		//sending character
	LCD_CTRL_PORT &= ~(1 << RW);		//write mode
	whisper();
	LCD_DATA_PORT = 0;
}

void LCD_send_string(char * data)
{
	while (*data > 0)
	{
		LCD_send_character(*data);
		_delay_ms(5);
		data++;
	}
	
}

void LCD_goTo(uint8_t row, uint8_t col)
{
	// check boundaries
    if((row > 4) | (col > 20))
        return;
	uint8_t location = 0;
	LCD_goTo(1, 1);
	_delay_ms(2);
	//Go to some destination
	if (row == 2)
	{
		location = 0x40;
	}
    else if(row == 3)
    {
        location = 0X14;
    }
    else if(row == 4)
    {
        location = 0X54;
    }
	location = location + col - 1;
	LCD_goTo(SET_DDRAM_ADDR | location, true);
	_delay_us(50);
}

void LCD_clear()
{
	LCD_send_command(0x01);
	
}