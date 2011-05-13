/*
 * TWIDisplay: Arduino Library for Akafugu TWI/I2C serial displays
 * (C) 2011 Akafugu
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

#include "TWIDisplay.h"

TWIDisplay::TWIDisplay(int addr)
	: m_addr(addr)
{
}



void  TWIDisplay::set_number(uint16_t num)
{
	m_data[3] = num % 10;
	num /= 10;
	m_data[2] = num % 10;
	num /= 10;
	m_data[1] = num % 10;
	num /= 10;
	m_data[0] = num % 10;
}


void TWIDisplay::changeAddress(int new_addr)
{
	Wire.beginTransmission(m_addr);
	Wire.send(0x81); // change address
	Wire.send(new_addr);
	Wire.endTransmission();
}

void TWIDisplay::setBrightness(int brightness)
{
	Wire.beginTransmission(m_addr);
	Wire.send(0x80); // set brightness
	Wire.send(brightness);
	Wire.endTransmission();
}

void TWIDisplay::clear()
{
	Wire.beginTransmission(m_addr);
	Wire.send(0x82); // clear
	Wire.endTransmission();	
}

void TWIDisplay::setRotateMode()
{
	Wire.beginTransmission(m_addr);
	Wire.send(0x80); //  set scroll mode
	Wire.send(0); // rotate mode
	Wire.endTransmission();
}

void TWIDisplay::setScrollMode()
{
	Wire.beginTransmission(m_addr);
	Wire.send(0x80); //  set scroll mode
	Wire.send(1); // scroll mode
	Wire.endTransmission();
}


void TWIDisplay::setSegmentData(int position, int segments)
{
	//fixme: implement
}

void TWIDisplay::setDot(int position, bool on)
{
	//fixme: implement
	
	// dots = (1 << position);
	Wire.beginTransmission(m_addr);
	Wire.send(0x85); //  set dots
	Wire.send(0b1111);
	Wire.endTransmission();
}

void TWIDisplay::setDots(bool dot0, bool dot1, bool dot2, bool dot3)
{
	Wire.beginTransmission(m_addr);
	Wire.send(0x85); //  set dots
	Wire.send(0b1111);
	Wire.endTransmission();
}

void TWIDisplay::setPosition(int position)
{
	Wire.beginTransmission(m_addr);
	Wire.send(0x89); // set position
	Wire.send(position);
	Wire.endTransmission();
}

void TWIDisplay::writeInt(int val)
{
	Wire.beginTransmission(m_addr);

	set_number(val);
	
	for (int i = 0; i <= 3; i++) {
		Wire.send(m_data[i]);
	}

	Wire.endTransmission();
}

void TWIDisplay::writeChar(char val)
{
	Wire.beginTransmission(m_addr);
	Wire.send(val);
	Wire.endTransmission();
}

void TWIDisplay::writeStr(char* val)
{
	Wire.beginTransmission(m_addr);
	
	for (uint8_t i = 0; i < strlen(val); i++) {
		Wire.send(val[i]);
		if (i == 3) break;
	}
	
	Wire.endTransmission();
}

void TWIDisplay::writeTemperature(int temp, char symbol)
{
	Wire.beginTransmission(m_addr);
	
	set_number(temp*100);
	
	for (int i = 0; i <= 2; i++) {
		Wire.send(m_data[i]);
	}
	
	Wire.send(symbol);
	
	Wire.send(0x85); // set dots
	Wire.send(1<<2);

	Wire.endTransmission();
}

void TWIDisplay::writeTime(int hour, int min, int sec)
{
	set_number(hour*100 + min);
	
	Wire.beginTransmission(m_addr);
	Wire.send(0x82); // clear
	
	// data
	for (int i = 0; i <= 3; i++) {
		Wire.send(m_data[i]);
	}
	
	Wire.endTransmission();
	
	// second dot on/off
	Wire.beginTransmission(m_addr);
	
	if (sec % 2 == 0) {
		Wire.send(0x85); // set dots
		Wire.send(0);
	}
	else {
		Wire.send(0x85); // set dots
		Wire.send(1<<2);
	}

	Wire.endTransmission();
}



