/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-11-28
 * Last update: 2019-10-11
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description: Control LEDs using functions from GPIO and Timer 
 *              libraries. Do not use delay library any more.
 * TODO: Create and test functions to control 8-bit Timer/Counter0 and 
 *       16-bit Timer/Counter1 modules.
 * NOTE: Store timer.c and timer.h files in library/src and library/inc
 *       folders.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include "GPIO.h"
#include "timer.h"
#include <avr/interrupt.h>
#include <util/delay.h>

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define LED_GREEN   PB5
#define LED_RED     PB0
#define LED_BOARD   PB1
#define BTN         PD3

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
int main(void)
{
    /* LED_GREEN */
    GPIO_config_output(&DDRB, LED_GREEN);
    GPIO_write(&PORTB, LED_GREEN, 0);
    /* LED_RED */
    GPIO_config_output(&DDRB, LED_RED);
    GPIO_write(&PORTB, LED_RED, 0);
    /* LED_BOARD */
    GPIO_config_output(&DDRB, LED_BOARD);
    GPIO_write(&PORTB, LED_BOARD, 0);
    /* BTN */
    GPIO_config_input_pullup(&PORTD, BTN);
    GPIO_read(&PORTD, BTN);

    EICRA |= _BV(ISC11);

    EIMSK |= _BV(INT1);


    /* Set Timer0 */
    TIM_config_prescaler(TIM0, TIM_PRESC_1024);
    TIM_config_interrupt(TIM0, TIM_OVERFLOW_ENABLE);

    /* Set Timer1 */
    TIM_config_prescaler(TIM1, TIM_PRESC_1024);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    sei();

    /* Infinite loop */
    for (;;) {
    }

    return (0);
}


ISR(TIMER0_OVF_vect)
{
    GPIO_toggle(&PORTB, LED_GREEN);
}

ISR(TIMER1_OVF_vect)
{
 GPIO_toggle(&PORTB, LED_RED);
}

ISR(INT1_vect){
    GPIO_toggle(&PORTB, LED_BOARD);
    _delay_ms(500);

}