#ifndef PIR_H_
#define PIR_H_
#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PIR_PORT_ID 		   PORTC_ID
#define PIR_PIN_ID             PIN2_ID


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Configures the pin connected to the PIR sensor*/
void PIR_init(void);

/* This function reads and returns the current state of the PIR sensor */
uint8  PIR_getState(void);

#endif /* PIR_H_ */
