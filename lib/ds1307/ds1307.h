/*
    Device overview
    ---------------- 
    The DS1307 Serial Real-Time Clock is a low-power, full binary-coded decimal (BCD) clock/calendar
    plus 56 bytes of NV SRAM. Address and data are transferred serially via a 2-wire, bi-directional bus.
    The clock/calendar provides seconds, minutes, hours, day, date, month, and year information. The end of
    the month date is automatically adjusted for months with fewer than 31 days, including corrections for
    leap year. The clock operates in either the 24-hour or 12-hour format with AM/PM indicator. The
    DS1307 has a built-in power sense circuit that detects power failures and automatically switches to the
    battery supply. 

    Real-time clock (RTC) counts seconds, minutes, hours, date of the month, month, day of the week, 
    and year with leap-year compensation valid up to 2100
    56-byte, battery-backed, nonvolatile (NV) RAM for data storage Two-wire serial interface
    Programmable squarewave output signal Automatic power-fail detect and switch circuitry
    Consumes less than 500nA in battery backup

    Device driver discription
    --------------------------
    DS1307 SLA = 0X68
    Max SLC frequency = 100KHz (MUST NOT EXCEED)

*/

#ifndef __DS1307_H__
#define __DS1307_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"
#include "i2c_lcd.h"
#include "serial_328p.h"

#define DS1307_SLA          0X68            // 7-bit address
#define SECONDS_WORD_ADDR   0X00
#define MINUTES_WORD_ADDR   0X01
#define HOURS_WORD_ADDR     0X02
#define DAY_WORD_ADDR       0X03
#define DATE_WORD_ADDR      0X04
#define MONTH_WORD_ADDR     0X05
#define YEAR_WORD_ADDR      0X06
#define CTRL_WORD_ADDR      0X07
#define NVRAM_WORD_ADDR     0X08            // 56 Bytes
#define CH_BIT              (uint8_t)(7)
#define HOUR_MODE_24        (uint8_t)(6)

typedef struct 
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
}rtc;

bool ds1307_init();
void ds1307_enable_oscillator();
bool ds1307_is_ticking();
void ds1307_set_hours(uint8_t hour);
void ds1307_set_minutes(uint8_t min);
void ds1307_set_seconds(uint8_t sec);
void ds1307_set_day(uint8_t day);
void ds1307_set_date(uint8_t date);
void ds1307_set_month(uint8_t month);
void ds1307_set_year(uint8_t year);
void ds1307_set_full_data(rtc time);

uint8_t ds1307_get_hours();
uint8_t ds1307_get_minutes();
uint8_t ds1307_get_seconds();
uint8_t ds1307_get_day();
uint8_t ds1307_get_date();
uint8_t ds1307_get_month();
uint8_t ds1307_get_year();
void ds1307_get_full_data(rtc* time);

// helper functions
// converts binary byte to BCD byte
uint8_t binary_to_bcd(uint8_t data);
uint8_t bcd_to_binary(uint8_t data);




#endif