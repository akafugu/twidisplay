/*
 * TWI 4-digit 7-segment display
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
#include "display.h"

#ifdef COMMON_CATHODE
// common cathode
#define SET_DIGIT cbi
#define CLEAR_DIGIT sbi
#define SET_SEGMENT sbi
#define CLEAR_SEGMENT cbi
#else
// common anode
#define SET_DIGIT sbi
#define CLEAR_DIGIT cbi
#define SET_SEGMENT cbi
#define CLEAR_SEGMENT sbi
#endif

// To support both ATTiny2313 and ATMegaXX8
#ifndef TIMSK0
#define TIMSK0 TIMSK
#endif

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

char data[4]; // Digit data
char segment_data[4]; // Segment data

// dots [bit 0~3]
uint8_t dots = 0;

uint16_t brightness = 80;	// Read from EEPROM on startup

void display(uint8_t pos, uint8_t digit);
void clearDisplay(void);

void display_init(void)
{
	// set digits as output
	sbi(DIGIT1_DDR, DIGIT1_BIT);
	sbi(DIGIT2_DDR, DIGIT2_BIT);
	sbi(DIGIT3_DDR, DIGIT3_BIT);
	sbi(DIGIT4_DDR, DIGIT4_BIT);
	
	// set segments as output
	sbi(A_DDR,  A_BIT);
	sbi(B_DDR,  B_BIT);
	sbi(C_DDR,  C_BIT);
	sbi(D_DDR,  D_BIT);
	sbi(E_DDR,  E_BIT);
	sbi(F_DDR,  F_BIT);
	sbi(G_DDR,  G_BIT);
	sbi(DP_DDR, DP_BIT);

	// clear screen
	clear_screen();

	sei();	// Enable interrupts

	// Inititalize timer for multiplexing
	TCCR0B = (1<<CS01); // Set Prescaler to clk/8 : 1 click = 1us. CS01=1 
	TIMSK0 |= (1<<TOIE0); // Enable Overflow Interrupt Enable
	TCNT0 = 0; // Initialize counter
}

uint8_t set_brightness(uint8_t b)
{
	if (b > 100) b = 100;
	b = 100 - b;

	brightness = b + 5;
	return b;
}

void shift_in(char c)
{
	data[0] = data[1];
	data[1] = data[2];
	data[2] = data[3];
	data[3] = c;

	segment_data[0] = segment_data[1];
	segment_data[1] = segment_data[2];
	segment_data[2] = segment_data[3];
	segment_data[3] = 0;
}

void set_char_at(char c, uint8_t offset)
{
	data[offset] = c;
}

void set_segments_at(uint8_t segments, uint8_t offset)
{
	segment_data[offset] = segments;
	data[offset] = 0b10000000; // set upper bit to indicate that segment data should be used for display
}

void shift_in_segments(uint8_t segments)
{
	data[0] = data[1];
	data[1] = data[2];
	data[2] = data[3];
	data[3] = 0b10000000; // set upper bit to indicate that segment data should be used for display

	segment_data[0] = segment_data[1];
	segment_data[1] = segment_data[2];
	segment_data[2] = segment_data[3];
	segment_data[3] = segments;
}

// show number on screen
void set_number(uint16_t num)
{
	data[3] = num % 10;
	num /= 10;
	data[2] = num % 10;
	num /= 10;
	data[1] = num % 10;
	num /= 10;
	data[0] = num % 10;
	dots = 0;
}

void clear_screen(void)
{
	data[0] = ' '; data[1] = ' '; data[2] = ' '; data[3] = ' ';
}

#ifdef FEATURE_SET_TIME
void set_time(uint8_t hour, uint8_t min, uint8_t sec)
{
	set_number(hour * 100 + min);
	if (sec % 2) sbi(dots, 2); // set dot 1 on
	else cbi(dots, 2); // set dot 1 off
}
#endif // FEATURE_SET_TIME

uint8_t multiplex_counter = 0;

// display multiplexing routine: run once every 5us
void display_multiplex(void)
{
	if (multiplex_counter == 0)
		display(0, 1);
	else if (multiplex_counter == 1)
		display(1, 2);
	else if (multiplex_counter == 2)
		display(2, 3);
	else if (multiplex_counter == 3)
		display(3, 4);
	else if (multiplex_counter == 4)
		clearDisplay();

	multiplex_counter++;

	// brightness setting: 5~100
	if (multiplex_counter == brightness) multiplex_counter = 0;
}

// run once every 1 us
ISR(TIMER0_OVF_vect)
{
	display_multiplex();	
	TCNT0 = 0xFF - 10; // Overflow again after 10 ticks = 10 us
}

uint8_t calculate_segments(uint8_t character)
{
	uint8_t segments = 0;

	switch (character)
	{
		case 0:
		case '0':
		case 'O':
			segments = (1<<A)|(1<<B)|(1<<C)|(1<<D)|(1<<E)|(1<<F);
			break;
		case 1:
		case '1':
		case 'l':
			segments = (1<<B)|(1<<C);
			break;
		case 2:
		case '2':
			segments = (1<<A)|(1<<B)|(1<<D)|(1<<E)|(1<<G);
			break;
		case 3:
		case '3':
			segments = (1<<A)|(1<<B)|(1<<C)|(1<<D)|(1<<G);
			break;
		case 4:
		case '4':
			segments = (1<<B)|(1<<C)|(1<<F)|(1<<G);
			break;
		case 5:
		case '5':
		case 'S':
		case 's':
			segments = (1<<A)|(1<<C)|(1<<D)|(1<<F)|(1<<G);
			break;
		case 6:
		case '6':
			segments = (1<<A)|(1<<C)|(1<<D)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 7:
		case '7':
			segments = (1<<A)|(1<<B)|(1<<C);
			break;
		case 8:
		case '8':
			segments = (1<<A)|(1<<B)|(1<<C)|(1<<D)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 9:
		case '9':
		case 'g':
			segments = (1<<A)|(1<<B)|(1<<C)|(1<<D)|(1<<F)|(1<<G);
			break;
		case 10:
		case 'A':
		case 'a':
			segments = (1<<A)|(1<<B)|(1<<C)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 11:
		case 'B':
		case 'b':
			segments = (1<<C)|(1<<D)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 12:
		case 'C':
			segments = (1<<A)|(1<<D)|(1<<E)|(1<<F);
			break;
		case 'c':
			segments = (1<<D)|(1<<E)|(1<<G);
			break;
		case 13:
		case 'D':
		case 'd':
			segments = (1<<B)|(1<<C)|(1<<D)|(1<<E)|(1<<G);
			break;
		case 14:
		case 'E':
			segments = (1<<A)|(1<<D)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 'e':
			segments = (1<<A)|(1<<B)|(1<<D)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 15:
		case 'F':
		case 'f':
			segments = (1<<A)|(1<<E)|(1<<F)|(1<<G);
			break;
#ifdef FEATURE_ALPHABET
		case 'G':
			segments = (1<<A)|(1<<C)|(1<<D)|(1<<E)|(1<<F);
			break;
		case 'H':
			segments = (1<<B)|(1<<C)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 'h':
			segments = (1<<C)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 'I':
		case 'i':
			segments = (1<<B)|(1<<C);
			break;
		case 'J':
		case 'j':
			segments = (1<<B)|(1<<C)|(1<<D)|(1<<E);
			break;
		case 'L':
			segments = (1<<D)|(1<<E)|(1<<F);
			break;
		case 'M':
		case 'm':
			segments = (1<<A)|(1<<C)|(1<<E)|(1<<G);
			break;
		case 'N':
		case 'n':
			segments = (1<<C)|(1<<E)|(1<<G);
			break;
		case 'o':
			segments = (1<<C)|(1<<D)|(1<<E)|(1<<G);
			break;
		case 'P':
		case 'p':
			segments = (1<<A)|(1<<B)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 'Q':
		case 'q':
			segments = (1<<A)|(1<<B)|(1<<C)|(1<<F)|(1<<G);
			break;
		case 'R':
		case 'r':
			segments = (1<<E)|(1<<G);
			break;
		case 'T':
		case 't':
			segments = (1<<D)|(1<<E)|(1<<F)|(1<<G);
			break;
		case 'U':
			segments = (1<<B)|(1<<C)|(1<<D)|(1<<E)|(1<<F);
			break;
		case 'u':
			segments = (1<<C)|(1<<D)|(1<<E);
			break;
		case 'V':
		case 'v':
			segments = (1<<C)|(1<<D)|(1<<E);
			break;
		case 'W':
		case 'w':
			segments = (1<<A)|(1<<C)|(1<<D)|(1<<E);
			break;
		case 'Y':
		case 'y':
			segments = (1<<B)|(1<<C)|(1<<D)|(1<<F)|(1<<G);
			break;
		case '-':
			segments = (1<<G);
			break;
		case '"':
			segments = (1<<B)|(1<<F);
			break;
		case 0x27:	// "'"
			segments = (1<<B);
			break;
		case '_':
			segments = (1<<D);
			break;
#endif // FEATURE_CHARACTERS
		case ' ':
		default:
			segments = 0;
			break;
	}

	return segments;
}

// Output number to digit 0, 1, 2 or 3
void display(uint8_t pos, uint8_t digit)
{	
	clearDisplay();

	//  Turn on selected digit
	switch (digit)
	{
		case 1:
			SET_DIGIT(DIGIT1_PORT, DIGIT1_BIT);
			break;
		case 2:
			SET_DIGIT(DIGIT2_PORT, DIGIT2_BIT);
			break;
		case 3:
			SET_DIGIT(DIGIT3_PORT, DIGIT3_BIT);
			break;	
		case 4:
			SET_DIGIT(DIGIT4_PORT, DIGIT4_BIT);
			break;
	}

	uint8_t segments = 0;

	if (data[pos] & 0b10000000)
		segments = segment_data[pos];
	else
		segments = calculate_segments(data[pos]);
	
	// set dot
	if (dots & (1<<digit))
		segments |= (1<<DP);

	// This allows segments do be on different ports
	if (segments & _BV(7)) SET_SEGMENT(DP_PORT, DP_BIT);
	else CLEAR_SEGMENT(DP_PORT, DP_BIT);

	if (segments & _BV(6)) SET_SEGMENT(G_PORT, G_BIT);
	else CLEAR_SEGMENT(G_PORT, G_BIT);

	if (segments & _BV(5)) SET_SEGMENT(F_PORT, F_BIT);
	else CLEAR_SEGMENT(F_PORT, F_BIT);

	if (segments & _BV(4)) SET_SEGMENT(E_PORT, E_BIT);
	else CLEAR_SEGMENT(E_PORT, E_BIT);

	if (segments & _BV(3)) SET_SEGMENT(D_PORT, D_BIT);
	else CLEAR_SEGMENT(D_PORT, D_BIT);

	if (segments & _BV(2)) SET_SEGMENT(C_PORT, C_BIT);
	else CLEAR_SEGMENT(C_PORT, C_BIT);
			
	if (segments & _BV(1)) SET_SEGMENT(B_PORT, B_BIT);
	else CLEAR_SEGMENT(B_PORT, B_BIT);

	if (segments & _BV(0)) SET_SEGMENT(A_PORT, A_BIT);
	else CLEAR_SEGMENT(A_PORT, A_BIT);
}

void clearDisplay(void)
{
	// all digits low
	CLEAR_DIGIT(DIGIT1_PORT, DIGIT1_BIT);
	CLEAR_DIGIT(DIGIT2_PORT, DIGIT2_BIT);
	CLEAR_DIGIT(DIGIT3_PORT, DIGIT3_BIT);
	CLEAR_DIGIT(DIGIT4_PORT, DIGIT4_BIT);
	
	// all segments high
	CLEAR_SEGMENT(A_PORT,  A_BIT);
	CLEAR_SEGMENT(B_PORT,  B_BIT);
	CLEAR_SEGMENT(C_PORT,  C_BIT);
	CLEAR_SEGMENT(D_PORT,  D_BIT);
	CLEAR_SEGMENT(E_PORT,  E_BIT);
	CLEAR_SEGMENT(F_PORT,  F_BIT);
	CLEAR_SEGMENT(G_PORT,  G_BIT);
	CLEAR_SEGMENT(DP_PORT, DP_BIT);
}
