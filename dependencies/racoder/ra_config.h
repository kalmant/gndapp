/* Copyright (C) Miklos Maroti 2015-2016 */

#ifndef RA_CONFIG_H
#define RA_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t  ra_word_t;  /* use uint16_t for SSE4.1 soft decoder */
typedef uint16_t ra_index_t; /* use uint16_t for RA_PACKET_LENGTH >= 256 */

enum {
  /* number of words, must be at least 4 */
  RA_MAX_DATA_LENGTH = 2048,

  /* 1 for rate 1/4, 2 for 2/5, 3 for 1/2, 5 for 5/8 */
  RA_PUNCTURE_RATE = 3,

  /* use the test program to verify it */
  RA_MAX_CODE_LENGTH = RA_MAX_DATA_LENGTH * 2 + 3,
};

extern ra_index_t ra_data_length;
extern ra_index_t ra_code_length;
extern ra_index_t ra_chck_length;
extern uint16_t ra_lfsr_masks[4];
extern uint8_t ra_lfsr_highbit;

/* data length in words */
void ra_length_init(ra_index_t data_length);

enum { RA_BITCOUNT = 8 * sizeof(ra_word_t), RA_BITSHIFT = RA_BITCOUNT - 1 };

#ifdef __cplusplus
}
#endif

#endif // RA_CONFIG_H
