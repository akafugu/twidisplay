// Based on: http://code.google.com/p/wirejungle/source/browse/trunk/libraries/HT1621/?r=17
// Modified by Akafugu Corporation 2012

#include "ht1621.h"
#include <avr/io.h>

#define ADDR_MAX 128

#define DATA_BIT PORTB1
#define DATA_DDR DDRB
#define DATA_PORT PORTB
#define DATA_PIN  PINB
#define DATA_HIGH DATA_PORT |= _BV(DATA_BIT)
#define DATA_LOW  DATA_PORT &= ~(_BV(DATA_BIT))

#define WR_BIT PORTB2
#define WR_DDR DDRB
#define WR_PORT PORTB
#define WR_HIGH WR_PORT |= _BV(WR_BIT)
#define WR_LOW  WR_PORT &= ~(_BV(WR_BIT))

#define RD_BIT PORTB3
#define RD_DDR DDRB
#define RD_PORT PORTB
#define RD_HIGH RD_PORT |= _BV(RD_BIT)
#define RD_LOW  RD_PORT &= ~(_BV(RD_BIT))

#define CS_BIT PORTB4
#define CS_DDR DDRB
#define CS_PORT PORTB
#define CS_HIGH CS_PORT |= _BV(CS_BIT)
#define CS_LOW  CS_PORT &= ~(_BV(CS_BIT))

#define COMMAND_MODE 0b100
#define WRITE_MODE 0b101
#define READ_MODE 0b110

void write_bits(uint8_t data, uint8_t cnt);
uint8_t read_bits(uint8_t cnt);
bool test_mem(void);

bool test_mem(void)
{
    uint8_t test = 10;
    ht1621_write(0x5a, test);
    if (ht1621_read(0x5a) != test)
        return false;
    return true;
}

bool ht1621_init(void)
{
	// set pins as output
	DATA_DDR |= _BV(DATA_BIT);
	WR_DDR   |= _BV(WR_BIT);
	RD_DDR   |= _BV(RD_BIT);
	CS_DDR   |= _BV(CS_BIT);

	// set all pins high
	DATA_HIGH;
	WR_HIGH;
	RD_HIGH;
	CS_HIGH;

    ht1621_send_cmd(SYS_DIS);
    if (!test_mem())
        return false;

    ht1621_memset(0, 0, ADDR_MAX);
    ht1621_send_cmd(SYS_EN);
    ht1621_send_cmd(LCD_ON);

    return true;
}

void write_bits(uint8_t data, uint8_t cnt)
{
    while (cnt) {
        WR_LOW;
        
        uint8_t bitval = (data & (1 << (cnt - 1))) ? 1 : 0;
        if (bitval)
	        DATA_HIGH;
        else
	        DATA_LOW;
        
        WR_HIGH;
        cnt--;
    }
}

uint8_t read_bits(uint8_t cnt)
{
    uint8_t data = 0;

    DATA_DDR &= ~(_BV(DATA_BIT)); // input
    //DATA_PORT |= _BV(DATA_BIT);

    while (cnt) {
        RD_LOW;

        DATA_DDR &= ~(_BV(DATA_BIT)); // input

        if (DATA_PIN & _BV(DATA_BIT))
		    data += (1 << (cnt - 1));

        RD_HIGH;
        cnt--;
    }
    
    DATA_DDR   |= _BV(DATA_BIT); // output
    return data;
}

void ht1621_send_cmd(uint8_t cmd)
{
        CS_LOW;
        write_bits(COMMAND_MODE, 3);
    write_bits(cmd, 8);
    write_bits(0, 1); //Last bit - don't care

        CS_HIGH;
}

void ht1621_write(uint8_t address, uint8_t data)
{
    CS_LOW;
    write_bits(WRITE_MODE, 3);
    write_bits(address, 6);
    write_bits(data, 4);
    CS_HIGH;
}

void ht1621_write_check(uint8_t address, uint8_t data)
{
    if (ht1621_read(address) != data) {
        CS_LOW;
        write_bits(WRITE_MODE, 3);
        write_bits(address, 6);
        write_bits(data, 4);
        CS_HIGH;
    }
}

void ht1621_memset(uint8_t address, uint8_t data, uint8_t cnt)
{
  CS_LOW;
  write_bits(WRITE_MODE, 3);
  write_bits(address, 6);
  for (uint8_t i = 0; i < cnt; i++)
    write_bits(data, 4);
  CS_HIGH;
}

uint8_t ht1621_read(uint8_t address)
{
    uint8_t data;
    CS_LOW;
    write_bits(READ_MODE, 3);
    write_bits(address, 6);
    data = read_bits(4);
    CS_HIGH;
    return data;
}

/*
void ht1621_memread(uint8_t address, uint8_t *data, uint8_t cnt)
{
    CS_LOW;
    write_bits(READ_MODE, 3);
    write_bits(address, 6);
    for (uint8_t i = 0; i < cnt; i++)
        data[i] = read_bits(4);
    CS_HIGH;
}
*/
