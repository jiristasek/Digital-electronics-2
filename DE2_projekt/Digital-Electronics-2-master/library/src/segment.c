/***********************************************************************
 * Title:    Seven-segment display library
 * Author:   Tomas Fryza, Brno University of Technology, Czechia
 * Software: avr-gcc, tested with avr-gcc 4.9.2
 * Hardware: Any AVR
 *
 * MIT License
 *
 * Copyright (c) 2019 Tomas Fryza
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * EDITED FOR SEMESTRAL PROJECT PURPOSES by Radim Dvo¯·k & Ji¯Ì äù·sek
 **********************************************************************/

 /* Includes ----------------------------------------------------------*/
#include <util/delay.h>
#include "GPIO.h"
#include "segment.h"

/* Define ------------------------------------------------------------*/
/* Variables ---------------------------------------------------------*/
/* Active HIGH digit 0 to 9 */
uint8_t segment_digit[] = {
	//DPgfedcba
	 0xff - 0b11000000,      // Digit 0
	 0xff - 0b11111001,      // Digit 1
	 0xff - 0b10100100,      // Digit 2
	 0xff - 0b10110000,      // Digit 3
	 0xff - 0b10011001,      // Digit 4
	 0xff - 0b10010010,      // Digit 5
	 0xff - 0b10000010,      // Digit 6
	 0xff - 0b11111000,      // Digit 7
	 0xff - 0b10000000,      // Digit 8
	 0xff - 0b10011000 };     // Digit 9

 /* Active LOW position 0 to 3 */
uint8_t segment_position[] = {
	0xff - 0b00001000,   // Position 0
	0xff - 0b00000100,   // Position 1
	0xff - 0b00000010,   // Position 2
	0xff - 0b00000001 };  // Position 3
	
 /* Active HIGH LED digits LSB -> MSB */
uint8_t segment_LEDs[] = {
	0b11111111,		// 8 LEDS on
	0b11111110,		// 7 LEDS on
	0b11111100,		// 6 LEDS on
	0b11111000,		// 5 LEDS on
	0b11110000,		// 4 LEDS on
	0b11100000,		// 3 LEDS on
	0b11000000,		// 2 LEDS on
	0b10000000,		// 1 LED on
	0b00000000		// 0 LEDS on
	 }; 

/* Functions ---------------------------------------------------------*/
void SEG_putc(uint16_t digit,
	uint8_t position, uint8_t number_of_LEDs_OFF)
{
	
	switch(position){
		case 0:
			digit = ((digit % 1000)%100)%10;
			break;
		case 1:
			digit = ((digit % 1000)%100)/10;
			break;
		case 2:
			digit = (digit % 1000)/100;
			break;
		case 3:
			digit = digit/1000;
			break;
		default : 
			digit = 0;
			break;
	}
	/* Read values from look-up tables */
	digit = segment_digit[digit];
	position = segment_position[position];
	uint8_t LEDs_value = segment_LEDs[number_of_LEDs_OFF];

	/* Put 1st byte to serial data */
	SEG_shift_data(position);
	//Put 2nd byte to serial data 
	SEG_shift_data(digit);
	//Put 3rd byte to serial data 
	SEG_shift_data(LEDs_value);

	//Latches values to outputs of shift registers
	SEG_latch_data();
}

/*--------------------------------------------------------------------*/
void SEG_toggle_clk(void)
{
	GPIO_toggle(&PORTD, SEGMENT_CLK);
	_delay_us(1);
	GPIO_toggle(&PORTD, SEGMENT_CLK);
	_delay_us(1);
	/*  Generate 2 us clock period */
}

/*--------------------------------------------------------------------*/
void SEG_shift_data(uint8_t value){
	for (int i = 0; i < 8; i++) {
		GPIO_write(&PORTD, SEGMENT_DATA, (value >> (7 - i)) & 1);
		SEG_toggle_clk();
	}
}

void SEG_latch_data(){
	GPIO_toggle(&PORTD, SEGMENT_LATCH);
	_delay_us(1);
	GPIO_toggle(&PORTD, SEGMENT_LATCH);
	_delay_us(1);
}