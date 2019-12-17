#ifndef _EEPROM_H_
#define _EEPROM_H_

/* Defines ---------------------------------------------------------  */

#define MEMORY_ADDRESS_MIN_DISTANCE_LOWER_BYTE 0x01
#define MEMORY_ADDRESS_MIN_DISTANCE_UPPER_BYTE 0x02
#define MEMORY_ADDRESS_MAX_DISTANCE_LOWER_BYTE 0x03
#define MEMORY_ADDRESS_MAX_DISTANCE_UPPER_BYTE 0x04
#define MEMORY_ADDRESS_SENSOR_TRIGGER_TIME_LOWER_BYTE 0x05
#define MEMORY_ADDRESS_SENSOR_TRIGGER_TIME_UPPER_BYTE 0x06
#define MEMORY_ADDRESS_BUZZER_ON_OFF_STATE 0x07
#define MEMORY_ADDRESS_BUZZER_TONE 0x08

#define MIN_DISTANCE 1
#define MAX_DISTANCE 2
#define SENSOR_TRIGGER 3
#define BUZZER_ON_OFF_STATE 4
#define BUZZER_TONE 5

/* Includes --------------------------------------------------------  */
#include  <avr/io.h>

/* Declarations ----------------------------------------------------  */
// Writes data to an address
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);

// Reads data from address
unsigned char EEPROM_read(unsigned int uiAddress);

// Reads specific data from EEPROM and coverts them to suit the variable in main.c
uint16_t EEPROM_getValue(uint8_t data);

// Converts and then writes data from main.c to the EEPROM.
void EEPROM_setValue(uint8_t address, uint16_t data);

#endif