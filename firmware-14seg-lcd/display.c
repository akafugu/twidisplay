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

#include <avr/io.h> 
#include <avr/interrupt.h>
#include "display.h"

#include "ht1621.h"

uint16_t calculate_segments(uint8_t character); // in font-14seg-lcd.c

void update_ht1621(void);
void write_char_at(uint8_t index, uint16_t segments);

// To support both ATTiny2313 and ATMegaXX8
#ifndef TIMSK0
#define TIMSK0 TIMSK
#endif

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

char data[8]; // Digit data
uint16_t segment_data[8]; // Segment data

// dots and apostrophes[bits 0~7]
uint16_t dots = 0;
uint16_t apostrophes = 0;

void display(uint8_t pos, uint8_t digit);
void clearDisplay(void);

void display_init(void)
{
	ht1621_init();
	ht1621_send_cmd(BIAS_THIRD_4_COM);
	//ht1621_send_cmd(BIAS_HALF_4_COM);

	// clear screen
	clear_screen();

	sei();	// Enable interrupts
}

void shift_in(char c)
{
	data[0] = data[1];
	data[1] = data[2];
	data[2] = data[3];
	data[3] = data[4];
	data[4] = data[5];
	data[5] = data[6];
	data[6] = data[7];
	data[7] = c;

	segment_data[0] = segment_data[1];
	segment_data[1] = segment_data[2];
	segment_data[2] = segment_data[3];
	segment_data[3] = segment_data[4];
	segment_data[4] = segment_data[5];
	segment_data[5] = segment_data[6];
	segment_data[6] = segment_data[7];
	segment_data[7] = 0;

	update_ht1621();
}

void set_char_at(char c, uint8_t offset)
{
	data[offset] = c;
	write_char_at(offset, calculate_segments(c));
}

uint16_t process_segments(uint8_t in)
{
	uint16_t out = 0;

	if (bit_is_set(in, 0))
		sbi(out, 0);
	if (bit_is_set(in, 1))
		sbi(out, 1);
	if (bit_is_set(in, 2))
		sbi(out, 2);
	if (bit_is_set(in, 3))
		sbi(out, 7);
	if (bit_is_set(in, 4))
		sbi(out, 6);
	if (bit_is_set(in, 5))
		sbi(out, 5);
	if (bit_is_set(in, 6)) {
		sbi(out, 9);
		sbi(out, 14);
	}
	if (bit_is_set(in, 7))
		sbi(out, 3);

	return out;
}

void set_segments_at(uint8_t segments, uint8_t offset)
{
	segment_data[offset] = process_segments(segments);
	data[offset] = 0b10000000; // set upper bit to indicate that segment data should be used for display
	write_char_at(offset, process_segments(segments));
}

void shift_in_segments(uint8_t segments)
{
	data[0] = data[1];
	data[1] = data[2];
	data[2] = data[3];
	data[3] = data[4];
	data[4] = data[5];
	data[5] = data[6];
	data[6] = data[7];
	data[7] = 0b10000000; // set upper bit to indicate that segment data should be used for display

	segment_data[0] = segment_data[1];
	segment_data[1] = segment_data[2];
	segment_data[2] = segment_data[3];
	segment_data[3] = segment_data[4];
	segment_data[4] = segment_data[5];
	segment_data[5] = segment_data[6];
	segment_data[6] = segment_data[7];
	segment_data[7] = process_segments(segments);

	update_ht1621();
}

uint16_t process_segments_16(uint8_t low, uint8_t high)
{
	uint8_t out_l = 0;
	uint8_t out_h = 0;

	uint16_t out = 0;

	if (bit_is_set(low, 0)) // A
		sbi(out_l, 0);
	if (bit_is_set(low, 1)) // B
		sbi(out_l, 1);
	if (bit_is_set(low, 2)) // C
		sbi(out_l, 2);
	if (bit_is_set(low, 3)) // D
		sbi(out_l, 7);
	if (bit_is_set(low, 4)) // E
		sbi(out_l, 6);
	if (bit_is_set(low, 5)) // F
		sbi(out_l, 5);
	if (bit_is_set(low, 6)) // G
		sbi(out_h, 1);
	if (bit_is_set(low, 7)) // H
		sbi(out_h, 0);

	if (bit_is_set(high, 0)) // I
		sbi(out_h, 4);
	if (bit_is_set(high, 1)) // J
		sbi(out_h, 5);
	if (bit_is_set(high, 2)) // K
		sbi(out_h, 6);
	if (bit_is_set(high, 3)) // L
		sbi(out_h, 7);
	if (bit_is_set(high, 4)) // M
		sbi(out_h, 3);
	if (bit_is_set(high, 5)) // N
		sbi(out_h, 2);

	out = (out_h << 8) + out_l;
	return out;
}

