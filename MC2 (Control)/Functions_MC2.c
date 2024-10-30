#include "Functions_MC2.h"
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
uint8 pass_Recived[size] ; 		    	/* array to save the password */
uint8 pass_check_Recived [size] ; 		/* array to save the re_entered password */
uint8 Pass_Entered_Recived [size];	    /* array to save the entered password to open door */
uint8 saved_Password[size];				/* array to read the password frm EEPROM */
uint8 i = 0; 							/* counter For arrays Loops*/
uint8 match = 1;  						/* Flag to indicate if passwords match */
uint8 count = 0;						/* counter For timers*/
uint8 Attempt = 0;						/* counter to count number of attempts by user*/
uint8 PIR = 0;							/* Variable to store the received confirmation status */
uint8 Flag = 0; 						/* Flag for timers */
uint8 Choose = 0;						/* Choose which state open or change */

/*******************************************************************************
 *                          Functions Definitions                              *
 *******************************************************************************/

/*
 * Initializes hardware components and communication protocols.
 * DC motor, buzzer, PIR sensor, UART, and TWI (I2C).
 */
void Initialise (void)
{
	DcMotor_Init();
	Buzzer_init();
	PIR_init();

	/* Enable global interrupts by setting the 7th bit of the SREG register */
	SREG |= (1<<7);

	/* Configure UART settings:
	 * 8-bit data
	 * No parity
	 * 1 stop bit
	 * Baud rate: 9600
	 */
	UART_ConfigType ptr = {(uint8)EIGHT_BIT,(uint8)DISABLE,(uint8)ONE_BIT,(uint16)9600};
	UART_init(&ptr);

	/* Configure TWI (I2C) settings:
	 * Adress : 0b00000110
	 * Mode: FAST_MODE (set the I2C to fast mode)
	 */
	TWI_ConfigType TWI_Config = { 0b00000110 , FAST_MODE };
	TWI_init(&TWI_Config);

}

/* Receives a password via UART and stores it in the given array. */
void Recive_Password (uint8 array[])
{
	/*Loop to recive password sent by (HMI MCU) */
	for (i=0 ; i<size ; i++)
	{
		/* Receive a byte from UART and store it in the array */
		array[i] = UART_recieveByte();
	}
}


/*
 * Compares two passwords and sends the result via UART.
 * Repeats the process until the passwords match.
 */
void Check_Passwords_Match(void)
{
	do
	{
		match = 1;  /* Assume passwords match initially*/

		/* Loop to compare each byte in both passwords */
		for (i = 0; i < size; i++)
		{
			if (pass_Recived[i] != pass_check_Recived[i])
			{
				match = 0;  /* Set flag to indicate mismatch */
				break;      /* Exit loop on first mismatch */
			}
		}

		/* Send result based on match flag */
		if (match)
		{
			UART_sendByte(SUCCESS); 	 /* Send SUCCESS if all matched */
		}
		else
		{
			UART_sendByte(FAIL);   		 /* Send SUCCESS if all matched */
			Recive_Password(pass_Recived);
			Recive_Password(pass_check_Recived);

		}

	} while (!match);  /* Repeat until passwords match */
}

/* Saves the received password to EEPROM if passwords match. */
void Save_Password_EEPROM (void)
{
	if (match)  /* Check if passwords matched */
	{
		uint16 address = 0x0311;  /* Starting address for password storage */

		for (i = 0; i < size; i++)
		{
			EEPROM_writeByte(address, pass_Recived[i]);  /* Write each byte to EEPROM */
			address++;  /* Move to the next address for each byte */
			_delay_ms(10);
		}
		_delay_ms(10);
		/*Reset address to the starting point to read back the password */
		address = 0x0311;

		/* Read the password back from EEPROM into the new array */
		for (uint8 i = 0; i < size; i++,address++)
		{
			EEPROM_readByte(address, &saved_Password[i]); /* Read each byte from EEPROM */
		}
	}
}

/*
 * Receives a password and checks it against the saved password.
 * If the password matches, it activates the motor and checks for PIR input.
 */
