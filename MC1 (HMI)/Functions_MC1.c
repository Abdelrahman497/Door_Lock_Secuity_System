#include "Functions_MC1.h"
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

uint8 pass[size] ; 		   		 /* array to save the password */
uint8 pass_check [size] ; 		 /* array to save the re_entered password */
uint8 Pass_Entered [size];		 /* array to save the entered password to open door */
uint8 key = 0; 					 /* keypad button */
uint8 i = 0; 				 	 /* counter For arrays Loops*/
uint8 count = 0;				 /* counter For timers*/
uint8 Attempt = 0;				 /* counter to count number of attempts by user*/
uint8 Minute =60; 				 /* variable for Minutes  */
uint8 Flag = 0;  				 /* Flag for timers */

/*******************************************************************************
 *                          Functions Definitions                              *
 *******************************************************************************/

/* Initializes hardware components LCD, UART */
void Initialise (void)
{
	/* Initialize the LCD */
	LCD_init();

	/* Enable global interrupts */
	SREG |= (1<<7);

	/* Configure UART settings:
	 * 8-bit data
	 * No parity
	 * 1 stop bit
	 * Baud rate: 9600
	 */
	UART_ConfigType ptr = {(uint8)EIGHT_BIT,(uint8)DISABLE,(uint8)ONE_BIT,(uint16)9600};
	UART_init(&ptr);

	/* Display welcome messages on the LCD */
	LCD_displayStringRowColumn(0,2,"Door Locker");
	LCD_displayStringRowColumn(1,0,"Security System");

	_delay_ms(1000);	/* Wait for 1 second */
	LCD_clearScreen();  /* Clear the LCD screen */
}

/*
 * Prompts the user to enter and re-enter a password using a keypad.
 * The password must consist of exactly 5 digits.
 */
void Create_Password (void)
{
	LCD_displayStringRowColumn(0,0,"Plz enter pass: ");

	/* Loop to scan password entered by the user.
	 * Loop continues until user presses 'Enter' ('=').
	 * and password contains 5 digits.
	 */
	i=0; /*reset counter*/
	while (1)
	{
		key = KEYPAD_getPressedKey();

		/* Check if key is a number (0-9) */
		if (key >= 0 && key <= 9 && i < size)
		{
			pass[i] = key; 		/* Store the number of key in pass array */
			LCD_displayStringRowColumn(1, i, "*"); /* Display '*' for privacy */
			i++;
			_delay_ms(200);		 /* Handl pressig on keypad */
		}
		else if (key == '=' && i == size)
		{
			/* Exit the loop if the password is complete */
			break;
		}
	}
	LCD_clearScreen(); /* Clear screen to re enter the password */

	/* Prompt user to re-enter the same password */
	LCD_displayStringRowColumn(0,0,"Plz re-enter the");
	LCD_displayStringRowColumn(1,0,"same pass:");

	/* Loop to scan the re-entered password */
	i=0; /*reset counter*/
	while (1)
	{
		key = KEYPAD_getPressedKey();

		/* Check if key is a number (0-9) */
		if (key >= 0 && key <= 9 && i < size)
		{
			pass_check[i] = key; 		/* Store the number of key in pass_check array */
			LCD_displayStringRowColumn(1, 10+i , "*"); /* Display '*' for privacy */
			i++;
			_delay_ms(200);		 /* Handl pressig on keypad */
		}
		else if (key == '=' && i == size )
		{
			/* Exit the loop if the password is complete */
			break;
		}
	}
}

/* Sends each byte of the entered password via UART.*/
void Send_Password (uint8 array[])
{
	/* Loop to send each byte for entered password */
	for (i=0 ; i<size ; i++)
	{
		/* Send each byte via UART */
		UART_sendByte(array[i]);
	}
}

/*
 * Receives confirmation of the password matching status via UART.
 * If the confirmation fails, prompts the user to create a new password.
 */
void Recive_Confirmation (void)
{
	uint8 recive ; /* Variable to store the received confirmation status */

	/* Receive the confirmation status via UART */
	recive = UART_recieveByte();

	/* If the received status is FAIL, the passwords do not match */
	if(recive == FAIL)
	{
		LCD_clearScreen(); /* Clear the screen */
		LCD_displayStringRowColumn(0,0,"Fail : Not match"); /* Display failure message */
		_delay_ms(500);  /* Wait for a moment */

		/* Prompt the user to create a new password */
		Create_Password ();
		Send_Password (pass);
		Send_Password (pass_check);

		/* Recursively call to receive confirmation again */
		Recive_Confirmation();
	}

	/* If the received status is SUCCESS, the passwords match */
	else if ( recive == SUCCESS )
	{
		LCD_clearScreen();  /* Clear the screen */
		LCD_displayStringRowColumn(0,4,"SUCCESS"); /* Display success message */
		_delay_ms(500); /* Wait for a moment */
	}
}

