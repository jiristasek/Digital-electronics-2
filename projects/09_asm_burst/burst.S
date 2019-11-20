#include <avr/io.h>
#define __SFR_OFFSET    0
#define result  r16
#define temp    r17
#define input   r24


.global burst_asm
burst_asm:
    
    sbi PORTB, 5
    cbi PORTB, 5   
    rjmp burst_asm
    ret
    