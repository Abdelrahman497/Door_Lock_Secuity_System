#include "Buzzer.h"
#include "gpio.h"
#include <avr/io.h>

/*******************************************************************************
 *                          Functions Definitions                              *
 *******************************************************************************/

/* Function to initialize the BUZZER pin as output */
void Buzzer_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID , BUZZER_PIN_ID , PIN_OUTPUT);
}

/* Function to turn on BUZZER */
void Buzzer_on(void)
{
	GPIO_writePin( BUZZER_PORT_ID ,  BUZZER_PIN_ID , LOGIC_HIGH);
}

/* Function to turn off BUZZER */
void Buzzer_off(void)
{
	GPIO_writePin( BUZZER_PORT_ID , BUZZER_PIN_ID , LOGIC_LOW);
}


