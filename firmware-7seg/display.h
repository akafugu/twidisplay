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
void set_number(uint16_t num);
void clear_screen(void);
#ifdef FEATURE_SET_TIME
void set_time(uint8_t hour, uint8_t min, uint8_t sec);
#endif // FEATURE_SET_TIME
// PINOUT for ATMEGA328 in TQFP package
#ifdef ATMEGA328_TQFP

// Segment A: PB7
#define A_DDR  DDRB
#define A_PORT PORTB
#define A_BIT  PB7
#define A	0
// Segment B: PB0
#define B_DDR DDRB
#define B_PORT PORTB
#define B_BIT  PB0
#define B	1
// Segment C: PC0
#define C_DDR DDRC
#define C_PORT PORTC
#define C_BIT  PC0
#define C	2
// Segment D: PC2
#define D_DDR DDRC
#define D_PORT PORTC
#define D_BIT  PC2
#define D	3
// Segment E: PC3
#define E_DDR DDRC
#define E_PORT PORTC
#define E_BIT  PB3
#define E	4
// Segment F: PD5
#define F_DDR DDRD
#define F_PORT PORTD
#define F_BIT  PD5
#define F	5
// Segment G: PC4
#define G_DDR DDRC
#define G_PORT PORTC
#define G_BIT  PC4
#define	G	6
// Segment DP: PC1
#define DP_DDR DDRC
#define DP_PORT PORTC
#define DP_BIT  PC1
#define	DP	7

// bits and ports for digits (For Atmega XX8 TQFP-32 pinout)
// Digit 1: PB6
#define DIGIT1_DDR DDRB
#define DIGIT1_PORT PORTB
#define DIGIT1_BIT  6
// Digit 2: PD6
#define DIGIT2_DDR DDRD
#define DIGIT2_PORT PORTD
#define	DIGIT2_BIT	6
// Digit 3: PD7
#define DIGIT3_DDR DDRD
#define DIGIT3_PORT PORTD
#define DIGIT3_BIT	7
// Digit 4: PC5
#define DIGIT4_DDR DDRC
#define DIGIT4_PORT PORTC
#define	DIGIT4_BIT	5

// Pinout for ATTiny2313
#else

// Segment A: PB0
#define A_DDR  DDRB
#define A_PORT PORTB
#define A_BIT  PB0
#define A	0
// Segment B: PB4 (pin 3)
#define B_DDR DDRB
#define B_PORT PORTB
#define B_BIT  PB4
#define B	1
// Segment C: PD3 (pin 4)
#define C_DDR DDRD
#define C_PORT PORTD
#define C_BIT  PD3
#define C	2
// Segment D: PD5 (pin 2)
#define D_DDR DDRD
#define D_PORT PORTD
#define D_BIT  PD5
#define D	3
// Segment E: PA1 (pin 1)
#define E_DDR DDRA
#define E_PORT PORTA
#define E_BIT  PA1
#define E	4
// Segment F: PB1 (pin 10)
#define F_DDR DDRB
#define F_PORT PORTB
#define F_BIT  PB1
#define F	5
// Segment G: PD2 (pin 5)
#define G_DDR DDRD
#define G_PORT PORTD
#define G_BIT  PD2
#define	G	6
// Segment DP: PD4 (pin 7)
#define DP_DDR DDRD
#define DP_PORT PORTD
#define DP_BIT  PD4
#define	DP	7

// Digit 1: PD6 (pin 12)
#define DIGIT1_DDR DDRD
#define DIGIT1_PORT PORTD
#define DIGIT1_BIT  6
// Digit 2: PB2 (pin 9)
#define DIGIT2_DDR DDRB
#define DIGIT2_PORT PORTB
#define	DIGIT2_BIT	2
// Digit 3: PB3 (pin 8)
#define DIGIT3_DDR DDRB
#define DIGIT3_PORT PORTB
#define DIGIT3_BIT	3
// Digit 4: PA0 (pin 6)
#define DIGIT4_DDR DDRA
#define DIGIT4_PORT PORTA
#define	DIGIT4_BIT	0

#endif

#endif // DISPLAY_H_
