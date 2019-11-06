/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-23
 * Last update: 2019-11-01
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Analog-to-digital conversion with displaying result on LCD and 
 *    transmitting via UART.
 * 
 * Note:
 *    Peter Fleury's UART library.
 */

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>             // itoa() function
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "lcd.h"
#include "uart.h"
#include <string.h>

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Read ADC result and transmit via UART.
 *  Input:  None
 *  Return: None
 */
char* getNameUART(uint16_t val);
char* getNameLCD(uint16_t val);

char buffer[16] = "";

int main(void)
{
    // LCD display
    lcd_init(LCD_DISP_ON);
    lcd_puts("Main Menu");
    lcd_gotoxy(0,1);
    lcd_puts("c-start");

    /* ADC
     * TODO: Configure ADC reference, clock source, enable ADC module, 
     *       and enable conversion complete interrupt */
    ADMUX &= ~(_BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
    ADMUX &= ~_BV(ADLAR);
    ADMUX |= _BV(REFS0);
    ADCSRA |= _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */

    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
    TIM_config_prescaler(TIM1,TIM_PRESC_64);

    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    // Enables interrupts by setting the global interrupt mask
    sei();


    // Put string to ringbuffer for transmitting via UART.
    //uart_puts("UART testing\r\n");
    // Infinite loop
    for (;;) {
        char character = uart_getc();
        if (character == 'c'){
            ADCSRA |= _BV(ADEN);
        }
        else if (character == 'b'){
            lcd_clrscr();
            lcd_puts("Main Menu");
            lcd_gotoxy(0,1);
            lcd_puts("c-start");
            ADCSRA &= ~_BV(ADEN);
        }

    }

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Start ADC conversion.
 */
ISR(TIMER1_OVF_vect)
{
    // TODO: Start ADC conversion
    ADCSRA |= _BV(ADSC);
}

/**
 *  Brief: ADC complete interrupt routine. Update LCD and UART 
 *         transmiter.
 */
ISR(ADC_vect)
{
    
    uint16_t value = 0;
    char uart_string[4];
    char test[16];

	// Read 10-bit ACD value
    value = ADC;

    //LCD part
    lcd_clrscr();
    lcd_puts(itoa(value,buffer,10));
    lcd_gotoxy(5,0);
    lcd_puts("- ");
    strcpy(test, getNameLCD(value));
    lcd_puts(test);
    lcd_gotoxy(5,1);
    lcd_gotoxy(0,1);
    lcd_puts("b-stop");
    //lcd_puts(itoa(16.2 % 3,buffer,10));

    //UART part
    uart_puts(itoa(value,uart_string,10));
    uart_puts(" - ");
    strcpy(test, getNameUART(value));
    uart_puts(test);
    uart_puts("\n\r");

    // TODO: Update LCD and UART transmiter
}

char* getNameUART(uint16_t val){

    if (val <= 10) return "\033[4;33mRIGHT\033[0m";
    else if (val > 90 && val < 110) return "\033[4;32mUP\033[0m";
    else if (val > 240 && val < 260) return "\033[4;35mDOWN\033[0m";
    else if (val > 390 && val < 420) return "\033[4;36mLEFT\033[0m";
    else if (val > 610 && val < 660) return "\033[4;31mSELECT\033[0m";
    else return "UNKNOWN";
    
}

char* getNameLCD(uint16_t val){

    if (val <= 10) return "RIGHT";
    else if (val > 90 && val < 110) return "UP";
    else if (val > 240 && val < 260) return "DOWN";
    else if (val > 390 && val < 420) return "LEFT";
    else if (val > 610 && val < 660) return "SELECT";
    else return "UNKNOWN";
    
}
