//Saved in folder
#include "EEPROM.h"
#include "timer.h"

#define DDR(x) (*(&x - 1))

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while (EECR & (1 << EEPE))
		;
	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1 << EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1 << EEPE);
}

/*--------------------------------------------------------------------*/

unsigned char EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while (EECR & (1 << EEPE))
		;
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1 << EERE);
	/* Return data from Data Register */
	return EEDR;
}

/*--------------------------------------------------------------------*/

uint16_t EEPROM_getValue(uint8_t data) {
	uint8_t upperReadEE = 0;
	uint8_t lowerReadEE = 0;
	switch (data) {
	case MIN_DISTANCE:
		upperReadEE = EEPROM_read(MEMORY_ADDRESS_MIN_DISTANCE_UPPER_BYTE);
		lowerReadEE = EEPROM_read(MEMORY_ADDRESS_MIN_DISTANCE_LOWER_BYTE);
		return ((upperReadEE << 8) + lowerReadEE);
	case MAX_DISTANCE:
		upperReadEE = EEPROM_read(MEMORY_ADDRESS_MAX_DISTANCE_UPPER_BYTE);
		lowerReadEE = EEPROM_read(MEMORY_ADDRESS_MAX_DISTANCE_LOWER_BYTE);
		return ((upperReadEE << 8) + lowerReadEE);
	case SENSOR_TRIGGER:
		upperReadEE = EEPROM_read(MEMORY_ADDRESS_SENSOR_TRIGGER_TIME_UPPER_BYTE);
		lowerReadEE = EEPROM_read(MEMORY_ADDRESS_SENSOR_TRIGGER_TIME_LOWER_BYTE);
		return ((upperReadEE << 8) + lowerReadEE);
	case BUZZER_ON_OFF_STATE:
		lowerReadEE = EEPROM_read(MEMORY_ADDRESS_BUZZER_ON_OFF_STATE);
		return lowerReadEE;
	case BUZZER_TONE:
		lowerReadEE = EEPROM_read(MEMORY_ADDRESS_BUZZER_TONE);
		switch (lowerReadEE) {
		case 1:
			return TIM_PRESC_8;
		case 3:
			return TIM_PRESC_64;
		case 4:
			return TIM_PRESC_128;
		case  5:
			return TIM_PRESC_256;
		case 6:
			return TIM_PRESC_1024;
		default: return 0;
		}
		return lowerReadEE;
	default:
		return 0;
	}
}

/*--------------------------------------------------------------------*/

void EEPROM_setValue(uint8_t address, uint16_t data) {
	uint8_t upperWriteEE = 0;
	uint8_t lowerWriteEE = 0;
	switch (address) {
	case MIN_DISTANCE:
		upperWriteEE = data >> 8;
		lowerWriteEE = data;
		EEPROM_write(MEMORY_ADDRESS_MIN_DISTANCE_LOWER_BYTE, lowerWriteEE);
		EEPROM_write(MEMORY_ADDRESS_MIN_DISTANCE_UPPER_BYTE, upperWriteEE);
		break;
	case MAX_DISTANCE:
		upperWriteEE = data >> 8;
		lowerWriteEE = data;
		EEPROM_write(MEMORY_ADDRESS_MAX_DISTANCE_LOWER_BYTE, lowerWriteEE);
		EEPROM_write(MEMORY_ADDRESS_MAX_DISTANCE_UPPER_BYTE, upperWriteEE);
		break;
	case SENSOR_TRIGGER:
		upperWriteEE = data >> 8;
		lowerWriteEE = data;
		EEPROM_write(MEMORY_ADDRESS_SENSOR_TRIGGER_TIME_LOWER_BYTE, lowerWriteEE);
		EEPROM_write(MEMORY_ADDRESS_SENSOR_TRIGGER_TIME_UPPER_BYTE, upperWriteEE);
		break;
	case BUZZER_ON_OFF_STATE:
		lowerWriteEE = data;
		EEPROM_write(MEMORY_ADDRESS_BUZZER_ON_OFF_STATE, lowerWriteEE);
		break;
	case BUZZER_TONE:
		switch (data) {
		case TIM_PRESC_8:
			lowerWriteEE = 1;
			break;
		case TIM_PRESC_64:
			lowerWriteEE = 3;
			break;
		case TIM_PRESC_128:
			lowerWriteEE = 4;
			break;
		case TIM_PRESC_256:
			lowerWriteEE = 5;
			break;
		case TIM_PRESC_1024:
			lowerWriteEE = 6;
			break;
		default:
			lowerWriteEE = 0;
			break;
		}
		EEPROM_write(MEMORY_ADDRESS_BUZZER_TONE, lowerWriteEE);
		break;
	default:
		break;
	}
}