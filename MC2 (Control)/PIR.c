#include "PIR.h"
#include "gpio.h"
#include <avr/io.h>

/*******************************************************************************
 *                          Functions Definitions                              *
 *******************************************************************************/

/* Configures the pin connected to the PIR sensor*/
void PIR_init(void)
{
	/* Set the PIR sensor pin as input */
	GPIO_setupPinDirection(PIR_PORT_ID , PIR_PIN_ID , PIN_INPUT);
}

/* This function reads and returns the current state of the PIR sensor */
uint8  PIR_getState(void)
{
	uint8 value = 0;
	  /* Read the value from the PIR sensor pin */
	value = GPIO_readPin( PIR_PORT_ID , PIR_PIN_ID );

	return value;
}
