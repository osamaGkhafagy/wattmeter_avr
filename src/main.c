
#include <avr/io.h>
#include <util/delay.h>
#include "serial_328p.h"
#include <stdlib.h>
#include "lcd.h"
#include "ds1307.h"
#include "adc.h"
#include "timer1.h"
#include "zmpt101.h"
#include "acs712.h"
#include "eeprom.h"

#define SW1_DDR             DDRB
#define SW2_DDR             DDRD
#define SW1_LATCH           PINB
#define SW2_LATCH           PIND
#define SW1                 PB0         // PCINT0  (PCIF0)
#define SW2                 PD7         // PCINT23 (PCIF2)
#define MAX_VALUES          1000
#define ALARM_DDR           DDRB
#define ALARM_PORT          PORTB
#define ALARM_PIN           PB1
#define E_WH_L_ADDR         0X10
#define E_WH_H_ADDR         0X11
#define ACCU_T_STAMP_ADDR   0X20
#define SAVE_T_STAMP_ADDR   0X24
#define ACCU_PERIOD         1
#define SAVE_PERIOD         1
#define POWER_ROW           3
#define DATE_TIME_ROW       4
#define ACCU_PERIOD         1
#define SAVE_PERIOD         1

char* day_of_week[] = {"Saturday", "Sunday", "Monday", 
                      "Tuesday", "Wednesday", "Thursday", "Friday"};

char *month[] = {"January", "February", "March", "April", "May", "June",
                 "July", "August", "September", "October", "November", "December"};

void get_and_display_date_time();
uint32_t get_time_stamp();
void store_time_stamp(uint32_t t_stamp, uint16_t t_stamp_addr);
uint32_t load_time_stamp(uint16_t t_stamp_addr);
volatile uint16_t load_energy();
void store_energy(volatile const uint16_t E_wh);
void calculate_power();
void display_power();
double filter_spurious(double value);
float find_max(float[]);
void alarm_init();
void alarm_on(const uint16_t duration_ms, uint8_t beeps);

int main(void)
{
  // 0. initialize all needed variables
  bool flag = false;
  long double E_kwh = 0;
  // 1. Initialize all proper peripherals and modules
  //USART_init(9600, 8, 0);
  i2c_init(100000);
  i2c_lcd_init();
  eeprom_init();
  alarm_init();
  alarm_on(100, 1);
  // display the opening screen
  i2c_lcd_clear();
  i2c_lcd_write_str("GASCO CANAL AREA");
  i2c_lcd_go_to(2, 1);
  i2c_lcd_write_str(" Wattmeter V1.1 ");
  i2c_lcd_go_to(3, 1);
  i2c_lcd_write_str("  Checking all  ");
  i2c_lcd_go_to(4, 1);
  i2c_lcd_write_str(" components ....");
  flag = ds1307_init();
  /*
  while(!(flag = ds1307_is_ticking()))
  {
    ds1307_enable_oscillator();
    i2c_lcd_go_to(3, 1);
    i2c_lcd_write_str("RTC Syncing fail");
    i2c_lcd_go_to(4, 1);
    i2c_lcd_write_str("Check Oscillator");
    alarm_on(150, 10);
  }
  */
  _delay_ms(1000);
  // once evetything is configured, hit the alarm for 100ms
  alarm_on(100, 1);
  i2c_lcd_clear_row(3);
  i2c_lcd_clear_row(4);
  /*
  rtc time;
  time.year = 22;
  time.month = 8;
  time.date = 19;
  time.day = 19;
  time.hour = 5;
  time.min = 31;
  ds1307_set_full_data(time);
  */
  /*
  uint16_t zero = 0;
  store_energy(zero);
  */
  while(1)
  {
    
        get_and_display_date_time();
        calculate_power();
        display_power();
        _delay_ms(300);
  }
}

void get_and_display_date_time()
{
  char s_str[3], m_str[3], h_str[3];
  char date_str[3], month_str[3], year_str[5];
  rtc time;
  ds1307_get_full_data(&time);
  itoa(time.sec, s_str, 10);
  itoa(time.min, m_str, 10);
  itoa(time.hour, h_str, 10);
  itoa(time.date, date_str, 10);
  itoa(time.month, month_str, 10);
  itoa(time.year + 2000, year_str, 10);
  i2c_lcd_go_to(DATE_TIME_ROW, 1);
  // display date
  if(time.date < 10)
  {
      i2c_lcd_write_str("0");
      i2c_lcd_write_str(date_str);
  }
  else
      i2c_lcd_write_str(date_str);
  // display month
  i2c_lcd_write_str("/");
  if(time.month < 10)
  {
      i2c_lcd_write_str("0");
      i2c_lcd_write_str(month_str);
  }
  else
      i2c_lcd_write_str(month_str);
  // display year
  i2c_lcd_write_str("/");
  i2c_lcd_write_str(year_str);
  // display time
  i2c_lcd_write_str(" ");
  // display hour
  if(time.hour < 10)
  {
      i2c_lcd_write_str("0");
      i2c_lcd_write_str(h_str);
  }
  else
      i2c_lcd_write_str(h_str);
  // display minutes
  i2c_lcd_write_str(":");
  if(time.min < 10)
  {
      i2c_lcd_write_str("0");
      i2c_lcd_write_str(m_str);
  }
  else
      i2c_lcd_write_str(m_str);
    
}
// returns the time stamp as follows (year|month|day|hour|min)
uint32_t get_time_stamp()
{
    rtc time;
    uint32_t time_stamp = 0, year, month, date, hour, min;
    ds1307_get_full_data(&time);
    year = (uint32_t)time.year;
    month = (uint32_t)time.month;
    date = (uint32_t)time.date;
    hour = (uint32_t)time.hour;
    min = (uint32_t)time.min;
    time_stamp = (year << 20) | (month << 16) | (date << 11) | (hour << 6) | (min << 0);
    return time_stamp;
}
// stores uint32_t time_stamp variable into EEPROM
void store_time_stamp(uint32_t t_stamp, uint16_t t_stamp_addr)
{
    uint8_t temp = 0;
    // loop of four rounds for 4 bytes
    for(uint8_t i = 0; i < 4; i++)
    {
        temp = (uint8_t)((t_stamp >> i*8) & 0XFF);
        // fut-dev transform uint32_t into array 
        // and send it drectly to eeprm_write, it will handle it
        // instead of for-loop
        eeprom_write(&temp, (t_stamp_addr + i), 1);
    }
}
// loads 4 bytes from EEPROM into uint32_t time_stamp variable
uint32_t load_time_stamp(uint16_t t_stamp_addr)
{
    uint8_t temp = 0;
    uint32_t t_stamp = 0;
    // loop of four rounds for 4 bytes
    for(uint8_t i = 0; i < 4; i++)
    {
      eeprom_read(&temp, (t_stamp_addr  + i), 1);
      t_stamp = t_stamp | (((uint32_t)temp) << i*8);
        // fut-dev transform uint32_t into array 
        // and send it drectly to eeprm_write, it will handle it
        // instead of for-loop
    }
    return t_stamp;
}

