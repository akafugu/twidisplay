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
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "usiTwiSlave.h"
#include "display.h"

#define SLAVE_ADDRESS 0x12
// brightness range 1-100
#ifndef DEFAULT_BRIGHTNESS
#define DEFAULT_BRIGHTNESS 100
#endif // DEFAULT_BRIGHTNESS

uint8_t EEMEM b_slave_address = SLAVE_ADDRESS;

#ifdef FEATURE_CHANGE_TWI_ADDRESS
void init_EEPROM(void)
{
	eeprom_write_byte(&b_slave_address, SLAVE_ADDRESS);
}
#endif

void init(void)
{
	cli();	// disable interrupts

	uint8_t stored_address = eeprom_read_byte(&b_slave_address);
	// Check that stored_address is sane
	if (stored_address >= 128)
		stored_address = SLAVE_ADDRESS;
	
	usiTwiSlaveInit(stored_address);
	display_init();
	
	sei(); // enable interrupts
 
}

// scroll mode
#define ROTATE 0 // use a rotating 4 byte buffer to store data
#define SCROLL 1  // scroll left each time a byte is received

uint8_t scroll_mode = 0;
uint8_t counter = 0;

void processTWI( void )
{
	uint8_t b,c;

	b = usiTwiReceiveByte();
	
	switch (b) {
		case 0x80: // set brightness (nop)
			c = usiTwiReceiveByte();
			break;
#ifdef FEATURE_CHANGE_TWI_ADDRESS
		case 0x81: // set slave address
			c = usiTwiReceiveByte();
			if(c < 128) // Address is 7 bit
			{
				eeprom_update_byte(&b_slave_address, c);
				usiTwiSlaveInit(eeprom_read_byte(&b_slave_address));
			}
			break;
#endif
		case 0x82: // clear display
			clear_screen();
			set_dots(0);
			set_apostrophes(0);
			counter = 0;
			break;
		case 0x83: // set scroll mode
			c = usiTwiReceiveByte();

			if (c == 0)
				scroll_mode = ROTATE;
			else
				scroll_mode = SCROLL;
			break;
		case 0x84: // receive segment data
			c = usiTwiReceiveByte(); // segment data

			if (scroll_mode == ROTATE) {
				set_segments_at(c, counter++);
				if (counter >= 8) counter = 0;
			}
			else {
				shift_in_segments(c);		
			}

			break;
		case 0x85: // set dots
			c = usiTwiReceiveByte();
			set_dots(c);
			break;
#ifdef FEATURE_SET_TIME
		case 0x87: // display time (hh:mm with seconds controlling middle dot)
			set_time(usiTwiReceiveByte(), usiTwiReceiveByte(), usiTwiReceiveByte());
			break;
#endif // FEATURE_SET_TIME
		case 0x88: // display integer
			{
				uint8_t i1 = usiTwiReceiveByte();
				uint8_t i2 = usiTwiReceiveByte();
			
				uint16_t i = (i2 << 8) + i1;
				set_number(i);
			}
			break;
		case 0x89: // set position (only valid for ROTATE mode)
			counter = usiTwiReceiveByte();
			break;
		case 0x8a: // get firmware revision
			usiTwiTransmitByte(2);
			break;
		case 0x8b: // get number of digits
			usiTwiTransmitByte(8);
			break;
		case 0x8c: // get number of segments
			usiTwiTransmitByte(14);
			break;
		case 0x90: // Show address
		{
			uint8_t address = eeprom_read_byte(&b_slave_address);
			uint8_t data[3];
			data[2] = address % 10;
			address /= 10;
			data[1] = address % 10;
			address /= 10;
			data[0] = address % 10;
				
			set_char_at('A', 0);
			set_char_at(data[0], 1);
			set_char_at(data[1], 2);
			set_char_at(data[2], 3);
		}
		    break;
	    case 0x91: // Control piezo
		    c = usiTwiReceiveByte();
		    set_tone(c);
			break;
	    case 0x92: // Control bias
		    c = usiTwiReceiveByte();
		    set_bias(c);
			break;
		case 0x93: // set apostophes
			c = usiTwiReceiveByte();
			set_apostrophes(c);
			break;
		case 0x94: // receive 16-bit segment data
			{
				uint8_t i1 = usiTwiReceiveByte();
				uint8_t i2 = usiTwiReceiveByte();
			
				//uint16_t i = (i2 << 8) + i1;

				if (scroll_mode == ROTATE) {
					set_segments_at_16(i1, i2, counter++);
					if (counter >= 8) counter = 0;
				}
				else {
					shift_in_segments_16(i1, i2);		
				}
			}

			break;
		default:
			if (b >= 0x80) break; // anything above 0x80 is considered a reserved command and is ignored

			if (scroll_mode == ROTATE) {
				set_char_at(b, counter++);
				if (counter >= 8) counter = 0;
			}
			else {
				shift_in(b);		
			}
			break;
	}
}

#include <util/delay.h>

void main(void) __attribute__ ((noreturn));

void main(void)
{
	init();

#ifdef DEMO
	// for testing
	while (1) {
		for (uint16_t i = 0; i < 99999999; i++) {
			set_number(i);
			dots = i%2 ? 0 : 0xff;
			_delay_ms(50);
			}
	}
#endif

	// clear display
	clear_screen();
	
#ifdef FEATURE_SHOW_ADDRESS_ON_STARTUP
	uint8_t counter = 0;
	
#define MAX_COUNTER	200

	while(!usiTwiDataInReceiveBuffer() && counter <= MAX_COUNTER)
	{
		counter++;
		_delay_ms(10);

		if(counter == MAX_COUNTER)
		{
			// fixme: What if someone sets brighness to 0 and then changes the address
			// should reset brightness here if it is too dim to see.

			uint8_t address = eeprom_read_byte(&b_slave_address);
			uint8_t data[3];
			data[2] = address % 10;
			address /= 10;
			data[1] = address % 10;
			address /= 10;
			data[0] = address % 10;
			
			set_char_at('A', 0);
			set_char_at('d', 1);
			set_char_at('d', 2);
			set_char_at('r', 3);
			set_char_at(data[0], 4);
			set_char_at(data[1], 5);
			set_char_at(data[2], 6);
		}
	}
#endif //FEATURE_SHOW_ADDRESS_ON_STARTUP
	
	while (1) {
		while (usiTwiDataInReceiveBuffer())	{ // process I2C command
			processTWI();
		}
	}
}

