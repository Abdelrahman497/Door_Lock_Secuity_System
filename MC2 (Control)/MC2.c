#include "Functions_MC2.h"
/*******************************************************************************
 *                              Main Functions                                 *
 *******************************************************************************/

int main (void)
{
	Initialise();
	Recive_Password(pass_Recived);
	Recive_Password(pass_check_Recived);
	Check_Passwords_Match ();
	Save_Password_EEPROM ();

	while (1)
	{
		Choose=UART_recieveByte();

		if(Choose == OPEN)
		{
			Recive_Open_Door();
		}
		else if (Choose == CHANGE)
		{
			Recive_Change_Password();
		}
	}
}

