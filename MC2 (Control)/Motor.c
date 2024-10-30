#include "Motor.h"
#include "gpio.h"
#include "Timer0_PWM.h"
#include <avr/io.h>


/*******************************************************************************
 *                          Functions Definitions                              *
 *******************************************************************************/

/* Initializes the DC motor */
void DcMotor_Init(void)
{
	GPIO_setupPinDirection(MOTOR_IN_PORT_ID,MOTOR_IN1_PIN_ID,PIN_OUTPUT) ;
	GPIO_setupPinDirection(MOTOR_IN_PORT_ID,MOTOR_IN2_PIN_ID,PIN_OUTPUT) ;

	/* Turn off the motor */
	GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
}

/* state The desired state of the motor (STOP, CW, ACW).
 * Controls the DC motor's rotation and speed.
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{

		PWM_Timer0_Start(speed);

		if (state == CW)
		{
			/* Turn on the motor clock wise direction */

			GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_HIGH);
			GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);

		}
		else if (state == ACW)
		{
			/* Turn on the motor anti clock wise direction */
			GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
			GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_HIGH);

		}
		else if (state == STOP)
		{
			/* Turn off the motor */
			GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
			GPIO_writePin(MOTOR_IN_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);

		}

}
