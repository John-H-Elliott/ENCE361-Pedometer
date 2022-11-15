/*
 * switchs.c Built off of P.J Bones UCECE work from buttons4.h
 *
 *  Created on: 10/05/2022
 *      Author: Johne
 */

#include <stdint.h>
#include <stdbool.h>
#include "switches.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "display.h"


// *******************************************************
// Globals to module
// *******************************************************
static bool sw_state[NUM_SW];    // Corresponds to the electrical state
static uint8_t sw_count[NUM_SW];
static bool sw_normal[NUM_SW];   // Corresponds to the electrical state


void
initSwitches (void)
{
    int i;

    // SW1 (active HIGH)
    SysCtlPeripheralEnable (SW1_PERIPH);
    GPIOPinTypeGPIOInput (SW1_PORT_BASE, SW1_PIN);
    GPIOPadConfigSet (SW1_PORT_BASE, SW1_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    sw_normal[SW1] = SW1_NORMAL;
    // SW2 (active HIGH)
    SysCtlPeripheralEnable (SW2_PERIPH);
    GPIOPinTypeGPIOInput (SW2_PORT_BASE, SW2_PIN);
    GPIOPadConfigSet (SW2_PORT_BASE, SW2_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    sw_normal[SW2] = SW2_NORMAL;


    for (i = ZERO; i < NUM_SW; i++)
    {
        sw_state[i] = sw_normal[i];
        sw_count[i] = ZERO;
    }
}


void
updateSwitches (void)
{
    bool sw_value[NUM_SW];
    int i;

    // Read the pins; true means HIGH, false means LOW
    sw_value[SW1] = (GPIOPinRead (SW1_PORT_BASE, SW1_PIN) == SW1_PIN);
    sw_value[SW2] = (GPIOPinRead (SW2_PORT_BASE, SW2_PIN) == SW2_PIN);
    // Iterate through the buttons, updating button variables as required
    for (i = ZERO; i < NUM_SW; i++)
    {
        if (sw_value[i] != sw_state[i])
        {
            sw_count[i]++;
            if (sw_count[i] >= NUM_SW_POLLS)
            {
                sw_state[i] = sw_value[i];
                sw_count[i] = ZERO;
            }
        }
        else
            sw_count[i] = ZERO;
    }
}


uint8_t
checkSwitch (uint8_t sw_name)
{
    if (sw_state[sw_name] == sw_normal[sw_name]){
        return SW_DOWN;
    } else {
        return SW_UP;
    }
}


bool
switchUp (uint8_t sw_name) {
    updateSwitches();
    if (checkSwitch(sw_name) == SW_UP) {
        return true;
    } else {
        return false;
    }
}


