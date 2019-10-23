/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-11
 * Last update: 2019-10-16
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description: Decimal counter with data display on 7-segment display.
 * TODO: Create and test functions to operate a 7-segment display using
 *       shift registers.
 * NOTE: Store segment.c and segment.h files in library/src and 
 *       library/inc folders.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include "GPIO.h"
#include "timer.h"
#include "segment.h"
#include <avr/interrupt.h>
#include <util/delay.h>

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define LED_D1          PB5
#define BTN_S1          PC1 // PCINT 9
#define BTN_S2          PC2 // PCINT 10
#define BTN_S3          PC3 // PCINT 11
/*
#define SEGMENT_DATA    PB0
#define SEGMENT_CLK     PD7
#define SEGMENT_LATCH   PD4
*/


/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Shows decimal values ​​on 7-segment display.
 *  Input:  None
 *  Return: None
 */
int a = 0;

int main(void)
{
    GPIO_config_input_nopull(&PORTC,BTN_S1);
    GPIO_config_input_nopull(&PORTC,BTN_S2);
    GPIO_config_input_nopull(&PORTC,BTN_S3);
    GPIO_config_output(&PORTB,LED_D1);

    GPIO_config_output(&PORTB,SEGMENT_DATA);
    GPIO_config_output(&PORTD,SEGMENT_CLK);
    GPIO_config_output(&PORTD,SEGMENT_LATCH);

    GPIO_write(&PORTD, SEGMENT_CLK, 0);
    GPIO_write(&PORTD, SEGMENT_LATCH, 0);
    
    TIMSK0 |= _BV(TOIE0);
    //TCCR0B |= _BV(CS02);
    TCCR0B |= _BV(CS01);
    TCCR0B |= _BV(CS00);

    TIMSK2 |= _BV(TOIE2);
    TCCR2B |= _BV(CS20);
    TCCR2B |= _BV(CS21);
    TCCR2B |= _BV(CS22);
    
    
    /* D1 led */
    // Configure D1 led at Multi-Function Shield
     /* Pin Change Interrupts 11:9 */
    //  Configure Pin Change Interrupts 11, 10, and 9
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT9) | _BV(PCINT10) | _BV(PCINT11) ;
    /* 7-segment display interface */
    //  Configure 7-segment display pins

    /* Enable interrupts by setting the global interrupt mask */
    sei();

    

    


    /* Infinite loop */
    for (;;) {
        a++;
        _delay_ms(100);

        //uint8_t hundreds = a % 1000
        
        
        
        // TODO: Use function to display digit 1 at position 0
       
     // _delay_ms(1000);
    }

    return (0);
}

/**
 *  Brief: Pin Change Interrupt 11:9 routine. Toggle a LED.
 */
ISR(PCINT1_vect)
{
    GPIO_toggle(&PORTB,LED_D1);
}

ISR(TIMER0_OVF_vect){
    uint8_t thousands = (int) a/1000;
    uint8_t hundreds = (int) (a % 1000)/100;
    uint8_t tenths = (int) (a % 1000)%100/10;
    uint8_t zbytek = (int) (a % 1000)%100%10;
    SEG_putc(zbytek, 0);
    SEG_putc(tenths, 1);
    SEG_putc(hundreds, 2);
    SEG_putc(thousands, 3);
}

ISR(TIMER2_OVF_vect){
    a++;
}
