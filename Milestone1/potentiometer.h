// *******************************************************
//
// potentiomemter.h
//
// Support for setting up the ADC device using simple commands
// J. Tasker
// Created: 12/05/2022
//
// *******************************************************

#ifndef POTENTIOMETER_H_
#define POTENTIOMETER_H_

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE_P      5
#define SQE_NUM         3
#define INCREASE        2
#define RES             4096
#define CHANGE          5000
#define MAX_DOWN        9900
#define LIMIT           100000

void
initADC (void);

int32_t
ADCgetReading (void);

void
ADCTask (void);

int32_t
ADCreadingInSteps (int32_t);

#endif /*POTENTIOMETER_H_*/
