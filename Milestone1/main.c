//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones  UCECE
// Last modified:   8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLEDInterface.h"
#include "readAcc.h"
#include "display.h"
#include "buttons4.h"
#include "refCal.h"
#include "buttonTasks.h"
#include "accData.h"
#include "switches.h"
#include "potentiometer.h"

#define SAMPLE_RATE_HZ      30
#define SYSTICKFREQ         1000
#define ACCTASKPERIOD       SYSTICKFREQ/30 //MHz over freq.
#define DISPLAYTASKPERIOD   SYSTICKFREQ/15
#define BUTTONTASKPERIOD    SYSTICKFREQ/100
#define ADCTASKPERIOD       SYSTICKFREQ/50
#define SCREENFREEZE        SYSTICKFREQ*3
#define SWTASKPERIOD        SYSTICKFREQ/100
#define DEFAULTGOAL         1000

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t accx_buffer, accy_buffer, accz_buffer;        // Buffer of size BUF_SIZE integers (sample values)
static unsigned long current_time;
static uint32_t clock_freq;


/***********************************************************
 * Initialization functions:
 ***********************************************************
 * Clock
 ***********************************************************/
void
SysTickIntHandler (void)
{
    current_time++;
}

void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    clock_freq = SysCtlClockGet ();
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet (clock_freq / SYSTICKFREQ);
    //
    // Register the interrupt handler
    SysTickIntRegister (SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable ();
    SysTickEnable ();
}


unsigned long
getCurrentTime (void)
{
    IntMasterDisable ();
    unsigned long time = current_time;
    IntMasterEnable ();
    return time;
}


void
initialiseAll (void)
{
    initClock ();
    initAccl ();
    initDisplay ();
    initADC ();

    initCircBuf (&accx_buffer, BUF_SIZE);
    initCircBuf (&accy_buffer, BUF_SIZE);
    initCircBuf (&accz_buffer, BUF_SIZE);

    initButtons ();
    initSwitches ();
}



int
main (void)
{
    initialiseAll();

    units cur_unit = RawKM; // Used for determining the current units.
    vector_xyz ave_raw = {ZERO, ZERO, ZERO}; // Is the current data from the accelerometer.
    acc_xyz acc_data = {accx_buffer, accy_buffer, accz_buffer}; // Are the buffers used to hold accelerometer data.
    unsigned long this_time, display_last_time, acc_last_time, adc_last_time, button_last_time, sw_last_time, goal_time; // Used for timings which are interrupt driving.
    angles ref_pr = getRefPR (getAcclData ()); // Current board orientation.
    bool complete = false;
    bool held = false;
    bool sw_up = false;
    bool goal_reached = false;
    bool goal_displaying = false;
    screen current_screen = Steps;
    int32_t steps = ZERO;
    int32_t goal = DEFAULTGOAL;
    int32_t display_goal = ZERO;

    IntMasterEnable ();

    while (1)
    {
        this_time = getCurrentTime ();
        // Gets the new ref point of the board.
        // And updates current screen value.
        if((this_time - button_last_time) >= BUTTONTASKPERIOD) {
            if (!goal_displaying && !sw_up) {
                buttonUnitTask (&cur_unit);
                buttonScreenTask (&current_screen);
                bool pushed = buttonDownHeld (false); // Used to determine when button is released.
                if (pushed) {
                    held = true; // Used to determine if at some point had held the button down.
                }
                if (!pushed && held) {
                    held = false;
                    if (current_screen == Goal) { // When on the goal screen.
                        goal = ADCreadingInSteps (goal);
                        if (goal < ZERO) {
                            goal = ZERO;
                        }
                        current_screen = Steps;
                        cur_unit = RawKM;
                    } else {
                        if (complete) {
                            steps = ZERO;
                        }
                    }
                }
            } else if (sw_up) { // Entered Test Mode
                cleanBottom ();
                loadingScreen (true);
                buttonScreenTask (&current_screen);
                buttonTestTask (&steps);
                buttonDownHeld (true);
            }
            button_last_time = this_time;
        }

        // Checks switches on the board and if to go into testing mode.
        if((this_time - sw_last_time) >= SWTASKPERIOD) {
            sw_up = switchUp (SW1);
            sw_last_time = this_time;
        }

        // Updates the acceleration of the board
        if((this_time - acc_last_time) >= ACCTASKPERIOD) {
            accTask (&ave_raw, &acc_data); //Logs current acceleration as it reads from buffer
            if(accIsStepping (ref_pr, ave_raw)) steps++;
            acc_last_time = this_time;
        }

        // Displays the board UI.
        if((this_time - display_last_time) >= DISPLAYTASKPERIOD) {
            if (sw_up) {
                displayScreenTask(current_screen, steps, goal, display_goal, cur_unit);
            } else if (((goal <= steps) && !goal_reached) || goal_displaying) {
                if (!goal_displaying) { // Ensures that the screen can return to normal.
                    goal_time = this_time;
                }
                goal_displaying = true;
                goal_reached = true;    // Used to stop buttons from being used and not allowing the screen to be shown again until the goal has be increased.
                displayGoalReached();
            } else {
                displayScreenTask (current_screen, steps, goal, display_goal, cur_unit);

                if (held && (current_screen != Goal)) {
                    complete = loadingScreen (false); // Updates the loading screen.
                } else { // Clears the loading screen.
                    cleanBottom ();
                    loadingScreen (true);
                }
            }
            display_last_time = this_time;
        }

        // Sets flag whether to un-freeze the screen or not.
        if (goal_reached && ((this_time - goal_time) >= SCREENFREEZE)) {
            goal_displaying = false; // Stops displaying the goal reached screen.
        }

        // Checks if the steps are below the goal and resets so the goal reached screen can be shown again.
        // Done here to reduce complexity.
        if (goal > steps) {
            goal_reached = false;
        }


        if((this_time - adc_last_time) >= ADCTASKPERIOD) {
            ADCProcessorTrigger (ADC0_BASE, SQE_NUM);
            display_goal = ADCreadingInSteps (goal);
            if (display_goal < ZERO) {
                display_goal = ZERO;
            }
            adc_last_time = this_time;
        }
    }
}

