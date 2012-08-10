/*
 * TWIDisplay 8-digit 14-segment LCD display
 * (C) 2012 Akafugu Corporation
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>

// data mode: determines how c parameter to shift_in and set_char_at is interpreted
#define ASCII 0    // interpreted as ascii
#define SEGMENTS 1 // interpreted as segments to turn on/off

void display_init(void);
uint8_t set_brightness(uint8_t b);
void shift_in(char c);
void set_char_at(char c, uint8_t offset);
void set_segments_at(uint8_t segments, uint8_t offset);
void shift_in_segments(uint8_t segments);
void set_segments_at_16(uint8_t low, uint8_t high, uint8_t offset);
void shift_in_segments_16(uint8_t low, uint8_t high);
void set_number(uint16_t num);
void clear_screen(void);
void set_dots(uint8_t d);
void set_apostrophes(uint8_t ap);
#ifdef FEATURE_SET_TIME
void set_time(uint8_t hour, uint8_t min, uint8_t sec);
#endif // FEATURE_SET_TIME

void set_tone(uint8_t level);
void set_bias(uint8_t bias);

#endif // DISPLAY_H_
