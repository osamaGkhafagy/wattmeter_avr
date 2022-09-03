#include "ds1307.h"

/* 
    initializes the DS1307 rtc and also checks on the functionality of the clock
    and whether it ticks or not
*/
bool ds1307_init()
{
    // first make sure a DS1307 is connected on the TWI bus
    i2c_generate_start();
    if(!(i2c_confirm_status(MT_START)))
        // TWI maybe uninitialized properly
        return false;
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    if(!(i2c_confirm_status(MT_SLA_ACK)))
        // DS1307 not found on the TWI bus
        return false;
    i2c_generate_stop();
    // disable clock halt(CH = 0)
    ds1307_enable_oscillator();
    // make sure the clock is ticking after the setup
    return ds1307_is_ticking();
}

void ds1307_enable_oscillator()
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(SECONDS_WORD_ADDR);
    i2c_transmit_byte(0);
    i2c_generate_stop();
}
// check that the clock is ticking (functionally working)
bool ds1307_is_ticking()
{
    uint8_t t0 = ds1307_get_seconds();
    _delay_ms(3000);
    uint8_t t1 = ds1307_get_seconds();
    if(t0 != t1)
        return true;
    else
        return false;
}

// sets seconds (00 to 59) and disable clock halt each time set
void ds1307_set_seconds(uint8_t sec)
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(SECONDS_WORD_ADDR);
    i2c_transmit_byte(binary_to_bcd(sec) & (~(1 << CH_BIT)));
    i2c_generate_stop();
}
// sets minutes (00 to 59)
void ds1307_set_minutes(uint8_t min)
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(MINUTES_WORD_ADDR);
    i2c_transmit_byte(binary_to_bcd(min));
    i2c_generate_stop();
}
// sets hour (00 to 23)
void ds1307_set_hours(uint8_t hour)
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(HOURS_WORD_ADDR);
    // set 24-Hour mode by setting the 6th bit to 0
    i2c_transmit_byte(binary_to_bcd(hour) & ~(1 << HOUR_MODE_24));
    i2c_generate_stop();
}
// sets day of week (01 to 07) = (Saturday to Friday)
void ds1307_set_day(uint8_t day)
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(DAY_WORD_ADDR);
    i2c_transmit_byte(binary_to_bcd(day));
    i2c_generate_stop();
}
// sets date (01 to 31)
void ds1307_set_date(uint8_t date)
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(DATE_WORD_ADDR);
    i2c_transmit_byte(binary_to_bcd(date));
    i2c_generate_stop();
}
// sets month (01 to 12) = (January to December)
void ds1307_set_month(uint8_t month)
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(MONTH_WORD_ADDR);
    i2c_transmit_byte(binary_to_bcd(month));
    i2c_generate_stop();
}
// sets year (00 to 99) = (2000 to 2099)
void ds1307_set_year(uint8_t year)
{
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(YEAR_WORD_ADDR);
    i2c_transmit_byte(binary_to_bcd(year));
    i2c_generate_stop();
}

void ds1307_set_full_data(rtc time)
{
    uint8_t data[7];
    data[0] = time.sec;
    data[1] = time.min;
    data[2] = time.hour;
    data[3] = time.day;
    data[4] = time.date;
    data[5] = time.month;
    data[6] = time.year;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(SECONDS_WORD_ADDR);
    for(uint8_t i = 0; i < 7; i++)
    {
        i2c_transmit_byte(binary_to_bcd(data[i]));
    }
    i2c_generate_stop();
}

uint8_t ds1307_get_hours()
{
    uint8_t hour = 0;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(HOURS_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    // ignoring the 24-Hour mode bit
    hour = bcd_to_binary(i2c_receive_byte_nack() & (~(1 << HOUR_MODE_24)));
    i2c_generate_stop();
    return hour;
}
uint8_t ds1307_get_minutes()
{
    uint8_t min = 0;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(MINUTES_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    min = bcd_to_binary(i2c_receive_byte_nack());
    i2c_generate_stop();
    return min;
}

uint8_t ds1307_get_seconds()
{
    uint8_t sec = 0;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(SECONDS_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    sec = bcd_to_binary(i2c_receive_byte_nack());
    i2c_generate_stop();
    return sec;
}
// returns the day of week as a number (1-7)
uint8_t ds1307_get_day()
{
    uint8_t day = 0;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(DAY_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    day = bcd_to_binary(i2c_receive_byte_nack());
    i2c_generate_stop();
    return day;
}
// returns the date (1-31)
uint8_t ds1307_get_date()
{
    uint8_t date = 0;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(DATE_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    date = bcd_to_binary(i2c_receive_byte_nack());
    i2c_generate_stop();
    return date;
}
// returns the month (1-12)
uint8_t ds1307_get_month()
{
    uint8_t month = 0;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(MONTH_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    month = bcd_to_binary(i2c_receive_byte_nack());
    i2c_generate_stop();
    return month;
}
// returns the year (2000-2099)
uint8_t ds1307_get_year()
{
    uint8_t year = 0;
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(YEAR_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    year = bcd_to_binary(i2c_receive_byte_nack());
    i2c_generate_stop();
    return year;
}

void ds1307_get_full_data(rtc* time)
{
    uint8_t data[7];
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | WRITE_BIT);
    i2c_transmit_byte(SECONDS_WORD_ADDR);
    i2c_generate_start();
    i2c_transmit_address((DS1307_SLA << 1) | READ_BIT);
    for(uint8_t i = 0; i < 6; i++)
    {
        data[i] = i2c_receive_byte_ack();
    }
    data[6] = i2c_receive_byte_nack();
    i2c_generate_stop();
    // doing some process to the BCD data
    time -> sec = bcd_to_binary(data[0]);
    time -> min = bcd_to_binary(data[1]);
    time -> hour = bcd_to_binary(data[2]);
    time -> day = bcd_to_binary(data[3]);
    time -> date = bcd_to_binary(data[4]);
    time -> month = bcd_to_binary(data[5]);
    time -> year = bcd_to_binary(data[6]);
}

// helper functions
// converts binary byte to BCD byte
uint8_t binary_to_bcd(uint8_t data)
{
    // dividing each nibble by 10 to get BCD nibbles
    // extracting the first nibble
    uint8_t  n1 = (uint8_t)(data % 10);
    // extracting the second nibble  
    uint8_t  n2 = (uint8_t)((data / 10) << 4);
    return (n1 | n2);
}
// converts BCD byte to binary byte
uint8_t bcd_to_binary(uint8_t data)
{
    // ORing the first nibble to the second nibble multiplied by 10
    // extracting the first nibble
    uint8_t  m1 = data & (0X0F);
    // extracting the second nibble
    uint8_t  m2 =(((data & (0XF0)) >> 4) * 10);
    return  (m1 + m2);
}