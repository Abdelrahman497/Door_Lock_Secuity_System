#ifndef FUNCTIONS_MC2_H_
#define FUNCTIONS_MC2_H_
#include "std_types.h"
#include "uart.h"
#include <avr/io.h>
#include "external_eeprom.h"
#include <util/delay.h>
#include "Timer0_PWM.h"
#include "Timer.h"
#include "Motor.h"
#include "PIR.h"
#include "Buzzer.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define SUCCESS		1
#define FAIL 		0
#define DONE 		2
#define WAIT		3
#define size 		5
#define OPEN		6
#define CHANGE 		7

/*******************************************************************************
 *                    Extern Global Variables                                  *
 *******************************************************************************/
extern uint8 pass_Recived[size] ; 		    	 /* array to save the password */
extern uint8 pass_check_Recived [size] ; 		 /* array to save the re_entered password */
extern uint8 Pass_Entered_Recived [size];		 /* array to save the entered password to open door */
extern uint8 Choose;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Initializes hardware components and communication protocols.
 * DC motor, buzzer, PIR sensor, UART, and TWI (I2C).
 */
void Initialise (void);

/* Receives a password via UART and stores it in the given array. */
void Recive_Password (uint8 array[]);

/*
 * Compares two passwords and sends the result via UART.
 * Repeats the process until the passwords match.
 */
void Check_Passwords_Match(void);

/* Saves the received password to EEPROM if passwords match. */
void Save_Password_EEPROM (void);

/*
 * Receives a password and checks it against the saved password.
 * If the password matches, it activates the motor and checks for PIR input.
 */
void Recive_Open_Door(void);

/*
 *  Rotates the motor clockwise for a specified duration.
 * It`s Call Back function for timer 1
 */
void Rotate_Motor_CW (void);

/*
 * Rotates the motor counterclockwise for a specified duration.
 * It`s Call Back function for timer 1
 */
void Rotate_Motor_ACW (void);

/*
 * Activates the buzzer for a specified duration.
 * It`s Call Back function for timer 1
 */
void Turn_Buzzer (void);

/*
 * Receives a new password and checks it against
 * the saved password in EEPROM.
 * If it matches, updates the password in EEPROM.
 */
void Recive_Change_Password (void);



#endif /* FUNCTIONS_MC2_H_ */
