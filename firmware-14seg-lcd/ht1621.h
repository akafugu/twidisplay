// Based on: http://code.google.com/p/wirejungle/source/browse/trunk/libraries/HT1621/?r=17
// Modified by Akafugu Corporation 2012

#ifndef HT1621_H
#define HT1621_H

#include <inttypes.h>
#include <stdbool.h>

enum HT1621_commands {
	SYS_DIS   = 0b00000000,
	SYS_EN    = 0b00000001,
	LCD_OFF   = 0b00000010,
	LCD_ON    = 0b00000011,
	TIMER_DIS = 0b00000100,
	WDT_DIS   = 0b00000101,
	TIMER_EN  = 0b00000110,
	WDT_EN    = 0b00000111,
	TONE_OFF  = 0b00001000,
	TONE_ON   = 0b00001001,
	
	// Set bias and commons: 1/2 or 1/3 bias, 2,3 or 4 commons
	BIAS_HALF_2_COM  = 0b00100000,
	BIAS_HALF_3_COM  = 0b00100100,
	BIAS_HALF_4_COM  = 0b00101000,
	BIAS_THIRD_2_COM = 0b00100001,
	BIAS_THIRD_3_COM = 0b00100101,
	BIAS_THIRD_4_COM = 0b00101001,
	
	// Buzzer tones
	TONE_4K = 0b010000000,
	TONE_2K = 0b011000000,

	// For factory testing
	TEST_ON   = 0b11100000,
	TEST_OFF  = 0b11100011
};

bool ht1621_init(void);
void ht1621_send_cmd(uint8_t cmd);
void ht1621_write(uint8_t address, uint8_t data);
void ht1621_write_check(uint8_t address, uint8_t data);
uint8_t ht1621_read(uint8_t address);
void ht1621_memset(uint8_t address, uint8_t data, uint8_t cnt);

#endif //HT1621_H