void Recive_Open_Door(void)
{
	/* Receive the password entered by the user */
	Recive_Password (Pass_Entered_Recived);
	match = 1;

	/* Loop to compare each byte in both passwords */
	for (i = 0; i < size; i++)
	{
		if (saved_Password[i] != Pass_Entered_Recived[i])
		{
			match = 0;  /* Set flag to indicate mismatch */
			break;      /* Exit loop on first mismatch */
		}
	}

	/* Send result based on match flag */
	if (match)
	{
		UART_sendByte(SUCCESS); 	 /* Send SUCCESS if all matched */

		/* Initialize timer for motor rotation */
		Timer_ConfigType ptr = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
		Timer_init(&ptr);
		Timer_setCallBack( Rotate_Motor_CW , Timer_1);

		/* Wait for the timer to complete */
		while(!Flag);
		Flag=0;
		Attempt=0;

		/* Check the state of the PIR sensor */
		PIR = PIR_getState();
		if (PIR == 1)
		{
			UART_sendByte(SUCCESS); 	 /* Send SUCCESS if all matched */

			/* Wait until PIR no longer detects movement */
			while(PIR==1)
			{
				PIR = PIR_getState();
			}
			/* Indicate completion */
			UART_sendByte(DONE);
		}
		else
		{
			/* Send FAIL if no movement detected */
			UART_sendByte(FAIL);
		}

		/* Initialize timer for motor rotation in the opposite direction */
		Timer_ConfigType ptr2 = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
		Timer_init(&ptr2);
		Timer_setCallBack( Rotate_Motor_ACW , Timer_1);

		/* Wait for the timer to complete */
		while(!Flag);
		Flag=0;
	}
	else
	{
		UART_sendByte(FAIL);   		 /* Send Fail if all miss matched */
		Attempt++;					 /* Increment the attempt count */

		/* Check if the maximum attempts have been reached */
		if (Attempt == 3)
		{
			/* Initialize timer for buzzer alert */
			Timer_ConfigType ptr = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
			Timer_init(&ptr);
			Timer_setCallBack( Turn_Buzzer , Timer_1);

			/* Wait for the timer to complete */
			while(!Flag);
			Flag=0;
			Attempt=0;  /* Reset attempt count */
		}
	}
}

/*
 * Rotates the motor clockwise for a specified duration.
 * It`s Call Back function for timer 1
 */
void Rotate_Motor_CW (void)
{
	count++;	 /* Increment the rotation count */
	Flag=0;  	 /* Reset the flag */

	/* Rotate the motor clockwise at maximum duty cycle */
	DcMotor_Rotate(CW , MAX_DUTY_CYCLE_100);

	 /* Stop the motor after a specified count */
	if (count == 15)
	{
		DcMotor_Rotate(STOP , DUTY_CYCLE_0); /* Stop the motor */
		Timer_deInit(Timer_1); /* De-initialize the timer */
		count = 0; /* Reset count */
		Flag=1; /* Set flag to indicate completion */
	}
}

/*
 * Rotates the motor counterclockwise for a specified duration.
 * It`s Call Back function for timer 1
 */
void Rotate_Motor_ACW (void)
{
	count++;	/* Increment the rotation count */
	Flag=0;		/* Reset the flag */

    /* Rotate the motor counterclockwise at maximum duty cycle */
	DcMotor_Rotate(ACW , MAX_DUTY_CYCLE_100);

	/* Stop the motor after a specified count */
	if (count == 15)
	{
		DcMotor_Rotate(STOP , DUTY_CYCLE_0); /* Stop the motor */
		Timer_deInit(Timer_1); /* De-initialize the timer */
		count = 0; /* Reset count */
		Flag=1;	   /* Set flag to indicate completion */
	}
}

/*
 * Activates the buzzer for a specified duration.
 * It`s Call Back function for timer 1
 */
void Turn_Buzzer (void)
{
	count++; /* Increment the buzzer count */
	Flag=0;  /* Reset the flag */

	Buzzer_on(); /* Turn on the buzzer */

	 /* Turn off the buzzer after a specified count */
	if (count == 60)
	{
		Buzzer_off(); /* Turn off the buzzer */
		Timer_deInit(Timer_1); /* De-initialize the timer */
		count = 0; /* Reset count */
		Flag=1;  /* Set flag to indicate completion */
	}
}

/*
 * Receives a new password and checks it against
 * the saved password in EEPROM.
 * If it matches, updates the password in EEPROM.
 */
void Recive_Change_Password (void)
{
	 /* Receive the password entered by the user */
	Recive_Password (Pass_Entered_Recived);
	match = 1; /* Assume passwords match initially */

	/* Loop to compare each byte in both passwords */
	for (i = 0; i < size; i++)
	{
		if (saved_Password[i] != Pass_Entered_Recived[i])
		{
			match = 0;  /* Set flag to indicate mismatch */
			break;      /* Exit loop on first mismatch */
		}
	}

	/* Send result based on match flag */
	if (match)
	{
		UART_sendByte(SUCCESS); 	 /* Send SUCCESS if all matched */

		/* Receive new password and confirmation */
		Recive_Password(pass_Recived);
		Recive_Password(pass_check_Recived);

		 /* Check if new passwords match and save to EEPROM */
		Check_Passwords_Match ();
		Save_Password_EEPROM ();

		Attempt=0; /* Reset attempt count */
	}
	else
	{

		UART_sendByte(FAIL);   		 /* Send Fail if all miss matched */
		Attempt++;					 /* Increment the attempt count */

		/* Check if the maximum attempts have been reached */
		if (Attempt == 3)
		{
			/* Initialize timer for buzzer alert */
			Timer_ConfigType ptr = {0,3910,Timer_1,Prescale_1024,Compare_Mode};
			Timer_init(&ptr);
			Timer_setCallBack( Turn_Buzzer , Timer_1);

			/* Wait for the timer to complete */
			while(!Flag);
			Flag=0; /* Reset the flag */
		}
	}
}
