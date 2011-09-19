/*
 * Akafugu TWI Display Driver
 * (C) 2011 Akafugu Corporation
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

#ifndef TWI_DISPLAY_H
#define TWI_DISPLAY_H

#include <stdbool.h>
#include "twi.h"

void change_address(uint8_t cur_addr, uint8_t new_addr);
void show_address(uint8_t addr);
void set_brightness(uint8_t addr, uint8_t brightness);
void clear(uint8_t addr);

void set_rotate_mode(uint8_t addr);
void set_scroll_mode(uint8_t addr);

void set_dot(uint8_t addr, uint8_t position, bool on);
void set_dots(uint8_t addr, bool dot0, bool dot1, bool dot2, bool dot3);
void set_position(uint8_t addr, uint8_t position);

void write_int(uint8_t addr, int val);
void write_int_sub(uint8_t addr, int val, uint8_t offset, uint8_t length);
void write_char(uint8_t addr, char val);
void write_str(uint8_t addr, char* val);
void write_temperature(uint8_t addr, int temp, char symbol);
void write_time(uint8_t addr, uint8_t hour, uint8_t min, uint8_t sec);
void write_segments(uint8_t addr, uint8_t segments);

#endif // TWI_DISPLAY_H