void set_segments_at_16(uint8_t low, uint8_t high, uint8_t offset)
{
	segment_data[offset] = process_segments_16(low, high);
	data[offset] = 0b10000000; // set upper bit to indicate that segment data should be used for display
	write_char_at(offset, process_segments_16(low, high));
}

void shift_in_segments_16(uint8_t low, uint8_t high)
{
	data[0] = data[1];
	data[1] = data[2];
	data[2] = data[3];
	data[3] = data[4];
	data[4] = data[5];
	data[5] = data[6];
	data[6] = data[7];
	data[7] = 0b10000000; // set upper bit to indicate that segment data should be used for display

	segment_data[0] = segment_data[1];
	segment_data[1] = segment_data[2];
	segment_data[2] = segment_data[3];
	segment_data[3] = segment_data[4];
	segment_data[4] = segment_data[5];
	segment_data[5] = segment_data[6];
	segment_data[6] = segment_data[7];
	segment_data[7] = process_segments_16(low, high);

	update_ht1621();
}


// show number on screen
void set_number(uint16_t num)
{
	data[7] = num % 10;
	num /= 10;
	data[6] = num % 10;
	num /= 10;
	data[5] = num % 10;
	num /= 10;
	data[4] = num % 10;
	num /= 10;
	data[3] = num % 10;
	num /= 10;
	data[2] = num % 10;
	num /= 10;
	data[1] = num % 10;
	num /= 10;
	data[0] = num % 10;
	dots = 0;

	update_ht1621();
}

void clear_screen(void)
{
	data[0] = ' '; data[1] = ' '; data[2] = ' '; data[3] = ' ';
	data[4] = ' '; data[5] = ' '; data[6] = ' '; data[7] = ' ';

	update_ht1621();
}

void set_dots(uint8_t d)
{
	if (dots == d) return;
	dots = d;
	update_ht1621();
}

void set_apostrophes(uint8_t ap)
{
	if (apostrophes == ap) return;
	apostrophes = ap;
	update_ht1621();
}

#ifdef FEATURE_SET_TIME
void set_time(uint8_t hour, uint8_t min, uint8_t sec)
{
	if (sec % 2) sbi(dots, 2); // set dot 1 on
	else cbi(dots, 2); // set dot 1 off

	set_number(hour * 100 + min);
}
#endif // FEATURE_SET_TIME

// 8 characters
// each character uses 4 segment lines
// 4 commons
// 
// index * 4 = starting segment
// offset 0 = seg A,B,C,DP
// offset 1 = D,E,F,CA
// offset 2 = H,G,N,M
// offset 3 = L,K,J,I
void write_char_at(uint8_t index, uint16_t segments)
{
   int start = index * 4;

   // dot/apostrophe 7 is encoded in bit 0
   if (index == 7 && bit_is_set(dots, 0))
	   sbi(segments, 3);

   if (index == 7 && bit_is_set(apostrophes, 0))
	   sbi(segments, 4);

   if (bit_is_set(dots, index+1))
	   sbi(segments, 3);

   if (bit_is_set(apostrophes, index+1))
	   sbi(segments, 4);

   ht1621_write(start, segments & 0b1111);
   ht1621_write(start+1, (segments >> 4) & 0b1111);
   ht1621_write(start+2, (segments >> 8) & 0b1111);
   ht1621_write(start+3, (segments >> 12) & 0b1111);
}

void update_ht1621(void)
{
	for (uint8_t i = 0; i < 8; i++) {
		if (data[i] & 0b10000000)
		    write_char_at(i, segment_data[i]);
		else
			write_char_at(i, calculate_segments(data[i]));
	}
}

// 0=off, 1=2KHz, 2=4kHz
void set_tone(uint8_t level)
{
	if (level == 0) {
		ht1621_send_cmd(TONE_OFF);
	}
	else if (level == 1) {
		ht1621_send_cmd(TONE_2K);
		ht1621_send_cmd(TONE_ON);
	}
	else if (level == 2) {
		ht1621_send_cmd(TONE_4K);
		ht1621_send_cmd(TONE_ON);
	}
}

// 3 = 1/3rd bias(default) 2 = 1/2 bias (darker)
void set_bias(uint8_t bias)
{
	if (bias == 3)
		ht1621_send_cmd(BIAS_THIRD_4_COM);
	else if (bias == 2)
		ht1621_send_cmd(BIAS_HALF_4_COM);
}
