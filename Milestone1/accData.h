/*
 * accData.h
 *
 *  Created on: 9/05/2022
 *      Author: Johne
 */

#ifndef ACCDATA_H_
#define ACCDATA_H_

#include <stdint.h>
#include "display.h"
#include "readAcc.h"

#define BUF_SIZE        15
#define RESOLUTION      256
#define G_RES           245
#define STEP_LIMIT      2000
#define UNDER_LIMIT     1950
#define GRAVITY         9.81
#define SIZE_INCREMENT  2


bool
accIsStepping (angles, vector_xyz);

void
accTask (vector_xyz*, acc_xyz*);



#endif /* ACCDATA_H_ */
