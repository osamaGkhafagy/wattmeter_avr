#include "zmpt101.h"

double zmpt101_sens = 0.001318;
double zmpt101_offset = 0.00856;

volatile double zmpt101_get_ac_voltage()
{
    volatile float Vrms_adc, Vrms_ac, dc_component = 0;
    uint16_t dig_reading = 0;
    double analog_value = 0, acc = 0;
    uint32_t measurements_count = 0;
    uint32_t period_ms = 200;
    // initialize adc
    adc_init(ZMPT101_PIN, 32);
    // configure analog pin as input (maybe not necessary)
    ZMPT101_DDR &= ~(1 << ZMPT101_PIN);
    //calculating the dc component ~2.5v
    timer1_set_timing_period(period_ms);
    timer1_start();
    while (!timer1_is_time_elapsed())
    {
        acc += adc_reading(ZMPT101_PIN, 10) * (5.0 / 1024.0);
        measurements_count++;
    }
    dc_component = (double)acc / measurements_count;
    acc = 0;
    measurements_count = 0;
    timer1_set_timing_period(period_ms);
    timer1_start();
    
    while (!timer1_is_time_elapsed())
    {
        // take a sample
        dig_reading = adc_reading(ZMPT101_PIN, 10);
        analog_value = (dig_reading * 5.0 / 1024.0) - dc_component;
        acc += analog_value * analog_value;
        measurements_count++;
    }
    adc_disable();
    // take mean and square root
    Vrms_adc = sqrt(acc / measurements_count);
    Vrms_ac =  ((Vrms_adc - zmpt101_offset) * (1 / zmpt101_sens));
    Vrms_ac = (Vrms_ac > 100) ? Vrms_ac : 0;
    /*
    char c[6];
    char v[8];
    char v_ac[5];
    
    i2c_lcd_clear();
    dtostrf(Vrms_adc, 5, 3, v);
    dtostrf(Vrms_ac, 5, 1, v_ac);
    itoa(measurements_count, c, 10);
    dtostrf(acc,5, 3, x);
    i2c_lcd_write_str("count = ");
    i2c_lcd_write_str(c);
    i2c_lcd_go_to(2, 1);
    i2c_lcd_write_str("sin_read = ");
    i2c_lcd_write_str(x);
    i2c_lcd_go_to(3, 1);
    i2c_lcd_write_str("Vrms_adc = ");
    i2c_lcd_write_str(v);
    i2c_lcd_go_to(4, 1);
    i2c_lcd_write_str("Vrms_ac = ");
    i2c_lcd_write_str(v_ac);
    _delay_ms(500);
    */

    return Vrms_ac;
}


uint16_t zmpt101_calibrate() 
{
    adc_init(ZMPT101_PIN, 10);
    uint16_t acc = 0;
    for (int i = 0; i < 10; i++) 
    {
        acc += adc_reading(ZMPT101_PIN, 10);
    }
    return acc / 10;
}