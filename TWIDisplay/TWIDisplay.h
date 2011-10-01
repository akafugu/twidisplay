/*
 * TWIDisplay: Arduino Library for Akafugu TWI/I2C serial displays
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

#ifndef TWIDISPLAY_H
#define TWIDISPLAY_H

#include <Arduino.h>
#include <../Wire/Wire.h>

class TWIDisplay
{
public:
  TWIDisplay(int addr);

  void changeAddress(int new_addr);
  void showAddress();
  void setBrightness(int brightness);
  void clear();

  void setRotateMode();
  void setScrollMode();

  void setDot(int position, bool on);
  void setDots(bool dot0, bool dot1, bool dot2, bool dot3);
  void setPosition(int position);

  void writeInt(int val);
  void writeChar(char val);
  void writeStr(char* val);
  void writeTemperature(int temp, char symbol);
  void writeTemperature(int temp_t, int temp_f, char symbol);
  void writeTime(int hour, int min, int sec);
  void writeSegments(int segments);

private:
  void  set_number(uint16_t num);
  int m_addr;
  uint8_t m_dots;
  char m_data[4];
};


#endif // TWIDISPLAY_H
