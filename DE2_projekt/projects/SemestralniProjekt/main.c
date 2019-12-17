/*
 * SemestralniProjekt.c
 *
 * Created: 3.12.2019 16:17:04
 * Authors : Radim Dvo¯·k, Ji¯Ì äù·sek
 *
 * Digital Electronics 2 Semestral project
 * 
 * 
 */ 

//Defines for UART menu navigation
#define MENU_MAIN 1
#define MENU_SELECT_MAX_OR_MIN 2
#define MENU_ENTER_MIN 3
#define MENU_ENTER_MAX 4
#define MENU_SELECT_TRIGGER_TIME 5
#define MENU_BUZZER_TONE 6
#define MENU_ENTER_MAX_DISTANCE_PRINT 7
#define MENU_ENTER_MIN_DISTANCE_PRINT 8
#define MENU_VALUE_UPDATED 9
#define MENU_VALUE_ERR 10

//Timing defines
#define F_CPU 16000000UL
#define UART_BAUD_RATE 9600
#define BUZZER_ON_TIME 192

//Defines for sensor states
#define SENSOR_READY 1
#define SENSOR_WAITING_FOR_ECHO 2
#define SENSOR_MEASURING 3

//Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include "timer.h"
#include "segment.h"
#include "EEPROM.h"
#include "functions.h"

 //Pin defines
#define ECHO_PIN PB0
#define TRIGGER_PIN PB1

//Functions declarations
uint16_t enterNumberInUart(uint8_t menuChoice);
void menuFunc(uint8_t choice);
void menuPrint(uint8_t choice);


//Declarations of variables
uint16_t measuredDistance = 0;
uint8_t sensorState = SENSOR_READY;
uint16_t timerOneOverflowCount = 0;
//Which number to display on seven segment display
uint8_t screenIndex = 0;
uint16_t timerZeroOverflowCount = 0;
//Distance at which the buzzer goes to a continuous tone
uint16_t minimumDistance = 0;
//Distance at which the buzzer stops beeping
uint16_t maximumDistance = 0;
//Period of the measurement cycle in milliseconds
uint16_t sensorTriggerTime = 0;
//Momentary state of the buzzer. Either beeping (1), not beeping(0) or completely off (2, buzzer will not beep even
//after surpassing the minimal distance)
uint8_t buzzerState = 0;
//"Beeping map" for buzzer beeping interval. Values represent time period for which the buzzer is not beeping.
//After this period the buzzer beeps for BUZZER_ON_TIME=48 milliseconds
uint16_t buzzerOffTime[] = {16,32,64,128,256,1024,1024,1024};
uint8_t sensorTriggerTimeCounter = 0;
//Defines the pitch/tone of the buzzer
uint8_t buzzerTone = TIM_PRESC_128;
uint8_t screenUpdateTimer = 0;
uint16_t buzzerUpdateTimer = 0;

//----------MAIN-------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
	//Initializes UART
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	
	//Configures pins as output for shift registers
	//DDRD = 0b01110000;
	DDRD |= _BV(PORTD4) | _BV(PORTD5) | _BV(PORTD6);
	
	//Configures pins for sensor
	//  TRIGGER - Output
	DDRB |= _BV(TRIGGER_PIN);
	//  ECHO - Input
	DDRB &= ~_BV(ECHO_PIN);
    
	//Configures pins for buzzer
	DDRB |= _BV(PORTB2) | _BV(PORTB3);
	PORTB |= _BV(PORTB2);
	
	//Enables external interrupt on ECHO pin
	PCICR |= _BV(PCIE0);
	PCMSK0 |= _BV(PCINT0);
	
	//Configure timers
	//Timer 0 configuration
	TIM_config_prescaler(TIM0, TIM_PRESC_256);
		//Sets CTC - Clear Timer on Compare
	TCCR0A = _BV(WGM01);
		//Every 1 ms
	OCR0A = 63;
	TIMSK0 = _BV(OCIE0A);
	
	//Timer 1 configuration
	TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
	
	//Timer 2 configuration
	TCCR2A = _BV(COM2A1) | _BV(WGM20) | _BV(WGM21);
	OCR2A = (50/100.0)*255.0;
	
	
	
	sei();
	
	buzzerState = EEPROM_getValue(BUZZER_ON_OFF_STATE);
	maximumDistance = EEPROM_getValue(MAX_DISTANCE);
	minimumDistance = EEPROM_getValue(MIN_DISTANCE);
	buzzerTone = EEPROM_getValue(BUZZER_TONE);
	sensorTriggerTime = EEPROM_getValue(SENSOR_TRIGGER);
	
	uart_clrscr();
	uart_puts("Press Enter to continue...");
	
    //Infinite Loop
	while (1) 
    {
		char c = uart_getc();
		if (c == '\r'){		//Enter
			while(1){
				menuFunc(MENU_MAIN);
			}
		} 
    }
	
}