/*
 * Handles the process of opening the door by verifying the entered password
 * and managing the unlocking and locking sequence.
 */
void Open_Door (void)
{
	uint8 recive , PIR ; /* Variables to store received status and PIR state */

	/* Prompt user to enter the password */
	Enter_Pasword();

	/* Send the entered password via UART */
	Send_Password (Pass_Entered);

	/* Receive confirmation of password correctness */
	recive = UART_recieveByte();

	if ( recive == SUCCESS )
	{
		/* If password is correct, initialize timer for unlocking sequence */
		Timer_ConfigType ptr = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
		Timer_init(&ptr);
		Timer_setCallBack( Display_Unlocking_on_screen , Timer_1);

		/* Clear the LCD and display unlocking message */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Door Unlocking");
		Attempt=0; /* Reset the attempt counter */

		/* Wait until the timer callback is executed */
		while(!Flag);
		Flag=0; /* Reset the flag for the next use */

		/* Receive another confirmation after unlocking */
		recive = UART_recieveByte();
		if(recive == SUCCESS)
		{
			/* If still successful, prompt to wait for people to enter */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Wait for people");
			LCD_displayStringRowColumn(1,3,"To Enter");

			/* Wait for PIR sensor signal */
			PIR = UART_recieveByte();
			while (PIR != DONE)
			{
				PIR = UART_recieveByte();  /* Keep checking for DONE status */
			}
		}

		/* Initialize timer for locking sequence */
		Timer_ConfigType ptr2 = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
		Timer_init(&ptr2);
		Timer_setCallBack( Display_locking_on_screen , Timer_1);

		/* Clear the LCD and display locking message */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Door Locking");

		/* Wait until the timer callback is executed */
		while(!Flag);
		Flag=0; /* Reset the flag for the next use */
	}

	else if(recive == FAIL)
	{
		/* If password is incorrect, increment the attempt counter */
		Attempt++;
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Wrong Pass");
		LCD_displayStringRowColumn(1,0,"Attempts Left ");
		LCD_intgerToString(3-Attempt); /* Display remaining attempts */
		_delay_ms(500); /* Short delay for user visibility */

		if (Attempt == 3)
		{
			/* If three incorrect succisive attempts, lock the system */
			Timer_ConfigType ptr = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
			Timer_init(&ptr);
			Timer_setCallBack( Lock_Screen , Timer_1);

			/* Clear the LCD and display locked message */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Locked....");

			/* Wait until the timer callback is executed */
			while(!Flag);
			Flag=0;    /* Reset the flag for the next use */
			Attempt=0; /* Reset the attempt counter */
		}
	}
}


/*
 * Prompts the user to enter a password using a keypad.
 * The entered password is masked on the display.
 */
void Enter_Pasword(void)
{
	LCD_clearScreen();  /* Clear the LCD screen */
	LCD_displayStringRowColumn(0,0,"Enter your pass: "); /* Display prompt for password entry */

	i=0; /* Reset counter for password length */

	while (1)
	{
		key = KEYPAD_getPressedKey(); /* Get the key pressed on the keypad */

		/* Check if key is a number (0-9) */
		if (key >= 0 && key <= 9 && i < size)
		{
			Pass_Entered[i] = key; 		/* Store the number of key in pass_entered array */
			LCD_displayStringRowColumn(1, i, "*"); /* Display '*' for privacy */
			i++;  /* Increment the counter */
			_delay_ms(200);	/* Handl pressig on keypad */
		}
		/* Check if the 'Enter' key ('=') is pressed and the password is fully entered */
		else if (key == '=' && i == size)
		{
			/* Exit the loop when the correct condition is met */
			break;
		}
	}
}

/*
 * Displays the unlocking status on the LCD and counts the duration.
 * It`s Call Back function for timer 1
 */
