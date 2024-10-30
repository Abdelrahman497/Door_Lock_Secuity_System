#include "Functions_MC1.h"
/*******************************************************************************
 *                              Main Functions                                 *
 *******************************************************************************/

int main (void)
{
	Initialise();
	Create_Password ();
	Send_Password (pass);
	Send_Password (pass_check);
	Recive_Confirmation();

	while (1)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0," + : Open Door");
		LCD_displayStringRowColumn(1,0," - : Change Pass");
		key = KEYPAD_getPressedKey();

		if(key == '+')
		{
			UART_sendByte(OPEN);
			Open_Door ();
		}
		else if (key == '-')
		{
			UART_sendByte(CHANGE);
			Change_Password ();
		}
	}
}
