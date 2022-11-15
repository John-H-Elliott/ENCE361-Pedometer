/*
 * buttonTasks.h
 *
 *  Created on: 4/05/2022
 *      Author: Johne
 */

#ifndef BUTTONTASKS_H_
#define BUTTONTASKS_H_

#include <stdbool.h>
#include "display.h"
#include "refCal.h"
#include "readAcc.h"

#define NEXT_SCREEN 1
#define INCREASE_STEP 100
#define DECREASE_STEP 500
#define INCREASE_DIST 9
#define DECREASE_DIST 45

void
buttonUnitTask (units*);

void
buttonScreenTask (screen*);

bool
buttonAngleTask (vector_xyz, angles*);

bool
buttonDownHeld (bool);

void
buttonTestTask (int32_t*);


#endif /* BUTTONTASKS_H_ */
