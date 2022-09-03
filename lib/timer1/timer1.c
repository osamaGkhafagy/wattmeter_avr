#include "timer1.h"

void timer1_set_timing_period(uint16_t time_ms)
{
  // set Timer 1 on CTC mode 
  TCCR1B |= (1 << WGM12);
  // Assuming Fclk = 8MHz
  // Load the timer with a value corresponding to the given time_ms
  // the max time is = 32us * 2^16 = 2097ms
  uint16_t t_max = (uint16_t)((32 * 65536) / 1000.0);
  uint16_t t_load = time_ms * 32;
  OCR1AH = (uint8_t)((t_load & 0XFF00 ) >> 8);
  OCR1AL = (uint8_t)(t_load & 0XFF);
}

void timer1_start()
{
  // configure timer prescaler to 256 so that
  // every 31.25 clock pulse inputted to the timer
  // is 100ms long
  TCCR1B |= (1 << CS12);
}

void timer1_stop()
{
  TCCR1B &= ~(1 << CS12);
  TIFR1 |= (1 << OCF1A);
}

bool timer1_is_time_elapsed()
{
  if(TIFR1 & (1 << OCF1A))
  {
    timer1_stop();
    return true;
  }  
  else
    return false;
}