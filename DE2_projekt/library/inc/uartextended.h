#ifndef UART_EXTENDED_H
#define UART_EXTENDED_H

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>

/* Define ------------------------------------------------------------*/
//"Clears" screen on UART interface
void uart_clrscr(void);

//Puts number into the uart
void uart_putnum(uint16_t num, uint8_t scale);

#endif /* UART_EXTENDED_H */