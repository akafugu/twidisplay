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

//using GHIElectronics.NETMF.FEZ;

namespace Akafugu
{
    class TWIDisplay
    {
        //private int m_addr;
        private byte m_dots;
        private byte[] m_data;
        I2CDevice m_display;

        // Create new instance using a pre-created I2CDevice
        public TWIDisplay(I2CDevice display)
        {
            m_data = new byte[4];
            m_dots = 0;
            m_display = display;

            setRotateMode();
        }

        public TWIDisplay(ushort address)
        {
            I2CDevice.Configuration config = new I2CDevice.Configuration(address, 100);
            m_display = new I2CDevice(config);

            m_data = new byte[4];
            m_dots = 0;

            setRotateMode();
        }

        private void set_number(int num)
        {
            m_data[3] = (byte)(num % 10);
            num /= 10;
            m_data[2] = (byte)(num % 10);
            num /= 10;
            m_data[1] = (byte)(num % 10);
            num /= 10;
            m_data[0] = (byte)(num % 10);
        }

        public void changeAddress(uint new_addr) // 0x81
        {
            if (new_addr > 255) return;

            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x80, (byte)new_addr })
                    };
            m_display.Execute(write, 1000);
        }

        public void showAddress()
        {
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x90 })
                    };
            m_display.Execute(write, 1000);
        }

        public void setBrightness(uint brightness)
        {
            if (brightness > 255) brightness = 0;

            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x80, (byte)brightness })
                    };
            m_display.Execute(write, 1000);
        }

        public void clear()
        {
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x82 })
                    };
            m_display.Execute(write, 1000);
        }

        public void setRotateMode()
        {
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x83, 0x0 })
                    };
            m_display.Execute(write, 1000);
        }

        public void setScrollMode()
        {
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x83, 0x1 })
                    };
            m_display.Execute(write, 1000);
        }

        public void setDot(int position, bool on)
        {
            if (position > 3) return;

            if (on) m_dots |= (byte)(1 << (position + 1));
            else m_dots &= (byte)(~(1 << (position + 1)));

            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x85, m_dots })
                    };
            m_display.Execute(write, 1000);
        }

        public void setDots(bool dot0, bool dot1, bool dot2, bool dot3)
        {
            m_dots = 0;
            if (dot0) m_dots |= 1 << 1;
            if (dot1) m_dots |= 1 << 2;
            if (dot2) m_dots |= 1 << 3;
            if (dot3) m_dots |= 1 << 4;

            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x85, m_dots })
                    };
            m_display.Execute(write, 1000);
        }

        public void setPosition(uint position)
        {
            if (position > 3) return;

            // set rotate mode, then set position
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x83, 0x0, 0x89, (byte)position })
                    };
            m_display.Execute(write, 1000);
        }

        public void writeInt(int val)
        {
            set_number(val);

            // send clear, then 4 bytes
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x82, m_data[0], m_data[1], m_data[2], m_data[3] })
                    };
            m_display.Execute(write, 1000);
        }

        public void writeChar(char val)
        {
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] {(byte)val})
                    };
            m_display.Execute(write, 1000);
        }

        public void writeStr(string val)
        {
    	    clear();

            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] {(byte)val[0], (byte)val[1], (byte)val[2], (byte)val[3]})
                    };
            m_display.Execute(write, 1000);
        }

        public void writeTemperature(int temp, char symbol)
        {
            if (temp >= 0)
            {
                set_number(temp * 100);

                // send clear, then 3 bytes, then symbol, then dots, then dots value
                I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x82, m_data[0], m_data[1], m_data[2], (byte)symbol, 0x85, (1<<2)})
                    };
                m_display.Execute(write, 1000);
            }
            else
            {
                set_number(-temp * 100);

                // send clear, then '-', then 2 bytes, then symbol
                I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x82, (byte)'-', m_data[0], m_data[1], (byte)symbol})
                    };
                m_display.Execute(write, 1000);
            }
        }

        public void writeTemperature(int temp)
        {
            writeTemperature(temp, 'C');
        }

        public void writeTemperature(float f, char symbol)
        {
            //fixme: implement
        }

        public void writeTime(int hour, int min, int sec)
        {
            set_number(hour * 100 + min);
            byte dots = 0;
            if ((sec % 2) == 1) dots = 1 << 2;

            // send clear, then 4 bytes, then dots, then dots on/off
            I2CDevice.I2CTransaction[] write = new I2CDevice.I2CTransaction[] {
                        I2CDevice.CreateWriteTransaction(new byte[] { 0x82, m_data[0], m_data[1], m_data[2], m_data[3], 0x85, dots })
                    };
            m_display.Execute(write, 1000);
        }

        public void writeSegments(int segments)
        {

        }
    }
}
