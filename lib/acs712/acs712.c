#include "acs712.h"

double acs712_sens = 0.066;
double acs712_offset = 0;

volatile double acs712_get_ac_current()
{
    volatile float Vrms_adc, Irms_ac, dc_component = 0;
    uint16_t dig_reading = 0;
    double analog_value = 0, acc = 0;
    uint32_t measurements_count = 0;
    uint32_t period_ms = 200;
    // initialize adc
    adc_init(ACS712_PIN, 32);
    // configure analog pin as input (maybe not necessary)
    ACS712_DDR &= ~(1 << ACS712_PIN);
    //calculating the dc component ~2.5v
    timer1_set_timing_period(period_ms);
    timer1_start();
    while (!timer1_is_time_elapsed())
    {
        acc += adc_reading(ACS712_PIN, 10) * (5.0 / 1024.0);
        measurements_count++;
    }
    dc_component = (double)(acc / measurements_count);
    acc = 0;
    measurements_count = 0;
    timer1_set_timing_period(period_ms);
    timer1_start();
    
    while (!timer1_is_time_elapsed())
    {
        // take a sample
        dig_reading = adc_reading(ACS712_PIN, 10);
        analog_value = (dig_reading * 5.0 / 1024.0) - 2.5;
        acc += analog_value * analog_value;
        measurements_count++;
    }
    adc_disable();
    // take mean and square root
    Vrms_adc = sqrt(acc / measurements_count);
    Irms_ac =  ((Vrms_adc - acs712_offset) * (1 / acs712_sens));
    /*
    char c[10];
    char v[8];
    char x[6];
    
    i2c_lcd_clear();
    dtostrf(Vrms_adc, 5, 3, v);
    itoa(measurements_count, c, 10);
    dtostrf(acc, 5, 3, x);
    i2c_lcd_write_str("sin_read = ");
    i2c_lcd_write_str(x);
    i2c_lcd_go_to(3, 1);
    i2c_lcd_write_str("Vrms_adc = ");
    i2c_lcd_write_str(v);
    _delay_ms(500);
    */
    return Irms_ac;
}


uint16_t acs712_calibrate() 
{
    adc_init(ACS712_PIN, 10);
    uint16_t acc = 0;
    for (int i = 0; i < 10; i++) 
    {
        acc += adc_reading(ACS712_PIN, 10);
    }
    return acc / 10;
}