/* Copyright (C) Miklos Maroti 2015-2016 */

#ifndef RA_DECODER_GEN_H
#define RA_DECODER_GEN_H

#include "ra_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void ra_decoder_gen(float *softbits, ra_word_t *packet, int passes);

#ifdef __cplusplus
}
#endif

#endif // RA_DECODER_GEN_H
