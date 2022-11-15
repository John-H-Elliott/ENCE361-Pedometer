// *******************************************************
//
// potentiomemter.c
//
// Support for setting up the ADC device using simple commands
// J. Tasker
// Created: 12/05/2022
//
// *******************************************************

#include <stdint.h>
#include <stdbool.h>
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
#include "display.h"
#include "potentiometer.h"


// SAMPLE RATE IS THE SAME AS CLOCK TICK RATE

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)

void
ADCTask (void)
{
    uint32_t ulValue;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet (ADC0_BASE, SQE_NUM, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear (ADC0_BASE, SQE_NUM);
}

//Returns 0-4095
int32_t
ADCgetReading (void)
{
    int32_t sum = ZERO;
    int32_t i = ZERO;
    for (i = ZERO; i < BUF_SIZE_P; i++)
        sum = sum + readCircBuf (&g_inBuffer);
    return (INCREASE * sum + BUF_SIZE_P) / INCREASE / BUF_SIZE_P;
}

void
initADC (void)
{
    initCircBuf (&g_inBuffer, BUF_SIZE_P);
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable (SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure (ADC0_BASE, SQE_NUM, ADC_TRIGGER_PROCESSOR, ZERO);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure (ADC0_BASE, SQE_NUM, ZERO, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable (ADC0_BASE, SQE_NUM);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, SQE_NUM, ADCTask);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable (ADC0_BASE, SQE_NUM);
}
int32_t
ADCreadingInSteps (int32_t cur_goal)
// If the goal is not above the default max the dial acts from 0-firstmax if it is it acts as a neg-pos dial
//either add or subtracting from the current goal and if its at its actual max it acts from ((max-firstmax) - max.
{
    if (cur_goal < CHANGE) {
        return ((ADCgetReading () * UNIT / RES) * UNIT);
    } else if (cur_goal == LIMIT) {
        return cur_goal - (MAX_DOWN - ((ADCgetReading () * UNIT / RES) * UNIT));
    } else if ((((ADCgetReading () * UNIT / RES) * UNIT) - CHANGE + cur_goal) > LIMIT) {
        return LIMIT;
    } else {
        return ((ADCgetReading () * UNIT / RES) * UNIT) - CHANGE + cur_goal;
    }

}
