/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2017-10-27
 * Last update: 2019-11-08
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Scan TWI bus for all connected slave devices and transmit info
 *    to UART.
 */

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>         // itoa() function
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "uart.h"
#include "twi.h"

/* Typedef -----------------------------------------------------------*/
typedef enum {
    IDLE_STATE = 1,
    TRY_STATE,
    ACK_STATE
} state_t;

/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600


 
/* Variables ---------------------------------------------------------*/
state_t current_state = IDLE_STATE;

/* Function prototypes -----------------------------------------------*/

//void fsm_twi_scanner(void);

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Test all TWI slave addresses and scan 
 *          connected devices.
 *  Input:  None
 *  Return: None
 */
int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    // TWI
    twi_init();

    /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */
    TIM_config_prescaler(TIM1, TIM_PRESC_8);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

   // TIM_config_prescaler(TIM0, TIM_PRESC_64);
   // TIM_config_interrupt(TIM0, TIM_OVERFLOW_ENABLE);


    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put strings to ringbuffer for transmitting via UART.
    uart_puts("\r\n---T s---");
//    uart_puts("\r\n     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");

    // Infinite loop
    for (;;) {
    }

    // Will never reach this
    return (0);
}


/**
 *  Brief: Timer1 overflow interrupt routine. Update state of TWI Finite
 *         State Machine.
 */
ISR(TIMER1_OVF_vect)
{   // 0x5c - Humidity senzor
    // 0x68 - RTC
    // 0x57 - eeprom
    //fsm_twi_scanner();
    uint8_t H_i = 0 , H_f = 0 , T_i = 0 , T_f = 0;
    twi_start(0x5c<<1);
    twi_write(0x00);
    twi_stop();
    twi_start((0x5c<<1)+1);
    
    H_i = twi_read_ack();
    H_f = twi_read_ack();
    T_i = twi_read_ack();
    T_f = twi_read_nack();
    twi_stop();
    
    
    char buffer[128];
    uart_puts("\r\nHum : " );
    uart_puts(itoa(H_i,buffer,10));
    uart_puts(",");
    uart_puts(itoa(H_f,buffer,10));
    uart_puts("\r\nTem : " );
    uart_puts(itoa(T_i,buffer,10));
    uart_puts(",");
    uart_puts(itoa(T_f,buffer,10));


    

}

/**
 *  Brief: Test one TWI address.
 */

void fsm_twi_scanner(void)
{
    static uint8_t addr = 0;
    uint8_t status;
    char uart_string[3];

    switch (current_state) {
    case IDLE_STATE:
        if (addr < 128) {
            itoa(addr, uart_string, 16);
            uart_puts("\r\n");
            uart_puts(uart_string);
            current_state = TRY_STATE;
        }
        break;

    // Transmit TWI slave address and check status
    case TRY_STATE:
        status = twi_start((addr<<1) + TWI_WRITE);
        twi_stop();

        if (status == 0) {
            current_state = ACK_STATE;
        } else {
            addr++;
            current_state = IDLE_STATE;
        }
        break;

    // Slave ACK received
    case ACK_STATE:
        uart_puts(" OK - REEEE");
        addr++;
        current_state = IDLE_STATE;
        break;

    default:
        current_state = IDLE_STATE;
    }
}
