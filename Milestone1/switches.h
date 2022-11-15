/*
 * switchs.h Built off of P.J Bones UCECE work from buttons4.h
 *
 *  Created on: 10/05/2022
 *      Author: Johne
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// Constants
//*****************************************************************************
enum swNames {SW1, SW2, NUM_SW};
enum swStates {SW_DOWN = 0, SW_UP};
// SWITCH 1 button
#define SW1_PERIPH      SYSCTL_PERIPH_GPIOA
#define SW1_PORT_BASE   GPIO_PORTA_BASE
#define SW1_PIN         GPIO_PIN_7
#define SW1_NORMAL      false
// SWITCH 2 button
#define SW2_PERIPH      SYSCTL_PERIPH_GPIOA
#define SW2_PORT_BASE   GPIO_PORTA_BASE
#define SW2_PIN         GPIO_PIN_6
#define SW2_NORMAL      false

#define NUM_SW_POLLS    3

void
initSwitches (void);

void
updateSwitches (void);

uint8_t
checkSwitches (uint8_t);

bool
switchUp (uint8_t);


#endif /* SWITCHES_H_ */
