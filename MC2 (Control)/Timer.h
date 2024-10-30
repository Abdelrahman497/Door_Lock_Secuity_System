#ifndef TIMER_H_
#define TIMER_H_
#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
typedef enum
{
	Timer_0 , Timer_1 , Timer_2

}Timer_ID_Type;

typedef enum
{
	 No_Clock,
	 Prescale_1,
	 Prescaler_8,
	 Prescale_64,
	 Prescaler_256,
	 Prescale_1024,
	 External_Clock_Falling_Edge,
	 External_Clock_Raising_Edge

}Timer_ClockType;

typedef enum
{
	Normal_Mode,
	Compare_Mode

}Timer_ModeType;

typedef struct
{

uint16 timer_InitialValue;
uint16 timer_compare_MatchValue;     /*it will be used in compare mode only*/
Timer_ID_Type  timer_ID;
Timer_ClockType timer_clock;
Timer_ModeType  timer_mode;

}Timer_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*  Initializes the timer with the specified settings. */
void Timer_init(const Timer_ConfigType * Config_Ptr);

/* Disables the specified timer and resets its settings. */
void Timer_deInit(Timer_ID_Type timer_type);

/* Sets a callback function for the specified timer's interrupt. */
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID );




#endif /* TIMER_H_ */