volatile uint16_t load_energy()
{
    uint8_t temp = 0;
    volatile uint16_t E_wh = 0;
    eeprom_read(&temp, E_WH_L_ADDR, 1);
    E_wh = (temp & 0X00FF);
    // now the upper byte
    eeprom_read(&temp, E_WH_H_ADDR, 1);
    E_wh = E_wh | ((uint16_t)(temp << 8) & 0XFF00);
    return E_wh;
}

void store_energy(volatile const uint16_t E_wh)
{
    // splitting the 16-bit E_wh into two bytes to be stored
    // in (E_WH_ADDR) and (E_WH_ADDR + 1)
    // saving the lower byte
    uint8_t temp = 0;
    // splitting big_int into two bytes, upper and lower
    // the lower byte first
    temp = (uint8_t)(E_wh & 0X00FF);
    eeprom_write(&temp, E_WH_L_ADDR, 1);
    // now storing the upper byte
    temp = (uint8_t)((E_wh >> 8) & 0X00FF);
    eeprom_write(&temp, E_WH_H_ADDR, 1);
}

void calculate_power()
{
    // initialize some needed auto variables
    double i_ac = 0, v_ac, p_ac, e_wh;
    volatile uint16_t E_wh;
    static uint16_t E_wh_curr = 0;
    volatile uint32_t current_t_stamp, accu_t_stamp, save_t_stamp;
    char p_str[5];
    // recall saved E_wh value in order to accumulate on
    E_wh = load_energy();
    // sample v, i and filter spurious noise
    v_ac = zmpt101_get_ac_voltage();
    i_ac = acs712_get_ac_current();
    p_ac = v_ac * i_ac;
    // get the current time stamp
    current_t_stamp = get_time_stamp();
    accu_t_stamp = load_time_stamp(ACCU_T_STAMP_ADDR);
    save_t_stamp = load_time_stamp(SAVE_T_STAMP_ADDR);
    // now check whether 1 minute has passed to accumulate E_kwh
    if((current_t_stamp - accu_t_stamp) >= ACCU_PERIOD)
    {
        // update the last accu time stamp
        store_time_stamp(current_t_stamp, ACCU_T_STAMP_ADDR);
        // accumulate E_wh
        E_wh_curr += p_ac * (ACCU_PERIOD / 60.0);
        // now check whether 10 minutes have passed to store E_kwh
        if((current_t_stamp - save_t_stamp) >= SAVE_PERIOD)
        {
            // update the last save time stamp
            store_time_stamp(current_t_stamp, SAVE_T_STAMP_ADDR);
            // update the overall accumulated energy
            E_wh += E_wh_curr;
            // reset the accumulated energy
            E_wh_curr = 0;
            // store the new accumulated energy value
            store_energy(E_wh);
        }
    }
    
}
void display_power()
{
    // define some needed variables
    uint16_t E_wh = 0;
    char vac_str[4], iac_str[5], e_wh_str[6];
    // reacall the current E_wh
    E_wh = load_energy();
    // sample v, i for the purpose of displaying
    dtostrf((uint8_t)(zmpt101_get_ac_voltage()), 3, 0, vac_str);
    dtostrf((double)(acs712_get_ac_current()), 4, 1, iac_str);
    dtostrf((double)(E_wh), 3, 0, e_wh_str);
    i2c_lcd_go_to(POWER_ROW, 1);
    i2c_lcd_write_str(vac_str);
    i2c_lcd_write_str("V");
    i2c_lcd_write_str(" ");
    i2c_lcd_write_str(iac_str);
    i2c_lcd_write_str("A");
    i2c_lcd_write_str(" ");
    i2c_lcd_write_str(e_wh_str);
    i2c_lcd_write_str("Wh");
}

void alarm_init()
{
  // configure alarm pin
  ALARM_DDR |= (1 << ALARM_PIN);
  // alarm off initially
  ALARM_PORT &= ~(1 << ALARM_PIN);
}

void alarm_on(const uint16_t duration_ms, uint8_t beeps)
{
  for(uint8_t i = 0; i < beeps; i++)
  {
    ALARM_PORT |= (1 << ALARM_PIN);
    _delay_ms(100);
    ALARM_PORT &= ~(1 << ALARM_PIN);
  }
  
}