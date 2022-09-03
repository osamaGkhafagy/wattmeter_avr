#include "adc.h"

void adc_init(uint8_t bit_mode, uint8_t div_factor)
{
	//Enabling ADC
	ADCSRA |= (1 << ADEN);
	
	//Selecting the bit mode of the ADC
	switch(bit_mode)
	{
		case 8: ADMUX |= (1 << ADLAR);
		break;
		case 10: ADMUX &= ~(1 << ADLAR);
		break;
	}
	//Division factor configuration
	switch(div_factor)
	{
		case 2: ADCSRA &= ~(1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
		break;
		case 4: ADCSRA &= ~(1 << ADPS0) | (1 << ADPS2); ADCSRA |= ( 1<< ADPS1);
		break;
		case 8: ADCSRA &= ~(1 << ADPS2); ADCSRA |= (1 << ADPS0) | (1 << ADPS1);
		break;
		case 16: ADCSRA &= ~(1 << ADPS0) | (1 << ADPS1); ADCSRA |= (1 << ADPS2);
		break;
		case 32: ADCSRA &= ~(1 << ADPS1); ADCSRA |= (1 << ADPS0) | (1 << ADPS2);
		break;
		case 64: ADCSRA &= ~(1 << ADPS0); ADCSRA |= (1 << ADPS1) | (1 << ADPS2);
		break;
		case 128: ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
		break;	
    }
}

volatile uint16_t adc_reading(uint8_t channel,uint8_t bit_mode)
{
	//Channel configuration
	switch(channel)
	{
		case 0:ADMUX &=~(1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3);
		break;
		case 1:ADMUX |=(1<<MUX0);ADMUX &=~(1<<MUX1)|(1<<MUX2)|(1<<MUX3);
		break;
		case 2:ADMUX |=(1<<MUX1);ADMUX &=~(1<<MUX0)|(1<<MUX2)|(1<<MUX3);
		break;
		case 3:ADMUX |=(1<<MUX0)|(1<<MUX1);ADMUX &=~(1<<MUX2)|(1<<MUX3);
		break;
		case 4:ADMUX |=(1<<MUX2);ADMUX &=~(1<<MUX0)|(1<<MUX1)|(1<<MUX3);
		break;
		case 5:ADMUX |=(1<<MUX0)|(1<<MUX2);ADMUX &=~(1<<MUX1)|(1<<MUX3);
		break;
		case 6:ADMUX |=(1<<MUX1)|(1<<MUX2);ADMUX &=~(1<<MUX0)|(1<<MUX3);
		break;
		case 7:ADMUX |=(1<<MUX0)|(1<<MUX1)|(1<<MUX2);ADMUX &=~(1<<MUX3);
		break;
		case 8:ADMUX |=(1<<MUX3);ADMUX &=~(1<<MUX0)|(1<<MUX1)|(1<<MUX2);
		break;
		case 9:ADMUX |=(1<<MUX0)|(1<<MUX3);ADMUX &=~(1<<MUX1)|(1<<MUX2);
		break;
	}
	// Start conversion
	ADCSRA |= (1 << ADSC);
    // Wait for the conversion to be completed
	while ( ADCSRA & (1 << ADSC) );
	
	//Bit Mode Configuration
	if(bit_mode == 10)
	    return (ADCL | (ADCH << 8));
	else 
        return (ADCH << 2);
}

void adc_disable()
{
	ADCSRA &= ~(1 << ADEN);				//Disables the ADC
}