/*
 * readAcc.h
 *
 *  Created on: 28/03/2022
 *      Author: jtask
 */

#ifndef READACC_H_
#define READACC_H_

#include <stdint.h>
#include "circBufT.h"

typedef struct vector_xyz_s{
    int16_t x;
    int16_t y;
    int16_t z;
} vector_xyz;

typedef struct acc_xyz_s{
    circBuf_t x;
    circBuf_t y;
    circBuf_t z;
} acc_xyz;

void initAccl (void);

vector_xyz getAcclData (void);


#endif /* READACC_H_ */
