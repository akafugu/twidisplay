/*
 * Akafugu TWI Display: Test
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
#include <avr/interrupt.h>
#include <util/delay.h>

#include <string.h>

#include "twi-display.h"

#define SLAVE_ADDR 0x12 // 7-seg

void main(void) __attribute__ ((noreturn));

void scroll_string(char* buf)
{
	clear(SLAVE_ADDR);
	
	for (uint16_t i = 0; i < strlen(buf); i++) {
		write_char(SLAVE_ADDR, buf[i]);
		_delay_ms(250);
	}
	_delay_ms(1000);
}

void test_dots(void)
{
	write_str(SLAVE_ADDR, "----");
	
	set_dots(SLAVE_ADDR, 1, 0, 0, 0);
	_delay_ms(500);
	set_dots(SLAVE_ADDR, 1, 1, 0, 0);
	_delay_ms(500);
	set_dots(SLAVE_ADDR, 1, 1, 1, 0);
	_delay_ms(500);
	set_dots(SLAVE_ADDR, 1, 1, 1, 1);
	_delay_ms(500);
	set_dots(SLAVE_ADDR, 0, 1, 1, 1);
	_delay_ms(500);
	set_dots(SLAVE_ADDR, 0, 0, 1, 1);
	_delay_ms(500);
	set_dots(SLAVE_ADDR, 0, 0, 0, 1);
	_delay_ms(500);
	set_dots(SLAVE_ADDR, 0, 0, 0, 0);
	_delay_ms(500);

	// test set dot
	clear(SLAVE_ADDR);
	set_dot(SLAVE_ADDR, 0, true);
	_delay_ms(500);
	set_dot(SLAVE_ADDR, 1, true);
	_delay_ms(500);
	set_dot(SLAVE_ADDR, 2, true);
	_delay_ms(500);
	set_dot(SLAVE_ADDR, 3, true);
	_delay_ms(500);
	set_dot(SLAVE_ADDR, 3, false);
	_delay_ms(500);
	set_dot(SLAVE_ADDR, 2, false);
	_delay_ms(500);
	set_dot(SLAVE_ADDR, 1, false);
	_delay_ms(500);
	set_dot(SLAVE_ADDR, 0, false);
	_delay_ms(500);
}

void test_temp(void)
{
	float t = 23.3;
	write_temperature(SLAVE_ADDR, (int)t, 'C');
	_delay_ms(1000);

	t = -12.3;
	write_temperature(SLAVE_ADDR, (int)t, 'C');
	_delay_ms(1000);
}

void test_time(void)
{
	static uint8_t hour = 0, min = 0, sec = 0;
	//rtc_get_time(&hour, &min, &sec);
	
	sec++;
	if (sec >= 60) { sec = 0; min++; }
	if (min >= 60) { min = 0; hour++; }
	if (hour >= 24) { hour = 0; min = 0; sec = 0; }
	
	write_time(SLAVE_ADDR, hour, min, sec);
}

void main(void)
{
	twi_init_master();
	sei();	

	set_brightness(SLAVE_ADDR, 255);
	clear(SLAVE_ADDR);

	// test set number
	for (uint16_t i = 0; i < 9999; i++) {
		write_int(SLAVE_ADDR, i);
		_delay_ms(1);
	}

	// test write char
	char buf[] = "this is a long string    ";
	set_rotate_mode(SLAVE_ADDR);
	scroll_string(buf);
	
	// run the same in scroll mode
	set_scroll_mode(SLAVE_ADDR);
	scroll_string(buf);

	// test write string
	write_str(SLAVE_ADDR, "abcd");
	_delay_ms(500);
	write_str(SLAVE_ADDR, "ABCD");
	_delay_ms(500);
	write_str(SLAVE_ADDR, buf); // will be cut off
	_delay_ms(500);
	
	// test set position
	set_rotate_mode(SLAVE_ADDR);
	clear(SLAVE_ADDR);
	write_char(SLAVE_ADDR, 'a');
	set_position(SLAVE_ADDR, 3);
	write_char(SLAVE_ADDR, 'b');
	_delay_ms(1000);

	// test set dots
	test_dots();

	test_temp();
	
	while(1) {
		test_time();
		_delay_ms(1000);
	}
}
