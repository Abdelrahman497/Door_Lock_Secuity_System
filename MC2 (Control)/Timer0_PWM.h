#ifndef TIMER0_PWM_H_
#define TIMER0_PWM_H_


#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MAX_DUTY_CYCLE_100  255
#define DUTY_CYCLE_75       192
#define DUTY_CYCLE_50       128
#define DUTY_CYCLE_25       64
#define DUTY_CYCLE_0        0

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

void PWM_Timer0_Start(uint8 duty_cycle);


#endif /* TIMER0_PWM_H_ */
