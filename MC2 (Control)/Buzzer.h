#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define BUZZER_PORT_ID         PORTC_ID
#define BUZZER_PIN_ID          PIN7_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Function to initialize the BUZZER pin as output */
void Buzzer_init(void);

/* Function to turn on BUZZER */
void Buzzer_on(void);

/* Function to turn off BUZZER */
void Buzzer_off(void);

#endif /* BUZZER_H_ */
