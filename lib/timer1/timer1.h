#ifndef __TIMER1_H__
#define __TIMER1_H__

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

void timer1_set_timing_period(uint16_t time_ms);
void timer1_start();
void timer1_stop();
bool timer1_is_time_elapsed();

#endif