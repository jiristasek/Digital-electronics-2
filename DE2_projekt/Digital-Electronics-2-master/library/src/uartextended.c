/* Includes ----------------------------------------------------------*/
#include "uartextended.h"
#include "uart.h"

/* Define ------------------------------------------------------------*/

void uart_clrscr(void)
{
	uart_putc(0x1b); //ESC command
	uart_puts("[2J"); //[2J completes the clear screen command
	uart_putc(0x1b); //ESC command
	uart_puts("[H"); //[H completes the home screen command
}

/*--------------------------------------------------------------------*/

void uart_putnum(uint16_t num, uint8_t scale) {
	char numberBuffer[16];
	itoa(num, numberBuffer, scale);
	uart_puts(numberBuffer);
}
