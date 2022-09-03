#include "i2c_lcd.h"

// writes data to the lcd port nibble by nibble
void i2c_lcd_write(uint8_t data, bool is_command)
{
    // temp variable to hold final data to transmit to LCD port
	uint8_t lcd_data;
    // Masking the upper four bits and discarding the lower ones
	lcd_data = (data & 0xF0);
    // if command, RS = 0, RW = 0
    // if data, RS = 1, RW = 0
    if(!is_command)
        lcd_data |= (1 << RS_BIT);
    // now toggle EN_BIT so the LCD can latch data
    lcd_data |= (1 << EN_BIT);
	lcd_data |= (1 << LED_K_BIT);
    // transmit data to LCD port through PCF8574
	i2c_transmit_packet((PCF8574_SLA | WRITE_BIT), &lcd_data, 1);
	_delay_ms(1);
	lcd_data &= ~(1 << EN_BIT);
	lcd_data |= (1 << LED_K_BIT);
	i2c_transmit_packet((PCF8574_SLA | WRITE_BIT), &lcd_data, 1);
	_delay_ms(1);
	
	// Masking the lower four bits and discarding the upper ones
	lcd_data = ((data << 4) & 0xF0);
	// if command, RS = 0, RW = 0
    // if data, RS = 1, RW = 0
    if(!is_command)
        lcd_data |= (1 << RS_BIT);
    // now toggle EN_BIT so the LCD can latch data
    lcd_data |= (1 << EN_BIT);
	lcd_data |= (1 << LED_K_BIT);
    // transmit data to LCD port through PCF8574
	i2c_transmit_packet((PCF8574_SLA | WRITE_BIT), &lcd_data, 1);
	_delay_ms(1);
	lcd_data &= ~(1 << EN_BIT);
	lcd_data |= (1 << LED_K_BIT);
	//lcd_data &= ~(1 << LED_K_BIT);
	i2c_transmit_packet((PCF8574_SLA | WRITE_BIT), &lcd_data, 1);
	_delay_ms(1);
}
// write strings on lcd
void i2c_lcd_write_str(char * data)
{
	while (*data != '\0')
	{
		i2c_lcd_write(*data, false);
		_delay_ms(5);
		data++;
	}
	
}
// clear lcd screen and return to home
void i2c_lcd_clear()
{
	i2c_lcd_write(RETURN_HOME, true);
	i2c_lcd_write(CLEAR_CMD, true);
}

// clear the lcd by row
void i2c_lcd_clear_row(uint8_t row)
{
	i2c_lcd_go_to(row, 1);
	for(uint8_t i = 0; i < LCD_COLUMNS; i++)
		i2c_lcd_write_str(" ");
}
// go to row(1:4), col(1:20)
void i2c_lcd_go_to(uint8_t row, uint8_t col)
{
    // check boundaries
    if((row > 4) | (col > 20))
        return;
	uint8_t location = 0;
	//return home
	i2c_lcd_write(RETURN_HOME, true);		        //--- Return Home
	_delay_ms(2);
	//Go to some destination
	if (row == 2)
	{
		location = 0x40;
	}
    else if(row == 3)
    {
        location = 0X10;
    }
    else if(row == 4)
    {
        location = 0X50;
    }
	location = location + col - 1;
	i2c_lcd_write(SET_DDRAM_ADDR | location, true);
	_delay_us(50);
}

void i2c_lcd_init()
{	
	// EN = 1 for 25us to initialize Sequence
	i2c_lcd_write((1 << EN_BIT), true);
	_delay_us(25);
	i2c_lcd_write(0x03, true);				        //--- Initialize Sequence
    _delay_ms(5);
	i2c_lcd_write(0x03, true);				        //--- Initialize Sequence
    _delay_us(100);
	i2c_lcd_write(0x03, true);				        //--- Initialize Sequence
	i2c_lcd_write(RETURN_HOME, true);		        //--- Return Home
	i2c_lcd_write(0x28, true);				        //--- Function set: 4-Bit Mode 2 line Select
	i2c_lcd_write(0x0C, true);				        //--- Display on/off: Display on, Cursor on, Blinking on
	i2c_lcd_clear();				                //--- Clear LCD
	i2c_lcd_write(0x06, true);				        //--- Entry mode set: AC=AC+1, no shift
	i2c_lcd_write((SET_DDRAM_ADDR | 0X00), true);	//--- Set DDRAM address: Row 1 Column 1 Address
	i2c_lcd_write_str("    Welcome    ");	        //--- String Send to LCD
	_delay_ms(1000);				                //--- 1s Delay
	i2c_lcd_clear();				                //--- Clear LCD
	i2c_lcd_write(RETURN_HOME, true);		        //--- Return Home
}