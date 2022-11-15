/*
 * refCal.c
 *
 *  Created on: 29/03/2022
 *      Author: Johne
 */


#include "refCal.h"
#include <math.h>
#include "display.h"
#include "readACC.h"

angles
pitchRoll (gravity g_matrix) // Equations to calculate the reference angles of the board.
{
   int8_t sign = NEGATIVE;
   if (g_matrix.z > ZERO) {
       sign = POSTIVE;
   }

   float sqrt_r = sqrtf((g_matrix.z * g_matrix.z) + (g_matrix.x * g_matrix.x * MIU));
   float roll = -(atan2f(g_matrix.y, sign*sqrt_r));

   float sqrt_p = sqrtf((g_matrix.y * g_matrix.y) + (g_matrix.z * g_matrix.z));
   float pitch = (atan2f((g_matrix.x), sqrt_p));

   angles both_angles = {roll, pitch};
   return both_angles;
}

angles
getRefPR (vector_xyz acc_raw_xyz) {
    // x and y are switched as to get desired values at certain positions.
    gravity snap_shot_g = {(float) acc_raw_xyz.y, (float) acc_raw_xyz.x, (float) acc_raw_xyz.z};
    angles pitch_roll = pitchRoll(snap_shot_g);
    return pitch_roll;
}