//----------INTERRUPT ROUTINES-----------------------------------------------------------------------------------------------------------------
/*
* Timer 0 Interrupt Service Routine
* This should happen every ~ 1 millisecond
* Is the main timing handler of hardware other than the sensor e.g display and LEDs
*/
ISR(TIMER0_COMPA_vect){
	//Increment values used to time the events
	sensorTriggerTimeCounter++;
	screenUpdateTimer++;
	
	//If the buzzer is not disabled, increment value of timer
	if (buzzerState != 2) buzzerUpdateTimer++;
	
	
	//Updates seven segment screen every 4 milliseconds
	if(screenUpdateTimer == 4){
		screenUpdateTimer = 0;
		uint16_t LED_value = 0;
		if (measuredDistance >= maximumDistance) LED_value = 8;
		else if (measuredDistance <= minimumDistance) LED_value = 0;
		else LED_value = FUNC_mapNumber(measuredDistance, minimumDistance, maximumDistance, 1,8);
		SEG_putc(measuredDistance, screenIndex, LED_value);
		screenIndex++;
		if (screenIndex == 4) screenIndex = 0;
	}
	
	//If we surpassed time for trigger and the sensor is ready, then send a trigger signal to the sensor
	if ((sensorTriggerTimeCounter >= sensorTriggerTime) && (sensorState == SENSOR_READY)){
		sensorState = SENSOR_WAITING_FOR_ECHO;
		sensorTriggerTimeCounter = 0;
		FUNC_sensorSendTriggerSignal();
	}
	
	//Determines whether to turn the buzzer on or off
	//If measured distance is between the thresholds beep buzzer
	if (measuredDistance > minimumDistance && measuredDistance < maximumDistance){
		//If buzzerState is 2 (completely OFF) this will do nothing
		//If buzzer is currently beeping, check if it is time to turn it OFF according to define BUZZER_ON_TIME value
		if (buzzerState == 1){
			if (buzzerUpdateTimer >= BUZZER_ON_TIME){
				TIM_config_prescaler(TIM2, TIM_PRESC_STOP);
				buzzerUpdateTimer = 0;
				buzzerState = 0;
			}
		}
		//If buzzer is currently NOT beeping, check if it is time to turn it ON according to preset values in buzzerOffTime field variable
		if (buzzerState == 0){
			uint8_t buzzerOffTimeIndex = FUNC_mapNumber(measuredDistance, minimumDistance, maximumDistance, 1,8);
			if (buzzerUpdateTimer >= buzzerOffTime[buzzerOffTimeIndex]){
				TIM_config_prescaler(TIM2, buzzerTone);
				buzzerUpdateTimer = 0;
				buzzerState = 1;
			}
		}
	}
	
	//If measured distance is less than minimum set and the buzzer is allowed, then enter continuous sound mode
	else if (measuredDistance <= minimumDistance && buzzerState != 2){
		TIM_config_prescaler(TIM2, buzzerTone);
	}
	//If measured distance is greater than maximum set, turn the buzzer OFF
	else if (measuredDistance >= maximumDistance){
		TIM_config_prescaler(TIM2, TIM_PRESC_STOP);
	}
	
	
}
//---------------------------------------------------------------------------
/*
* Timer 1 Interrupt Service Routine
*/
ISR(TIMER1_OVF_vect){
	timerOneOverflowCount++;
}
//---------------------------------------------------------------------------
/*
* PCINT0 Interrupt Service Routine
* Handles measuring of the distance with sensor
*/
ISR(PCINT0_vect){
	
	//If we sent a trigger and we are waiting for an echo signal
	if (sensorState == SENSOR_WAITING_FOR_ECHO){
		timerOneOverflowCount = 0;
		sensorState = SENSOR_MEASURING;
		
		//Starts Timer/Counter 1
		TIM_config_prescaler(TIM1, TIM_PRESC_1);
		
	}
	
	//If we are waiting for the echo signal to go low again (I.e we are measuring / timer1 runs)
	else if (sensorState == SENSOR_MEASURING){
		
		//Stops Timer/Counter 1
		TIM_config_prescaler(TIM1, TIM_PRESC_STOP);
		
		//Calculates measured distance using formula (uS / 58 = centimeters)
		uint16_t tempNum = TCNT1;
		TCNT1 = 0;
		tempNum = ((tempNum + timerOneOverflowCount*65535) * 0.0625) / 58;
		measuredDistance = tempNum;
		sensorState = SENSOR_READY;
		
	}
}

