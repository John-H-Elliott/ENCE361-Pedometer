/*
 * display.h
 *
 *  Created on: 29/03/2022
 *      Author: Johne
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "refCal.h"

#define ZERO            0
#define HEADER          0
#define UNIT            100
#define FIRST           1
#define SECOND          2
#define THIRD           3
#define MAX             17
#define MAX_COL         16
#define DECREASE        2
#define MILE            1.609
#define STEP_TO_DIST    0.09

typedef enum units_e{
    RawKM, PercentM
} units;

typedef enum screen_e{
    LoopStart, Steps, Goal, Distance, LoopEnd
} screen;

void
initDisplay (void);

void
cleanScreen (void);

void
displayScreenTask (screen, int32_t, int32_t, int32_t, units);

bool
loadingScreen(bool);

void
cleanBottom (void);

void
displayGoalReached (void);

#endif /* DISPLAY_H_ */
