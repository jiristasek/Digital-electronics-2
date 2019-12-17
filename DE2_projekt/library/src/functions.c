
#define F_CPU 16000000


/* Includes ----------------------------------------------------------*/
#include "uartextended.h"
#include "uart.h"
#include "functions.h"
#include <avr/io.h>
#include <util/delay.h>

/* Define ------------------------------------------------------------*/

void FUNC_sensorSendTriggerSignal(){
	PORTB |= _BV(TRIGGER_PIN);
	_delay_us(10);
	PORTB &= ~_BV(TRIGGER_PIN);
}

/*--------------------------------------------------------------------*/

uint16_t FUNC_mapNumber(uint16_t value, uint16_t inFromNumber, uint16_t inToNumber, uint16_t outFromNumber, uint16_t outToNumber){
	uint16_t outValue = (((value - inFromNumber) / (float) (inToNumber - inFromNumber)) * (outToNumber - outFromNumber)) + outFromNumber;
	return outValue;
}

/*--------------------------------------------------------------------*/

void FUNC_printBuffer(uint8_t start, uint8_t end, char* buffer){
	
	for (int i = start; i < end; i++){
		uart_putc(buffer[i]);
	}
}

