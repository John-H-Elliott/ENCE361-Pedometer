/*
 * accData.c
 *
 *  Created on: 9/05/2022
 *      Author: Johne
 */

#include <math.h>
#include "accData.h"
#include "circBufT.h"
#include "refCal.h"
#include "readAcc.h"
#include "display.h"

bool
accIsStepping (angles ref_pr, vector_xyz ave_raw)
{
    static bool can_step = false;



    vector_xyz gravity = {ZERO, ZERO, G_RES};
    float roll = ref_pr.roll;
    float pitch = ref_pr.pitch;

    vector_xyz gravity_pitched = {gravity.x, gravity.y * cosf (pitch) + gravity.z * sinf (pitch), - gravity.y * sinf (pitch) + gravity.z * cosf (pitch)};
    vector_xyz gravity_rolled = {gravity_pitched.x * cosf (roll) - gravity_pitched.z * sinf (roll) , gravity_pitched.y, - gravity_pitched.x * sinf (roll) + gravity_pitched.z * cosf (roll)};

    vector_xyz balanced_acc = {ave_raw.x - gravity_rolled.x, ave_raw.y - gravity_rolled.y, ave_raw.z - gravity_rolled.z};

    uint16_t mag = balanced_acc.x * balanced_acc.x + balanced_acc.y * balanced_acc.y + balanced_acc.z * balanced_acc.z;


    bool stepped = mag > STEP_LIMIT;

    bool valid_step = false;

    if (can_step && stepped) {
        valid_step = true;
        can_step = false;
    } else if (mag < UNDER_LIMIT) {
        can_step = true;
    }

    return valid_step;
}

void
accTask (vector_xyz* xyz, acc_xyz* acc_data)
{
    vector_xyz acceleration_xyz = getAcclData(); //Reads the acceleration raw
    writeCircBuf (&(acc_data->x), acceleration_xyz.x); //Adds the raw data to the buffers
    writeCircBuf (&(acc_data->y), acceleration_xyz.y);
    writeCircBuf (&(acc_data->z), acceleration_xyz.z);

    //Sums the buffers
    int32_t x_buffered_sum = ZERO, y_buffered_sum = ZERO, z_buffered_sum = ZERO;
    uint16_t i;
    for (i = ZERO; i < BUF_SIZE; i++) {
        x_buffered_sum += readCircBuf (&(acc_data->x));
        y_buffered_sum += readCircBuf (&(acc_data->y));
        z_buffered_sum += readCircBuf (&(acc_data->z));
    }
    //Overwrites xyz with buffer averages
    xyz -> x = (SIZE_INCREMENT * x_buffered_sum + BUF_SIZE) / SIZE_INCREMENT / BUF_SIZE;
    xyz -> y = (SIZE_INCREMENT * y_buffered_sum + BUF_SIZE) / SIZE_INCREMENT / BUF_SIZE;
    xyz -> z = (SIZE_INCREMENT * z_buffered_sum + BUF_SIZE) / SIZE_INCREMENT / BUF_SIZE;
}

