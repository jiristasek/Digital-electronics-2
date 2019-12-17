#ifndef FUNCTIONS_H
#define FUNCTIONS_H



/* Includes ----------------------------------------------------------*/
#include <avr/io.h>

/* Define ------------------------------------------------------------*/
#define ECHO_PIN PB0
#define TRIGGER_PIN PB1

/* Declarations ----------------------------------------------------  */

/************************************************************************/
/*	Sends 10 uS trigger signal to sensor
	No inputs                                                           */
/************************************************************************/
void FUNC_sensorSendTriggerSignal();

/************************************************************************/
/*	Maps the number from range to another range
	Inputs : value - number we want to map
			inFromNumber - the bottom of the old interval               
			inToNumber - the top of the old interval
			outFromNumber - the bottom of the new interval
			outToNumber - the top of the new interval
	Output : uint16_t the initial value mapped to the new interval      */
/************************************************************************/
uint16_t FUNC_mapNumber(uint16_t value, uint16_t inFromNumber, uint16_t inToNumber, uint16_t outFromNumber, uint16_t outToNumber);

/************************************************************************/
/*	Prints content of the buffer to the UART
	Inputs : start - starting index we want to print. Usually 0
			 end - index of where we want to end               
			 buffer - buffer we want to print out of                    */
/************************************************************************/
void FUNC_printBuffer(uint8_t start, uint8_t end, char* buffer);

#endif