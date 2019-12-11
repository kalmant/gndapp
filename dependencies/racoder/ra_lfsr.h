/* Copyright (C) Miklos Maroti 2015-2016 */

#ifndef RA_LFSR_H
#define RA_LFSR_H

#include "ra_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void ra_lfsr_init(uint8_t seqno);
ra_index_t ra_lfsr_next(void);
ra_index_t ra_lfsr_prev(void);

#ifdef __cplusplus
}
#endif

#endif // RA_LFSR_H
