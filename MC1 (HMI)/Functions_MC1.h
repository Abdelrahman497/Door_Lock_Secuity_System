#ifndef FUNCTIONS_MC1_H_
#define FUNCTIONS_MC1_H_
#include "lcd.h"
#include "uart.h"
#include "Timer.h"
#include "keypad.h"
#include <avr/io.h>
#include "std_types.h"
#include <util/delay.h>

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

extern uint8 pass[size] ; 		   		 /* array to save the password */
extern uint8 pass_check [size] ; 		 /* array to save the re_entered password */
extern uint8 Pass_Entered [size];		 /* array to save the entered password to open door */
extern uint8 key; 						 /* keypad button */


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Initializes hardware components LCD, UART */
void Initialise (void);

/*
 * Prompts the user to enter and re-enter a password using a keypad.
 * The password must consist of exactly 5 digits.
 */
void Create_Password (void);

/* Sends each byte of the entered password via UART.*/
void Send_Password (uint8 array[]);

/*
 * Receives confirmation of the password matching status via UART.
 * If the confirmation fails, prompts the user to create a new password.
 */
void Recive_Confirmation (void);

/*
 * Handles the process of opening the door by verifying the entered password
 * and managing the unlocking and locking sequence.
 */
void Open_Door (void);

/*
 * Prompts the user to enter a password using a keypad.
 * The entered password is masked on the display.
 */
void Enter_Pasword(void);

/*
 * Displays the unlocking status on the LCD and counts the duration.
 * It`s Call Back function for timer 1
 */
void Display_Unlocking_on_screen (void);

/*
 * Displays the locking status on the LCD and counts the duration.
 * It`s Call Back function for timer 1
 */
void Display_locking_on_screen (void);

/*
 * Handles the locking mechanism by counting down time and displaying status on the LCD.
 * It`s Call Back function for timer 1
 */
void Lock_Screen (void);

/*
 * Handles the process of changing the user's password.
 */
void Change_Password (void);


#endif /* FUNCTIONS_MC1_H_ */
