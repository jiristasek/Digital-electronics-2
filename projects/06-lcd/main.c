/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-16
 * Last update: 2019-10-25
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Decimal counter with data output on LCD display.
 * 
 * Note:
 *    Modified version of Peter Fleury's LCD library with R/W pin 
 *    connected to GND. Newline symbol "\n" is not implemented, use
 *    lcd_gotoxy() function instead.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>             // itoa() function
#include "timer.h"
#include "lcd.h"

uint8_t number = 0;
char buffer[9] = "";
/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Shows decimal values ​​on LCD display.
 *  Input:  None
 *  Return: None
 */
/*uint8_t znaky1[] = {0b10101010, 0b01010101, 0b10101010, 0b01010101,
0b10101010, 0b01010101,0b10101010, 0b01010101};
uint8_t znaky2[] = {0x04,
  0x00,
  0x17,
  0x12,
  0x01,
  0x0A,
  0x02,
  0x0C};*/
  uint8_t znaky[] = { 0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x14,
  0x14,
  0x14,
  0x14,
  0x14,
  0x14,
  0x14,
  0x14,
  0x15,
  0x15,
  0x15,
  0x15,
  0x15,
  0x15,
  0x15,
  0x15
  };


int main(void)
{
    /* LCD display
     * TODO: See Peter Fleury's online manual for LCD library 
     * http://homepage.hispeed.ch/peterfleury/avr-software.html
     * Initialize display and test different types of cursor */
    lcd_init(LCD_DISP_ON);

    // Display string without auto linefeed
//    lcd_puts("LCD testing");
    // TODO: Display variable value in decimal, binary, and hexadecimal
    lcd_gotoxy(0,0);
    lcd_puts("Counter");

     /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */

    TIM_config_prescaler(TIM1, TIM_PRESC_64);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
      



    /* TODO: Design at least two user characters and store them in 
     *       the display memory */
 lcd_command(1<<LCD_CGRAM);
  /*  lcd_data(0b10101);
    lcd_data(0b01010);
    lcd_data(0b10101);
    lcd_data(0b01010);
    lcd_data(0b10101);
    lcd_data(0b01010);
    lcd_data(0b10101);
    lcd_data(0b01010);*/

    for(int i = 0; i < 24; i++){
        lcd_data(znaky[i]);
    }

    
    lcd_gotoxy(13,0);
    lcd_putc(0x00); 
    lcd_putc(0x01); 
    lcd_putc(0x02); 



    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    for (;;) {
    }
    

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Increment counter value.
 */
ISR(TIMER1_OVF_vect)
{
    // TODO: Increment counter value form 0 to 255
    lcd_clrscr();
    if(number == 255){
        number = 0;
        lcd_clrscr();
        lcd_gotoxy(0,0);
        lcd_puts("Counter");
        
     
     }  
    number++;
    lcd_gotoxy(9,0);
    itoa(number,buffer,10);
    lcd_puts(buffer);

    lcd_gotoxy(0,1);
    uint8_t iterace = (uint8_t) number/16;
    for(int m = 0; m<iterace-1;m++){
        lcd_putc(0x02);
    }

    uint8_t nu = number - (iterace*16);
    if (nu < 5){
        lcd_gotoxy(iterace,1);
        lcd_putc(' ');
    }
    else if (nu > 5 && nu < 10){
        lcd_gotoxy(iterace,1);
        lcd_putc(0x00);
    }
    else if (nu > 10 && nu < 16){
        lcd_gotoxy(iterace,1);
        lcd_putc(0x01);
    }
    else {
        lcd_gotoxy(iterace,1);
        lcd_putc(0x02);}

    for(int m = 0; m<iterace-1;m++){
        lcd_gotoxy(0,1);
        lcd_putc(0x02);
    }

    /*for(int n = 0; n<iterace;n++){
        lcd_gotoxy(n,1);
        //uint8_t p = iterace/3;
        for(int p = iterace/3; p<iterace ;p++){
            if(p == 0){
                lcd_putc(0x00);
            }
             if(p == iterace/3){
                lcd_putc(0x01);
            }
             if(p == 2*iterace/3){
                lcd_putc(0x02);
                //p=0;
            }


        }




    }*/


    /*lcd_gotoxy(0,1);
    lcd_puts("0x");
    //lcd_gotoxy(2,1);
    itoa(number,buffer,16);
    lcd_puts(buffer);

    lcd_gotoxy(6,1);
    lcd_puts("0b");
    //lcd_gotoxy(8,1);
    itoa(number,buffer,2);
    lcd_puts(buffer);*/
}