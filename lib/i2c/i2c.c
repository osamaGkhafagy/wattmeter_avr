#include "i2c.h"

void i2c_init(uint32_t speed)
{
	uint32_t gen_t = 0;
	gen_t = (((F_CPU/speed) - 16) / 2) & 0xFF;
	TWBR = gen_t & 0xFF;
	TWCR = (1 << TWEN);
	//PORTC |= (1 << PORTC5 | 1 << PORTC4);
	
}
//Generate START or REPEATED START
void i2c_generate_start()
{
	//USART_writeln_string("Entering i2c_generate_start()");
	//Generate the START condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	//Wait until START is transmitted
	while(!(TWCR & (1 << TWINT)));
	//USART_writeln_string("Exiting i2c_generate_start()");
}
//Check the validity of transmission using the contents of the TWI status register 
bool i2c_confirm_status(uint8_t status_code)
{
	//USART_writeln_string("Entering i2c_confirm_status()");
	if ((TWSR & 0xF8) == status_code)
	{
		//USART_writeln_string("Exiting i2c_confirm_status()");
		return true;
	}
	else 
	{
		//USART_writeln_string("Exiting i2c_confirm_status()");
		return false;
	}
}

//Addresses the slave device
void i2c_transmit_address(uint8_t address)
{
	//USART_writeln_string("Entering i2c_transmit_address()");
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	//Wait until address is transmitted
	while(!(TWCR & (1 << TWINT)));
	//USART_writeln_string("Exiting i2c_transmit_address()");
}

//Transmits data to a slave device
void i2c_transmit_byte(uint8_t data)
{
	//USART_writeln_string("Entering i2c_transmit_byte()");
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Wait until data is transmitted
	while(!(TWCR & (1 << TWINT)));
	//USART_writeln_string("Exiting i2c_transmit_byte()");
}

// recieves data from a slave device and acknowledges slave (ACK)
uint8_t i2c_receive_byte_ack()
{	
	TWCR |= (1 << TWINT) | (1 << TWEA);
	//wait until TW int is set
	while (!(TWCR & (1 << TWINT)));	
	return TWDR;
}
// recieves data from a slave device and doesn't acknowledge slave (NACK)
uint8_t i2c_receive_byte_nack()
{
	TWCR |= (1 << TWINT) & (~(1 << TWEA));
	//wait until TW int is set
	while (!(TWCR & (1 << TWINT)));	
	return TWDR;
}
//Stops controlling the TWI bus
void i2c_generate_stop()
{
	//USART_writeln_string("Entering i2c_generate_stop()");
	//Generate the STOP
	TWCR = (1<<TWINT) | (1 << TWEN) | (1<<TWSTO);
	//Wait until STOP is transmitted
	//while(!(TWCR & (1<<TWINT)));
	//USART_writeln_string("Exiting i2c_generate_stop()");
}

// transmits multiple bytes to a slave
bool i2c_transmit_packet(uint8_t addr, uint8_t* data, uint8_t len)
{
	//USART_writeln_string("Entering i2c_transmit_packet()");
	i2c_generate_start();
	// check if START is transmitted successfully
	if(i2c_confirm_status(MT_START))
	{
		//USART_writeln_string("START generated successfully!");
	}
	i2c_transmit_address(addr | WRITE_BIT);
	// check if slave address is identified by a slave
	if(i2c_confirm_status(MT_SLA_ACK))
	{
		//USART_writeln_string("SLA acknowledged by slave!");
	}
	// start transmitting number of bytes
	for(uint8_t i = 0; i < len; i++)
	{
		i2c_transmit_byte(*(data + i));
		// check if a slave has received the data byte
		if(i2c_confirm_status(MT_DATA_ACK))
		{
			//USART_writeln_string("data transmission success!");
		}
	}
	i2c_generate_stop();
}

// receives multiple bytes from a slave
bool i2c_receive_packet(uint8_t addr, uint8_t* data, uint8_t len)
{
	i2c_generate_start();
	// check if START is transmitted successfully
	if(!(i2c_confirm_status(MT_START)));
		return false;
	i2c_transmit_address(addr | READ_BIT);
	// check if slave address is identified by a slave
	if(!( i2c_confirm_status(MT_SLA_ACK) | i2c_confirm_status(MT_SLA_NACK) ))
		return false;
	for(uint8_t i = 0; i < len; i++)
	{
		data[i] = i2c_receive_byte_ack();
		// check if status code for data reception is available
		if(!( i2c_confirm_status(MR_DATA_ACK) | i2c_confirm_status(MR_DATA_NACK) ))
			return false;
	}
	i2c_generate_stop();
}