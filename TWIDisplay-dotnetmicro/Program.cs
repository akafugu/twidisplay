/*
 * TWIDisplay: .net micro Library for Akafugu TWI/I2C serial displays
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

using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware;
using SecretLabs.NETMF.Hardware.Netduino;

using Akafugu;

namespace TWIDisplayTest
{
    public class Program
    {
        static TWIDisplay m_display;

        public static void Main()
        {
            I2CDevice.Configuration config = new I2CDevice.Configuration(0x12, 400);
            I2CDevice i2c = new I2CDevice(config);

            m_display = new TWIDisplay(i2c);

            m_display.clear();
            m_display.setBrightness(255);

            while (true)
            {
                m_display.clear();

                for (int i = 0; i < 10; i++)
                {
                    m_display.writeStr("DOT-");
                    Thread.Sleep(400);
                    m_display.writeStr("-NET");
                    Thread.Sleep(400);
                }

                testTime();

                m_display.writeTemperature(32, 'C');
                Thread.Sleep(1000);
                m_display.writeTemperature(-4, 'F');
                Thread.Sleep(1000);
                m_display.writeTemperature(-17);
                Thread.Sleep(1000);

                m_display.clear();

                for (int i = 0; i < 4; i++)
                {
                    m_display.setDot(i, true);
                    Thread.Sleep(500);
                }

                for (int i = 0; i < 4; i++)
                {
                    m_display.setDot(i, false);
                    Thread.Sleep(500);
                }

                m_display.clear();
                m_display.setRotateMode();

                for (char i = 'A'; i <= 'Z'; i++)
                {
                    m_display.writeChar(i);

                    Thread.Sleep(200);
                }

                m_display.clear();
                m_display.setScrollMode();

                for (char i = 'A'; i <= 'Z'; i++)
                {
                    m_display.writeChar(i);

                    Thread.Sleep(200);
                }

                m_display.clear();
                m_display.setRotateMode();

                for (int i = 0; i <= 9999; i += 3)
                {
                    m_display.writeInt(i);
                    Thread.Sleep(10);
                }
            }
        }

        static public void testTime()
        {
            // for the test, just "fake" the time and temperature
            byte temp = 25;
            byte hour = 12;
            byte min = 35;
            byte sec = 0;

            for (int i = 0; i < 10; i++)
            {
                // show time for 5 seconds then temperature for 5 seconds
                if (i < 5) m_display.writeTime(hour, min, sec);
                else m_display.writeTemperature(temp, 'C');

                // increase time
                sec++;
                if (sec == 60) { sec = 0; min++; }
                if (min == 60) { min = 0; hour++; }
                if (hour == 24) { hour = min = sec = 0; }

                Thread.Sleep(1000);
            }
        }
    }
}
