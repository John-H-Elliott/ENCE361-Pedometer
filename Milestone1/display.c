/*
 * display.c
 *
 *  Created on: 29/03/2022
 *      Author: Johne
 */

#include <stdint.h>
#include <math.h>
#include "i2c_driver.h"
#include "OrbitOLEDInterface.h"
#include "display.h"
#include "utils/ustdlib.h"
#include "refCal.h"

/*********************************************************
 * initDisplay
 *********************************************************/
void
initDisplay (void)
{
    // Initialise the Orbit OLED display
    OLEDInitialise ();
}


void
cleanScreen (void) {
    OLEDStringDraw ("                ", ZERO, HEADER);
    OLEDStringDraw ("                ", ZERO, FIRST);
    OLEDStringDraw ("                ", ZERO, SECOND);

}

void
cleanBottom (void) {
    OLEDStringDraw ("                ", ZERO, THIRD);
}

/*
 * Depending on the screen and units being shown and used the display will show the related information.
 *
 * There are 3 screens GOAL, STEPS and DIST.
 * For STEPS and DIST there are two units it can be shown in Raw and km and a percentage of the goal and miles.
 */
void
displayScreenTask (screen cur_screen, int32_t steps, int32_t goal, int32_t dis_goal, units unit)
{
    char header_text_buffer[MAX];           //Display fits 16 characters wide.
    char body_text_buffer[MAX];             //Display fits 16 characters wide.
    uint32_t dist = steps*STEP_TO_DIST;
    cleanScreen();
    switch (cur_screen){
    case Steps:
        usnprintf(header_text_buffer, sizeof(header_text_buffer), "Steps:");
        if (unit == RawKM) {
            usnprintf(body_text_buffer, sizeof(body_text_buffer), "%d steps", steps);
        } else {
            uint16_t perc;
            uint16_t rem;
           if (goal == ZERO) {
                usnprintf(body_text_buffer, sizeof(body_text_buffer), "%d %%", UNIT);
            } else {
                perc = ((float)steps / (float)goal) * UNIT;
                rem = (((float)steps / (float)goal) * UNIT - perc) * UNIT;
                usnprintf(body_text_buffer, sizeof(body_text_buffer), "%d.%02d %%", perc, rem);
            }
        }
        break;
    case Distance:
        usnprintf(header_text_buffer, sizeof(header_text_buffer), "Distance:");
        uint16_t whole;
        uint16_t rem;
        if (unit == RawKM) {
            if (dist == ZERO) {
                usnprintf(body_text_buffer, sizeof(body_text_buffer), "0 km");
            } else {
                whole = ((float)dist / UNIT);
                rem = ((((float)dist) / UNIT) - whole) * UNIT;
                usnprintf(body_text_buffer, sizeof(body_text_buffer), "%d.%02d km", whole, rem);
            }
        } else {
            if (dist == ZERO) {
                usnprintf(body_text_buffer, sizeof(body_text_buffer), "0 mi");
            } else {
                whole = (((float)dist / UNIT) / MILE);
                rem = ((((float)dist / UNIT) / MILE) - whole) * UNIT;
                usnprintf(body_text_buffer, sizeof(body_text_buffer), "%d.%02d mi", whole, rem);
            }

        }
        break;
    case Goal:
        usnprintf(header_text_buffer, sizeof(header_text_buffer), "Goal: %d", goal);
        usnprintf(body_text_buffer, sizeof(body_text_buffer), "New Goal: %d", dis_goal);
        break;
    }
    // Update line on display.
    OLEDStringDraw (header_text_buffer, ZERO, HEADER);
    OLEDStringDraw (body_text_buffer, ZERO, FIRST);
}


void
displayGoalReached ()
{ // Small screen to show the goals been reached.
    cleanScreen ();
    cleanBottom ();
    char text_buffer[MAX];           //Display fits 16 characters wide.

    usnprintf(text_buffer, sizeof(text_buffer), "GOAL REACHED!");
    // Update line on display.
    OLEDStringDraw (text_buffer, ZERO, SECOND);

}


bool
loadingScreen (bool reset) // Loading screen. It will return true when completed and can be reset to start again.
{
    static uint8_t index = ZERO;

    if (!reset) {
        char text_buffer[MAX];           //Display fits 16 characters wide.
        //OLEDStringDraw ("                ", ZERO, THIRD);
        usnprintf(text_buffer, sizeof(text_buffer), "|");
        // Update line on display.
        OLEDStringDraw (text_buffer, index/DECREASE, THIRD);
        if (index/DECREASE != MAX_COL) {
            index++;
            return false;
        } else {
            return true;
        }
    } else {
        index = ZERO;
        return false;
    }
}
