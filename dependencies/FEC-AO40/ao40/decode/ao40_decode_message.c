/*
 * Viterbi encoder based on SPIRAL (http://spiral.ece.cmu.edu/vitgen/) decoder
 * Reed-Solonom decoder is Phil Karn work
 */

/*
 * Revised and modified by szabolor
 * 2015, 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ao40_decode_message.h"

const uint8_t ao40_Scrambler[320] = {
  0xff, 0x48, 0x0e, 0xc0, 0x9a, 0x0d, 0x70, 0xbc, 0x8e, 0x2c, 0x93, 0xad, 0xa7, 0xb7, 0x46, 0xce,
  0x5a, 0x97, 0x7d, 0xcc, 0x32, 0xa2, 0xbf, 0x3e, 0x0a, 0x10, 0xf1, 0x88, 0x94, 0xcd, 0xea, 0xb1,
  0xfe, 0x90, 0x1d, 0x81, 0x34, 0x1a, 0xe1, 0x79, 0x1c, 0x59, 0x27, 0x5b, 0x4f, 0x6e, 0x8d, 0x9c,
  0xb5, 0x2e, 0xfb, 0x98, 0x65, 0x45, 0x7e, 0x7c, 0x14, 0x21, 0xe3, 0x11, 0x29, 0x9b, 0xd5, 0x63,
  0xfd, 0x20, 0x3b, 0x02, 0x68, 0x35, 0xc2, 0xf2, 0x38, 0xb2, 0x4e, 0xb6, 0x9e, 0xdd, 0x1b, 0x39,
  0x6a, 0x5d, 0xf7, 0x30, 0xca, 0x8a, 0xfc, 0xf8, 0x28, 0x43, 0xc6, 0x22, 0x53, 0x37, 0xaa, 0xc7,
  0xfa, 0x40, 0x76, 0x04, 0xd0, 0x6b, 0x85, 0xe4, 0x71, 0x64, 0x9d, 0x6d, 0x3d, 0xba, 0x36, 0x72,
  0xd4, 0xbb, 0xee, 0x61, 0x95, 0x15, 0xf9, 0xf0, 0x50, 0x87, 0x8c, 0x44, 0xa6, 0x6f, 0x55, 0x8f,
  0xf4, 0x80, 0xec, 0x09, 0xa0, 0xd7, 0x0b, 0xc8, 0xe2, 0xc9, 0x3a, 0xda, 0x7b, 0x74, 0x6c, 0xe5,
  0xa9, 0x77, 0xdc, 0xc3, 0x2a, 0x2b, 0xf3, 0xe0, 0xa1, 0x0f, 0x18, 0x89, 0x4c, 0xde, 0xab, 0x1f,
  0xe9, 0x01, 0xd8, 0x13, 0x41, 0xae, 0x17, 0x91, 0xc5, 0x92, 0x75, 0xb4, 0xf6, 0xe8, 0xd9, 0xcb,
  0x52, 0xef, 0xb9, 0x86, 0x54, 0x57, 0xe7, 0xc1, 0x42, 0x1e, 0x31, 0x12, 0x99, 0xbd, 0x56, 0x3f,
  0xd2, 0x03, 0xb0, 0x26, 0x83, 0x5c, 0x2f, 0x23, 0x8b, 0x24, 0xeb, 0x69, 0xed, 0xd1, 0xb3, 0x96,
  0xa5, 0xdf, 0x73, 0x0c, 0xa8, 0xaf, 0xcf, 0x82, 0x84, 0x3c, 0x62, 0x25, 0x33, 0x7a, 0xac, 0x7f,
  0xa4, 0x07, 0x60, 0x4d, 0x06, 0xb8, 0x5e, 0x47, 0x16, 0x49, 0xd6, 0xd3, 0xdb, 0xa3, 0x67, 0x2d,
  0x4b, 0xbe, 0xe6, 0x19, 0x51, 0x5f, 0x9f, 0x05, 0x08, 0x78, 0xc4, 0x4a, 0x66, 0xf5, 0x58, 0xff,
  0x48, 0x0e, 0xc0, 0x9a, 0x0d, 0x70, 0xbc, 0x8e, 0x2c, 0x93, 0xad, 0xa7, 0xb7, 0x46, 0xce, 0x5a,
  0x97, 0x7d, 0xcc, 0x32, 0xa2, 0xbf, 0x3e, 0x0a, 0x10, 0xf1, 0x88, 0x94, 0xcd, 0xea, 0xb1, 0xfe,
  0x90, 0x1d, 0x81, 0x34, 0x1a, 0xe1, 0x79, 0x1c, 0x59, 0x27, 0x5b, 0x4f, 0x6e, 0x8d, 0x9c, 0xb5,
  0x2e, 0xfb, 0x98, 0x65, 0x45, 0x7e, 0x7c, 0x14, 0x21, 0xe3, 0x11, 0x29, 0x9b, 0xd5, 0x63, 0xfd,
};

/* Deinterleave data: 
 * 
 * - The bits of every interleaved byte is spread with 80 bit distance of each 
 *   other. This function reorder these bits to be neighboring again.
 * - The first interleaved column (65 byte) is the SYNC_POLY (0x48), ommit it (thus i=1).
 * - The CCSDS standard using CONV_POLY_B (0x6d) in inverted format, but
 *   ao40_viterbi decoder assumes non-inverted bits, so invert every second bit 
 *   by hand.
 */
