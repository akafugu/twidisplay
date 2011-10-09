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

#include <avr/io.h>
#include <string.h>

#include "twi.h"
#include "twi-display.h"

char temp_buf[4];

void set_number_ex(uint16_t num)
{
	temp_buf[3] = num % 10;
	num /= 10;
	temp_buf[2] = num % 10;
	num /= 10;
	temp_buf[1] = num % 10;
	num /= 10;
	temp_buf[0] = num % 10;
}

void change_address(uint8_t cur_addr, uint8_t new_addr)
{
	twi_begin_transmission(cur_addr);
	twi_send_byte(0x81); // change address
	twi_send_byte(new_addr);
	twi_end_transmission();
}


void show_address(uint8_t addr)
{
	twi_begin_transmission(addr);
	twi_send_byte(0x90); // show address
	twi_end_transmission();
}

void set_brightness(uint8_t addr, uint8_t brightness)
{
	twi_begin_transmission(addr);
	twi_send_byte(0x80); // set brightness
	twi_send_byte(brightness);
	twi_end_transmission();
}

void clear(uint8_t addr)
{
	twi_begin_transmission(addr);
	twi_send_byte(0x82); // clear
	twi_end_transmission();	
}

void set_rotate_mode(uint8_t addr)
{
	twi_begin_transmission(addr);
	twi_send_byte(0x83); //  set scroll mode
	twi_send_byte(0); // rotate mode
	twi_end_transmission();
}

void set_scroll_mode(uint8_t addr)
{
	twi_begin_transmission(addr);
	twi_send_byte(0x83); //  set scroll mode
	twi_send_byte(1); // scroll mode
	twi_end_transmission();
}

void set_segment_data(uint8_t addr, uint8_t position, int8_t segments)
{
	//fixme: implement
}

static uint8_t g_dots = 0;

void set_dot(uint8_t addr, uint8_t position, bool on)
{
	if (position > 3) return;
	
	if (on) g_dots |= (1<<(position+1));
	else g_dots &= ~(1<<(position+1));

	twi_begin_transmission(addr);
	twi_send_byte(0x85); //  set dots
	twi_send_byte(g_dots);
	twi_end_transmission();
}

void set_dots(uint8_t addr, bool dot0, bool dot1, bool dot2, bool dot3)
{
	g_dots = 0;
	if (dot0) g_dots |= 1<<1;
	if (dot1) g_dots |= 1<<2;
	if (dot2) g_dots |= 1<<3;
	if (dot3) g_dots |= 1<<4;
	
	twi_begin_transmission(addr);
	twi_send_byte(0x85); //  set dots
	twi_send_byte(g_dots);
	twi_end_transmission();
}

void set_position(uint8_t addr, uint8_t position)
{
	set_rotate_mode(addr);
	
	twi_begin_transmission(addr);
	twi_send_byte(0x89); // set position
	twi_send_byte(position);
	twi_end_transmission();
}

void write_int(uint8_t addr, int val)
{
	clear(addr);

	twi_begin_transmission(addr);
	
	set_number_ex(val);
	
	for (uint8_t i = 0; i <= 3; i++)
		twi_send_byte(temp_buf[i]);
	
	twi_end_transmission();
}

void write_int_sub(uint8_t addr, int val, uint8_t offset, uint8_t length)
{
	clear(addr);
	set_position(addr, offset);
	if (offset > 3) offset = 3;
	if (length > 4) length = 4;

	twi_begin_transmission(addr);
	
	set_number_ex(val);
	
	for (uint8_t i = 0; i <= length-1; i++)
		twi_send_byte(temp_buf[i+offset]);
	
	twi_end_transmission();
}

void write_char(uint8_t addr, char val)
{
	twi_begin_transmission(addr);
	twi_send_byte(val);
	twi_end_transmission();
}

void write_str(uint8_t addr, char* val)
{
	clear(addr);
	
	twi_begin_transmission(addr);
	
	for (uint8_t i = 0; i < strlen(val); i++) {
		twi_send_byte(val[i]);
		if (i == 3) break;
	}
	
	twi_end_transmission();
}

void write_temperature(uint8_t addr, int temp, char symbol)
{
	clear(addr);

	twi_begin_transmission(addr);
	
	if (temp >= 0) {
		set_number_ex(temp*100);
		for (int i = 0; i <= 2; i++)
			twi_send_byte(temp_buf[i]);
	}
	else {
		twi_send_byte('-');
		
		set_number_ex(-temp*100);
		for (int i = 0; i <= 1; i++)
			twi_send_byte(temp_buf[i]);
	}
	
	twi_send_byte(symbol);
		
	twi_send_byte(0x85); // set dots
	if (temp > 0) twi_send_byte(1<<2);
	else twi_send_byte(0);

	twi_end_transmission();
}

void write_time(uint8_t addr, uint8_t hour, uint8_t min, uint8_t sec)
{
	clear(addr);

	set_number_ex(hour*100 + min);
	
	twi_begin_transmission(addr);
	twi_send_byte(0x82); // clear
	
	// data
	for (int i = 0; i <= 3; i++) {
		twi_send_byte(temp_buf[i]);
	}
	
	twi_end_transmission();
	
	// second dot on/off
	twi_begin_transmission(addr);
	
	if (sec % 2 == 0) {
		twi_send_byte(0x85); // set dots
		twi_send_byte(0);
	}
	else {
		twi_send_byte(0x85); // set dots
		twi_send_byte(1<<2);
	}

	twi_end_transmission();
}

void write_segments(uint8_t addr, uint8_t segments)
{
	twi_begin_transmission(addr);
	twi_send_byte(0x84); // receive segment data
	twi_send_byte(segments);
	twi_end_transmission();
}

int get_firmware_revision(uint8_t addr)
{
  twi_begin_transmission(addr);
  twi_send_byte(0x8a); // get firmware revision
  twi_end_transmission();

  twi_request_from(addr, 1);
  return twi_receive();
}

int get_digits(uint8_t addr)
{
  twi_begin_transmission(addr);
  twi_send_byte(0x8b); // get number of digits
  twi_end_transmission();

  twi_request_from(addr, 1);
  return twi_receive();
}
