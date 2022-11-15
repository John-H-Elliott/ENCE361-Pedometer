/*
 * buttonTasks.c
 *
 *  Created on: 4/05/2022
 *      Author: Johne
 */

#include <stdint.h>
#include "buttonTasks.h"
#include "buttons4.h"
#include "display.h"

void
buttonUnitTask (units* cur_unit) // Changes the units currently being used.
{
    updateButtons ();
    uint8_t but_state_up = checkButton (UP);
    if (but_state_up == RELEASED) {
        switch(*cur_unit) {
        case RawKM:
            *cur_unit = PercentM;
            break;
        case PercentM:
            *cur_unit = RawKM;
            break;
        }
    }
}

bool
buttonAngleTask (vector_xyz ave_raw, angles* ref_pr) // Will recalculate the angles of the board/
{
    updateButtons ();
    uint8_t but_state_down = checkButton (DOWN);
    if (but_state_down == RELEASED) {
        *ref_pr = getRefPR(ave_raw);
        return true;
    }
    return false;
}

void
buttonScreenTask (screen* cur_screen) // Changes the current screen being viewed.
{
    updateButtons ();
    uint8_t but_state_left = checkButton (LEFT);
    uint8_t but_state_right = checkButton (RIGHT);
    if (but_state_right == RELEASED) {
        if ((*cur_screen + NEXT_SCREEN) == LoopEnd) {
            *cur_screen = Steps;
        } else {
            *cur_screen += NEXT_SCREEN;
        }
    }
    if (but_state_left == RELEASED) {
        if ((*cur_screen - NEXT_SCREEN) == LoopStart) {
            *cur_screen = Distance;
        } else {
            *cur_screen -= NEXT_SCREEN;
        }
    }
}

bool
buttonDownHeld (bool reset) // Checks to see if he button is being held or not.
{
    static bool wasPushed = false;
    if (reset) {
        wasPushed = false;
    } else {
        uint8_t but_state_down;
        updateButtons ();
        but_state_down = checkButton (DOWN);
        if (but_state_down == RELEASED) {
            wasPushed = false;
            return false;
         } else if (wasPushed || (but_state_down == PUSHED)) {
            wasPushed = true;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void
buttonTestTask (int32_t* steps) // Changes the buttons during test mode to add steps, (and therefore distance).
{
    updateButtons ();
    uint8_t but_state_up = checkButton (UP);
    uint8_t but_state_down = checkButton (DOWN);

    if (but_state_up == RELEASED) {
        *steps += INCREASE_STEP;
    }
    if (but_state_down == RELEASED) {
        if (*steps >= DECREASE_STEP) {
            *steps -= DECREASE_STEP;
        } else {
            *steps = ZERO;
        }
    }

}

