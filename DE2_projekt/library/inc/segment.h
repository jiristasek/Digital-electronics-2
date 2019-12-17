#ifndef SEGMENT_H_INCLUDED
#define SEGMENT_H_INCLUDED

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
 *
 * EDITED FOR SEMESTRAL PROJECT PURPOSES by Radim Dvo¯·k & Ji¯Ì äù·sek
 **********************************************************************/

 /**
  *  @file      segment.h
  *  @brief     Seven-segment display library for AVR-GCC.
  *
  *  @details   The library contains functions for controlling the seven-
  *             segment display using shift registers 74HC595.
  *
  *  @author    Tomas Fryza, Brno University of Technology, Czechia
  *  @date      2019-10-18
  *  @version   v1.0
  *  @copyright (c) 2019 Tomas Fryza, MIT License
  */

  /* Includes ----------------------------------------------------------*/
#include <avr/io.h>

/* Define ------------------------------------------------------------*/
/** @brief Defines the interface of 7-segment display. */
#define SEGMENT_DATA    PD4
#define SEGMENT_CLK     PD5
#define SEGMENT_LATCH   PD6

/* Function prototypes -----------------------------------------------*/
//Determines which signals need to be shifted out and shifts them out
void SEG_putc(uint16_t digit,
uint8_t position, uint8_t number_of_LEDs_OFF);

//Toggles clock of the shift register
void SEG_toggle_clk(void);

//Shifts 8 bit value into the shift register
void SEG_shift_data(uint8_t value);

//Latches data in shift registers to its outputs
void SEG_latch_data();

#endif /* SEGMENT_H_INCLUDED */