//----------MENU FUNCTIONS--------------------------------------------------------------------------------------------------------------------

void menuFunc(uint8_t choice){
	char c;
	switch(choice){
		
		case MENU_MAIN:
		menuPrint(MENU_MAIN);
		while(1){
			c = uart_getc();
			if (c == '1'){
				menuFunc(MENU_SELECT_MAX_OR_MIN);
				break;
			}
			else if (c == '2'){
				menuFunc(MENU_SELECT_TRIGGER_TIME);
				break;
			}
			else if (c == '4'){
				if (buzzerState == 2) {
					buzzerState = 0;
					//timerZeroOverflowCount = 0;
				}
				else {
					buzzerState = 2;
					//timerZeroOverflowCount = 0;
					TIM_config_prescaler(TIM2, TIM_PRESC_STOP);
				}
				EEPROM_setValue(BUZZER_ON_OFF_STATE, buzzerState);
				break;
			}
			else if(c == '3'){
				menuFunc(MENU_BUZZER_TONE);
			}
			else if (c == 0x1b){ //Escape
				break;
			}
			
		}
		break;
		
		case MENU_SELECT_MAX_OR_MIN:
		menuPrint(MENU_SELECT_MAX_OR_MIN);
		while(1){
			c = uart_getc();
			if (c == '1'){
				menuFunc(MENU_ENTER_MAX);
				break;
			}
			else if (c == '2'){
				menuFunc(MENU_ENTER_MIN);
				break;
			}
			else if (c == 0x1b){ //Escape
				menuPrint(MENU_MAIN);
				break;
			}
			
		}
		break;
		
		case MENU_ENTER_MAX:
		menuPrint(MENU_ENTER_MAX);
		uint16_t value = enterNumberInUart(MENU_ENTER_MAX_DISTANCE_PRINT);
		if (value != 0) {
			maximumDistance = value;
			EEPROM_setValue(MAX_DISTANCE, maximumDistance);
			menuFunc(MENU_VALUE_UPDATED);
		}
		else menuFunc(MENU_VALUE_ERR);
		break;
		
		case MENU_ENTER_MIN:
		menuPrint(MENU_ENTER_MIN);
		value = enterNumberInUart(MENU_ENTER_MIN_DISTANCE_PRINT);
		if (value != 0) {
			minimumDistance = value;
			EEPROM_setValue(MIN_DISTANCE, minimumDistance);
			menuFunc(MENU_VALUE_UPDATED);
		}
		else menuFunc(MENU_VALUE_ERR);
		break;
		
		case MENU_VALUE_UPDATED:
		menuPrint(MENU_VALUE_UPDATED);
		while (uart_getc() != '\r'){}
		break;
		
		case MENU_VALUE_ERR:
		menuPrint(MENU_VALUE_ERR);
		while (uart_getc() != '\r'){}
		break;
		
		case MENU_SELECT_TRIGGER_TIME:
		menuPrint(MENU_SELECT_TRIGGER_TIME);
		value = enterNumberInUart(MENU_SELECT_TRIGGER_TIME);
		if (!(value < 60) && value != 0) {
			sensorTriggerTime = value;
			sensorTriggerTimeCounter = 0;
			EEPROM_setValue(SENSOR_TRIGGER, sensorTriggerTime);
			menuFunc(MENU_VALUE_UPDATED);
		}
		else menuFunc(MENU_VALUE_ERR);
		menuPrint(MENU_MAIN);
		break;
		case MENU_BUZZER_TONE:
		menuPrint(MENU_BUZZER_TONE);
		while(1){
			c = uart_getc();
			if (c == '1'){
				buzzerTone = TIM_PRESC_1024;
				EEPROM_setValue(BUZZER_TONE, buzzerTone);
				menuFunc(MENU_VALUE_UPDATED);
				break;
			}
			else if (c == '2'){
				buzzerTone = TIM_PRESC_256;
				EEPROM_setValue(BUZZER_TONE, buzzerTone);
				menuFunc(MENU_VALUE_UPDATED);
				break;
			}
			else if (c == '3'){
				buzzerTone = TIM_PRESC_128;
				EEPROM_setValue(BUZZER_TONE, buzzerTone);
				menuFunc(MENU_VALUE_UPDATED);
				break;
			}
			else if (c == '4'){
				buzzerTone = TIM_PRESC_64;
				EEPROM_setValue(BUZZER_TONE, buzzerTone);
				menuFunc(MENU_VALUE_UPDATED);
				break;
			}
			else if (c == '5'){
				buzzerTone = TIM_PRESC_8;
				EEPROM_setValue(BUZZER_TONE, buzzerTone);
				menuFunc(MENU_VALUE_UPDATED);
				break;
			}
			else if (c == 0x1b){ //Escape
				menuPrint(MENU_MAIN);
				break;
			}
			
		}
		menuPrint(MENU_MAIN);
		break;
	}
}