void Display_Unlocking_on_screen (void)
{
	count++; /* Increment the count for each timer tick */
	Flag=0;  /* Reset the flag for this callback */

	/* Display unlocking messages on the LCD */
	LCD_displayStringRowColumn(0,0,"Door Unlocking");
	LCD_displayStringRowColumn(1,0,"Counting ");
	LCD_intgerToString(count);  /* Display the current count on the screen */

	/* Check if the count has reached the threshold */
	if (count >= 15)
	{
		Timer_deInit(Timer_1); /* Stop the timer */
		count = 0; /* Reset count */
		Flag = 1;  /* Set the flag to indicate the completion of the unlocking process */
	}
}

/*
 * Displays the locking status on the LCD and counts the duration.
 * It`s Call Back function for timer 1
 */
void Display_locking_on_screen (void)
{
	count++; /* Increment the count for each timer tick */
	Flag=0;  /* Reset the flag for this callback */

	/* Display locking messages on the LCD */
	LCD_displayStringRowColumn(0,0,"Door Locking");
	LCD_displayStringRowColumn(1,0,"Counting ");
	LCD_intgerToString(count); /* Display the current count on the screen */

	/* Check if the count has reached the threshold */
	if (count >= 15)
	{
		Timer_deInit(Timer_1); /* Stop the timer */
		count = 0; /* Reset count */
		Flag = 1;  /* Set the flag to indicate the completion of the locking process */
	}
}

/*
 * Handles the locking mechanism by counting down time and displaying status on the LCD.
 * It`s Call Back function for timer 1
 */
void Lock_Screen (void)
{
	count++;  /* Increment the count for each timer tick */
	Minute--;  /* Decrement the minute counter */
	Flag = 0;  /* Reset the flag for this callback */

	/* Display the locked status and countdown on the LCD */
	LCD_displayStringRowColumn(0,0,"Locked....");
	LCD_displayStringRowColumn(1,0,"Counting ");

	/* Display the remaining time */
	if(Minute >= 100)
	{
		LCD_intgerToString(Minute);
	}
	else
	{
		LCD_intgerToString(Minute);
		/* In case the digital value is two or one digits print space in the next digit place */
		LCD_displayCharacter(' ');
	}

	/* Check if the count has reached the locking threshold (60 seconds) */
	if (count == 60)
	{
		Timer_deInit(Timer_1); /* Stop the timer */
		count = 0; /* Reset count */
		Minute = 60; /* Reset minutes for the next lock period */
		Flag = 1;  /* Set the flag to indicate that the locking period is complete */


		LCD_clearScreen(); /* Clear the screen for the next message */
		LCD_displayStringRowColumn(0, 0, "Trying again Now"); /* Notify user to try again */
		_delay_ms(500); /* Wait for half a second before returning */
	}
}

/*
 * Handles the process of changing the user's password.
 */
void Change_Password (void)
{
	uint8 recive; 	 /* Variable to store the response from UART */

	/* Prompt user to enter their current password */
	Enter_Pasword();
	Send_Password (Pass_Entered); /* Send the entered password to the system */

	recive = UART_recieveByte();  /* Receive confirmation of the password */

	if ( recive == SUCCESS )
	{
		/* If password is correct, proceed with changing the password */
		LCD_clearScreen(); /* Clear the LCD screen */
		LCD_displayStringRowColumn(0, 0, "Changing Pass"); /* Display status message */
		_delay_ms(500); /* Delay for user to see the message */
		LCD_clearScreen();  /* Clear the screen again */

		/* Prompt the user to create a new password */
		Create_Password();
		Send_Password (pass);
		Send_Password (pass_check);
		Recive_Confirmation();

		/* Reset attempt counter */
		Attempt = 0;
	}

	else if(recive == FAIL)
	{

		/* If password is incorrect, increment attempt counter */
		Attempt++;
		LCD_clearScreen(); /* Clear the LCD screen */
		LCD_displayStringRowColumn(0,0,"Wrong Pass");
		LCD_displayStringRowColumn(1,0,"Attempts Left ");

		LCD_intgerToString(3-Attempt);
		_delay_ms(500);

		/* Lock the system after 3 failed attempts */
		if (Attempt == 3)
		{
			/* If three incorrect succisive attempts, lock the system */
			Timer_ConfigType ptr = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
			Timer_init(&ptr);
			Timer_setCallBack( Lock_Screen , Timer_1);

			LCD_clearScreen();  /* Clear the LCD screen */
			LCD_displayStringRowColumn(0,0,"Locked....");

			/* Wait until the timer callback sets the flag */
			while(!Flag);
			Flag=0;     /* Reset the flag */
			Attempt=0;  /* Reset the attempt counter */
		}
	}
}
