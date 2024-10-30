#ifndef MOTOR_H_
#define MOTOR_H_
#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	CW,ACW,STOP

}DcMotor_State;

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MOTOR_IN_PORT_ID         PORTD_ID
#define MOTOR_IN1_PIN_ID         PIN6_ID
#define MOTOR_IN2_PIN_ID         PIN7_ID
#define MOTOR_EN_PORT_ID         PORTB_ID
#define MOTOR_EN_PIN_ID          PIN3_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/* Initializes the DC motor */
void DcMotor_Init(void);

/* state The desired state of the motor (STOP, CW, ACW).
 * Controls the DC motor's rotation and speed.
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed);


#endif /* MOTOR_H_ */
