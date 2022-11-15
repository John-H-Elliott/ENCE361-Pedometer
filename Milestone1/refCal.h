/*
 * refCal.h
 *
 *  Created on: 29/03/2022
 *      Author: Johne
 */

#ifndef REFCAL_H_
#define REFCAL_H_

#include <stdint.h>
#include "readAcc.h"

#define NEGATIVE    -1
#define POSTIVE     1
#define MIU         0.1

typedef struct gravity_s {
    float x;
    float y;
    float z;
} gravity;

typedef struct angles_s {
    float roll;
    float pitch;
} angles;

angles
pitchRoll (gravity);

angles
getRefPR (vector_xyz);


#endif /* REFCAL_H_ */