//---------------------------------------------------------------------------

void menuPrint(uint8_t choice){
	switch(choice){
		
		case MENU_MAIN:
		uart_clrscr();
		uart_puts("Main Menu\r\n\r\n");
		uart_puts("1 - Set distance thresholds\r\n");
		uart_puts("2 - Set sensor trigger time\r\n");
		uart_puts("3 - Set buzzer tone\r\n");
		uart_puts("4 - Toggle buzzer ON/OFF\r\n\r\n");
		uart_puts("----------------------------\r\n\r\n");
		uart_puts("Current settings:\r\n");
		if (buzzerState == 2) uart_puts("Buzzer ON/\033[4;31mOFF\033[0m\r\n");
		else uart_puts("Buzzer \033[4;32mON\033[0m/OFF\r\n");
		uart_puts("Buzzer tone: ");
		switch (buzzerTone){
			case TIM_PRESC_8:
			uart_puts("VERY HIGH\r\n");
			break;
			case TIM_PRESC_64:
			uart_puts("HIGH\r\n");
			break;
			case TIM_PRESC_128:
			uart_puts("MEDIUM\r\n");
			break;
			case TIM_PRESC_256:
			uart_puts("LOW\r\n");
			break;
			case TIM_PRESC_1024:
			uart_puts("VERY LOW\r\n");
			break;
		}
		uart_puts("Minimum distance: ");
		uart_putnum(minimumDistance,10);
		uart_puts(" cm");
		uart_puts("\r\nMaximum distance: ");
		uart_putnum(maximumDistance,10);
		uart_puts(" cm");
		uart_puts("\r\nSensor trigger interval: ");
		uart_putnum(sensorTriggerTime,10);
		uart_puts(" ms");
		break;
		
		case MENU_SELECT_MAX_OR_MIN:
		uart_clrscr();
		uart_puts("Select which distance you would like to edit\r\n\r\n");
		uart_puts("1 - Maximum distance\r\n");
		uart_puts("2 - Minimum distance\r\n");
		break;
		
		case MENU_ENTER_MAX:
		uart_clrscr();
		break;
		
		case MENU_ENTER_MIN:
		uart_clrscr();
		break;
		
		case MENU_ENTER_MAX_DISTANCE_PRINT:
		uart_clrscr();
		uart_puts("Type your value in centimeters. Min 1 cm. ESC to quit. Enter to confirm\r\n\r\n");
		uart_puts("Current Value: ");
		uart_putnum(maximumDistance,10);
		uart_puts("\r\nNew Value: ");
		break;
		
		case MENU_ENTER_MIN_DISTANCE_PRINT:
		uart_clrscr();
		uart_puts("Type your value in centimeters. Min 1 cm. ESC to quit. Enter to confirm\r\n\r\n");
		uart_puts("Current Value: ");
		uart_putnum(minimumDistance,10);
		uart_puts("\r\nNew Value: ");
		break;
		
		case MENU_VALUE_UPDATED:
		uart_clrscr();
		uart_puts("Value has been \033[4;32msuccesfuly\033[0m updated. \r\n\r\nPress Enter to continue...\r\n\r\n");
		break;
		
		case MENU_VALUE_ERR:
		uart_clrscr();
		uart_puts("Ooops! Something went wrong. Value was \033[4;31mNOT updated\033[0m!\r\nThe update either \033[4;31mfailed\033[0m or you entered number \033[4;31mout of range\033[0m! \r\n\r\nPress Enter to continue...\r\n\r\n");
		break;
		
		case MENU_SELECT_TRIGGER_TIME:
		uart_clrscr();
		uart_puts("Type your value in milliseconds. Minimum is 60 ms due to sensor specs.\r\nESC to quit. Enter to confirm\r\n\r\n");
		uart_puts("Current Value: ");
		uart_putnum(sensorTriggerTime,10);
		uart_puts("\r\nNew Value: ");
		break;
		case MENU_BUZZER_TONE:
		uart_clrscr();
		uart_puts("Select buzzer tone from the following options\r\n\r\n");
		uart_puts("1 - VERY LOW\r\n");
		uart_puts("2 - LOW\r\n");
		uart_puts("3 - MEDIUM - default (recommended)\r\n");
		uart_puts("4 - HIGH\r\n");
		uart_puts("5 - VERY HIGH\r\n");
		break;
	}
}

//---------------------------------------------------------------------------

uint16_t enterNumberInUart(uint8_t menuChoice){
	char buffer[16] = "";
	uint8_t bufferPointer = 0;
	
	menuPrint(menuChoice);
	
	while(1){
		char c = uart_getc();
		if (c >= '0' && c <= '9'){ //0-9
			buffer[bufferPointer++] = c;
			menuPrint(menuChoice);
			FUNC_printBuffer(0, bufferPointer, buffer);
		}
		
		else if (c == 0x08){ //BackSpace
			buffer[--bufferPointer] = '\0';
			menuPrint(menuChoice);
			FUNC_printBuffer(0, bufferPointer, buffer);
		}
		
		else if (c == '\r'){ //Enter
			return atoi(buffer);
		}
		
		else if (c == 0x1b){ //Escape
			return 0;
		}
	}
	
}
