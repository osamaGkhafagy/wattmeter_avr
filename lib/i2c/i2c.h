#ifndef __I2C_H__
#define __I2C_H__

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <serial_328p.h>
#define F_CPU				8000000UL
#define ERROR				0X00
#define MT_START			0X08
#define	MT_REPEATED_START	0X10
#define MT_SLA_ACK			0X18
#define MT_SLA_NACK			0X20
#define MR_SLA_ACK			0X40
#define MR_SLA_NACK			0X48
#define MT_DATA_ACK			0X28
#define MT_DATA_NACK		0X30
#define MR_DATA_ACK			0X50
#define MR_DATA_NACK		0X58
#define	NO_EVENT			0XF8
#define WRITE_BIT			0X00
#define READ_BIT			0X01

void i2c_init(uint32_t speed);
void i2c_generate_start();
bool i2c_confirm_status(uint8_t status_code);
void i2c_transmit_address(uint8_t address);
void i2c_transmit_byte(uint8_t data);
uint8_t i2c_receive_byte_ack();
uint8_t i2c_receive_byte_nack();
void i2c_generate_stop();
//BIG subroutines
bool i2c_transmit_packet(uint8_t addr, uint8_t* data, uint8_t len);
bool i2c_receive_packet(uint8_t addr, uint8_t* data, uint8_t len);




#endif