void ao40_deinterleave(uint8_t raw[AO40_RAW_SIZE], uint8_t conv[AO40_CONV_SIZE]) {
  uint16_t i = 1;
  uint16_t j = 0;

  while (i != (AO40_RAW_SIZE + 79)) {
    if (i >= AO40_RAW_SIZE) {
      i -= (AO40_RAW_SIZE - 1);
    }
    conv[j] = raw[i];
    i += 80;
    ++j;
  }
}

/* Viterbi decoder:
 *   It uses the one generated from http://www.spiral.net/
 */
void ao40_viterbi(uint8_t conv[AO40_CONV_SIZE], uint8_t dec_data[AO40_RS_SIZE]) {
  struct ao40_v *vp;
  AO40_COMPUTETYPE conv_compute[AO40_CONV_SIZE];
  int i;

  if((vp = ao40_create_viterbi(AO40_FRAMEBITS)) == AO40_NULL){
    printf("ao40_create_viterbi failed\n");
    exit(1);
  }

  ao40_init_viterbi(vp, 0);

  // AO40_COMPUTETYPE is set to uint32, so convert uint8 to uint32
  // but the softbit value should be between 0 and 255 (!)
  i = AO40_CONV_SIZE;
  while (i--) {
    conv_compute[i] = conv[i];
  }

  ao40_update_viterbi_blk(vp, conv_compute, AO40_FRAMEBITS+(AO40_K-1));
  ao40_chainback_viterbi(vp, dec_data, AO40_FRAMEBITS, 0);

  ao40_delete_viterbi(vp);
}

void ao40_descramble_and_deinterleave(uint8_t dec_data[AO40_RS_SIZE], uint8_t rs[2][AO40_RS_BLOCK_SIZE]) {
  uint16_t i;
  uint16_t j = 0;

  for (i = 0; i < AO40_RS_BLOCK_SIZE; ++i) {
    rs[0][i] = dec_data[j] ^ ao40_Scrambler[j];
    ++j;
    rs[1][i] = dec_data[j] ^ ao40_Scrambler[j];
    ++j;
  }
}

void ao40_rs_decode(uint8_t rs[2][AO40_RS_BLOCK_SIZE], uint8_t data[AO40_DATA_SIZE], int8_t error[2]) {
  uint16_t i;

  error[0] = ao40_decode_rs_8(rs[0], AO40_NULL, 0);
  error[1] = ao40_decode_rs_8(rs[1], AO40_NULL, 0);

  for (i = 0; i < AO40_DATA_SIZE; ++i) {
    data[i] = rs[i & 1][i >> 1];
  }

}

void ao40_decode_data(uint8_t raw[AO40_RAW_SIZE], uint8_t data[AO40_DATA_SIZE], int8_t error[2]) {
  uint8_t conv[AO40_CONV_SIZE];
  uint8_t dec_data[AO40_RS_SIZE];
  uint8_t rs[2][AO40_RS_BLOCK_SIZE];

  ao40_deinterleave(raw, conv);
  ao40_viterbi(conv, dec_data);
  ao40_descramble_and_deinterleave(dec_data, rs);
  ao40_rs_decode(rs, data, error);
}

void ao40_decode_data_debug(
    uint8_t raw[AO40_RAW_SIZE],        // Data to be decoded, 5200 byte (soft bit format)
    uint8_t data[AO40_DATA_SIZE],      // Decoded data, 256 byte
    int8_t  error[2],             // RS decoder modules corrected errors or -1 if unrecoverable error happened
    uint8_t conv[AO40_CONV_SIZE],      // Deinterleaved data with SYNC removed (5132 byte, soft bit format)
    uint8_t dec_data[AO40_RS_SIZE],    // Viterbi decoder output (320 byte): two RS codeblock interleaved and scrambled(!)
    uint8_t rs[2][AO40_RS_BLOCK_SIZE]  // RS codeblocks without the leading padding 95 zeros
  ) {
  ao40_deinterleave(raw, conv);
  ao40_viterbi(conv, dec_data);
  ao40_descramble_and_deinterleave(dec_data, rs);
  ao40_rs_decode(rs, data, error);
}